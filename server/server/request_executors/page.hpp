#pragma once

#include <server/database_manager.hpp>
#include <libcommon/requests/page.hpp>

class PageExecutor: public PageRequest
{
public:
    PageExecutor(DatabaseManager& dbManager);
    virtual ~PageExecutor() = default;

    std::unique_ptr<Response> execute() override;
private:
    DatabaseManager& _dbManager;
    const std::string _sqlQueryTemplate;
    const std::string _orderBy;

    std::map<int, clib::TableModel> startPageThreads();
    clib::TableModel createPage(const int number);
};
