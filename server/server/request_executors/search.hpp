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
private:
    DatabaseManager& _dbManager;
    const std::string _sqlQueryTemplate;
    const std::string _orderBy;
    std::map<Column, int> _gameColumnOrder;
    const int _recordsForThread;
    size_t _recordCount;
    std::function<std::set<int>(const QueryParams&, std::atomic<bool>&)> _searchFunc;

    std::set<int> startSearchThreads(const int threadCount);
    std::set<int> locate(const QueryParams& queryParams, std::atomic<bool>& running);
    std::set<int> find(const QueryParams& queryParams, std::atomic<bool>& running);
    std::set<int> getRecordIds(const int leftBound, const int rightBound) const;
    int findLowerBound(const int startRow);
    int findHigherBound(const int startRow);
    bool isOrderBy(const std::map<Column, QString>& search) const;
    void chooseSearchFunc();
};