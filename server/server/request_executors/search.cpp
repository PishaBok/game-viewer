#include <server/request_executors/search.hpp>
#include "search.hpp"

SearchExecutor::SearchExecutor(DatabaseManager& dbManager)
:   _dbManager{dbManager}, _sqlQueryTemplate{"SELECT gameid, gamename, platform, year, genre, publisher, criticscore, rating FROM game WHERE 1 = 1"},
    _orderBy{"ORDER BY gamename, platform, year, genre, publisher, criticscore, rating"}, _recordsForThread{1000}, _recordCount{},
    _gameColumnOrder
    {
        {Column::gameName, 0}, {Column::platform, 1}, {Column::year, 2},
        {Column::genre, 3}, {Column::publisher, 4}, {Column::criticscore, 5},
        {Column::rating, 6}
    }
{}

std::unique_ptr<Response> SearchExecutor::execute()
{
    _recordCount = _dbManager.recordCount("game", _filter);
    size_t threadCount = ceil(_recordCount / static_cast<double>(_recordsForThread));

    auto searchResult = startSearchThreads(threadCount);
    std::unique_ptr<SearchResponse> response{std::make_unique<SearchResponse>(searchResult)};

    return response;
}

std::set<int> SearchExecutor::startSearchThreads(const int threadCount)
{
    std::vector<std::future<std::set<int>>> threadResults;
    chooseSearchFunc();

    std::atomic<bool> running{true};
    for (int i{0}; i < threadCount; ++i)
    {
        threadResults.push_back(std::async(std::launch::async, [this, i, &running]()
        {
            return _searchFunc(QueryParams{_sqlQueryTemplate, _orderBy,_filter, _recordsForThread, _recordsForThread * i}, 
                                        std::ref(running));
        }));
    }

    std::set<int> resultRecordIds;
    for (auto& threadRes: threadResults)
    {
        auto tempRes = threadRes.get();
        resultRecordIds.insert(tempRes.begin(), tempRes.end());
    }

    return resultRecordIds;
}

std::set<int> SearchExecutor::locate(const QueryParams& queryParams, std::atomic<bool>& running)
{
    auto query{_dbManager.getQuery(queryParams)};
    query->exec();

    std::set<int> recordIds;

    int row{queryParams.offset};
    while (query->next())
    {
        if (!running.load()) {break;}

        if (clib::compareRecordWithSearch(query->record(), _search))
        {
            if (!running.load()) {break;}
            running.store(false);

            qDebug() << row;
            
            std::future<int> lowerBound = std::async(std::launch::async, &SearchExecutor::findLowerBound, this, row);
            std::future<int> higherBound = std::async(std::launch::async, &SearchExecutor::findHigherBound, this, row);

            recordIds = getRecordIds(lowerBound.get(), higherBound.get());
            break;
        }
        ++row;
    }

    _dbManager.cleanupQuery(query.get());

    return recordIds;
}

std::set<int> SearchExecutor::find(const QueryParams& queryParams, std::atomic<bool>& running)
{
    auto query{_dbManager.getQuery(queryParams)};
    query->exec();

    std::set<int> recordIds;

    int row{queryParams.offset};
    while (query->next())
    {
        if (clib::compareRecordWithSearch(query->record(), _search))
        {
            recordIds.insert(row);
        }
        ++row;
    }
    _dbManager.cleanupQuery(query.get());
    return recordIds;
}

std::set<int> SearchExecutor::getRecordIds(const int leftBound, const int rightBound) const
{
    std::set<int> result;
    for (int i{leftBound}; i <= rightBound; ++i)
    {
        result.insert(i);
    }
    return result;
}

int SearchExecutor::findLowerBound(const int startRow)
{
    auto query{_dbManager.getQuery({_sqlQueryTemplate, _orderBy, _filter, 0, 0})};
    query->exec();
    QSqlQueryModel queryModel;
    queryModel.setQuery(std::move(*query));

    int boundRow{startRow - 1};

    while (boundRow >= 0)
    {
        if (!clib::compareRecordWithSearch(queryModel.record(boundRow), _search))
        {
            ++boundRow;
            break;
        }

        --boundRow;
    }

    _dbManager.cleanupQuery(query.get());

    return boundRow;
}

int SearchExecutor::findHigherBound(const int startRow)
{
    auto query{_dbManager.getQuery({_sqlQueryTemplate, _orderBy, _filter, 0, 0})};
    query->exec();
    QSqlQueryModel queryModel;
    queryModel.setQuery(std::move(*query));

    int boundRow{startRow + 1};

    while (boundRow < _recordCount)
    {
        if (!clib::compareRecordWithSearch(queryModel.record(boundRow), _search))
        {
            --boundRow;
            break;
        }

        ++boundRow;
    }

    _dbManager.cleanupQuery(query.get());

    return boundRow;
}

bool SearchExecutor::isOrderBy(const std::map<Column, QString>& search) const
{
    // Создаем отсортированный по orderBy вектор search
    std::vector<std::pair<Column, QString>> sortedSearch;
    for (const auto& pair: search)
    {
        sortedSearch.push_back(pair);
    }
    std::sort(sortedSearch.begin(), sortedSearch.end(), [this](const std::pair<Column, QString>& a, const std::pair<Column, QString>& b)
    {
        return _gameColumnOrder.at(a.first) < _gameColumnOrder.at(b.first);
    });

    // Проверяем последовательность orderBy
    bool foundEmpty{false};
    for (const auto& pair: sortedSearch)
    {
        if (pair.second.isEmpty()) {foundEmpty = true;}
        else if(foundEmpty) {return false;} // Если пустая строка уже была, а сейчас не пустая => orderBy не выполняется
    }

    return true;
}

void SearchExecutor::chooseSearchFunc()
{
    bool orderBy{isOrderBy(_search)};

    if (orderBy)
    {
        _searchFunc = std::bind(&SearchExecutor::locate, this, std::placeholders::_1, std::placeholders::_2);
    }
    else
    {
        _searchFunc = std::bind(&SearchExecutor::find, this, std::placeholders::_1, std::placeholders::_2);
    }
}
