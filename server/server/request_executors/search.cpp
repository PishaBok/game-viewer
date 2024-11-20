#include <server/request_executors/search.hpp>

SearchExecutor::SearchExecutor(DatabaseManager& dbManager)
:   _dbManager{dbManager}, _sqlQueryTemplate{},
    _orderBy{}, _recordsForThread{1000}, _recordCount{}
{}

std::unique_ptr<Response> SearchExecutor::execute()
{
    _recordCount = _dbManager.recordCount("game", _filter);
    size_t threadCount = ceil(_recordCount / static_cast<double>(_recordsForThread));

    auto searchResult = startSearchThreads(threadCount);
    std::unique_ptr<SearchResponse> response{std::make_unique<SearchResponse>(searchResult)};

    return response;
}


std::pair<int, std::set<int>> SearchExecutor::startSearchThreads(const int threadCount)
{
    std::vector<std::future<std::pair<int, std::set<int>>>> threadResults;

    std::atomic<bool> running{true};
    for (int i{0}; i < threadCount; ++i)
    {
        threadResults.push_back(std::async(std::launch::async, &SearchExecutor::locate, this, 
                                QueryParams{_sqlQueryTemplate, _orderBy,_filter, _recordsForThread, _recordsForThread * i}, 
                                        std::ref(running)));
    }

    int resultRecordCount;
    std::set<int> resultPageNumbers;
    for (auto& threadRes: threadResults)
    {
        auto tempRes = threadRes.get();

        resultRecordCount += tempRes.first;
        resultPageNumbers.insert(tempRes.second.begin(), tempRes.second.end());
    }

    return {resultRecordCount, resultPageNumbers};
}

std::pair<int, std::set<int>> SearchExecutor::locate(const QueryParams& queryParams, std::atomic<bool>& running)
{
    auto query{_dbManager.getQuery(queryParams)};
    query->exec();

    int recordCount{0};
    std::set<int> pageNumbers;

    int row{queryParams.offset};
    while (query->next())
    {
        if (!running.load()) {return {0, {}};}

        if (clib::compareRecordWithSearch(query->record(), _search))
        {
            running.store(false);
            
            std::shared_future<int> lowerBound = std::async(std::launch::async, &SearchExecutor::findLowerBound, this, row);
            std::shared_future<int> higherBound = std::async(std::launch::async, &SearchExecutor::findHigherBound, this, row);

            recordCount = (higherBound.get() - lowerBound.get()) + 1;
            pageNumbers = countPages(lowerBound.get(), higherBound.get());
        }
        ++row;
    }

    _dbManager.cleanupQuery(query.get());

    return {recordCount, pageNumbers};
}

std::vector<int> SearchExecutor::find(const QueryParams& queryParams, std::atomic<int>& recordCount)
{

}

std::set<int> SearchExecutor::countPages(const std::vector<int>& records)
{
    std::set<int> pageNumbers;

    for (int record: records)
    {
        int pageNumber = record / _recordsOnPage;
        pageNumbers.insert(pageNumber);
    }
    
    return pageNumbers;
}

std::set<int> SearchExecutor::countPages(const int lowBound, const int highBound)
{
    std::vector<int> records;
    for (int i = lowBound; i <= highBound; ++i)
    {
        records.push_back(i);
    }

    return countPages(records);
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