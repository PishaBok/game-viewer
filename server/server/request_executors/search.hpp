#pragma once

#include <thread>

#include <server/database_manager.hpp>
#include <libcommon/requests/search.hpp>

class SearchExecutor: public SearchRequest
{
public:
    SearchExecutor(DatabaseManager& dbManager);
    virtual ~SearchExecutor() = default;

    std::unique_ptr<Response> execute() override;
    std::pair<int, std::set<int>> startSearchThreads(const int threadCount);
private:
    DatabaseManager& _dbManager;
    const std::string _sqlQueryTemplate;
    const std::string _orderBy;
    const int _recordsForThread;

    size_t _recordCount;


    std::pair<int, std::set<int>> locate(const QueryParams& queryParams, std::atomic<bool>& running);
    std::vector<int> find(const QueryParams& queryParams, std::atomic<int>& recordCount);

    std::set<int> countPages(const std::vector<int>& records);
    std::set<int> countPages(const int lowBound, const int highBound);

    int findLowerBound(const int startRow);
    int findHigherBound(const int startRow);
};