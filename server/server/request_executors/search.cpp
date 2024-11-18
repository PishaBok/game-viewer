#include <server/request_executors/search.hpp>

SearchExecutor::SearchExecutor(DatabaseManager& dbManager)
:   _dbManager{dbManager}, _sqlQueryTemplate{},
    _orderBy{}, _recordsForThread{1000}
{}

std::unique_ptr<Response> SearchExecutor::execute()
{
    size_t recordCount = _dbManager.recordCount("game", _filter);
    size_t threadCount = ceil(recordCount / static_cast<double>(_recordsForThread));

    startSearchThreads(threadCount);
}


void SearchExecutor::startSearchThreads(const int threadCount)
{
    std::vector<std::thread> threadVector(threadCount);

    std::atomic<bool> running{true};
    for (int i{0}; i < threadCount; ++i)
    {
        QString connectoinName = "SearchThread_" + QString::number(i) + clib::generateRandomString();
        threadVector[i] = std::thread{&SearchExecutor::locate, this, connectoinName, 
                                        QueryParams{_typeId, _sqlQueryTemplate, _orderBy,_filter, _recordsForThread, _recordsForThread * i}, 
                                        std::ref(running)};
    }
}

std::pair<int, std::vector<int>> SearchExecutor::locate(const QString& connectionName, const QueryParams& queryParams, std::atomic<bool>& running)
{
    auto query{_dbManager.getQuery(connectionName, queryParams)};
    query->exec();

    int recordCount{0};
    std::vector<int> pageNumbers;

    int row{queryParams.offset};
    while (query->next())
    {
        if (!running.load()) {return {0, {}};}

        if (clib::compareRecordWithSearch(query->record(), _search))
        {
            running.store(false);
        

        }
        ++row;
    }

    _dbManager.disconnect(connectionName);

    return {recordCount, pageNumbers};
}

std::vector<int> SearchExecutor::find(const QString& connectionName, const QueryParams& queryParams, std::atomic<int>& recordCount)
{

}