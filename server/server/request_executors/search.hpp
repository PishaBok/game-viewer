#pragma once

#include <server/database_manager.hpp>
#include <libcommon/requests/search.hpp>

class SearchExecutor: public SearchRequest
{
public:
    SearchExecutor(DatabaseManager& dbManager);
    virtual ~SearchExecutor() = default;

    std::unique_ptr<Response> execute() override;
    void startSearchThreads(const int threadCount);
private:
    DatabaseManager& _dbManager;
    const std::string _sqlQueryTemplate;
    const std::string _orderBy;
    const int _recordsForThread;


    std::pair<int, std::vector<int>> locate(const QString& connectionName, const QueryParams& queryParams, std::atomic<bool>& running);
    std::vector<int> find(const QString& connectionName, const QueryParams& queryParams, std::atomic<int>& recordCount);
};