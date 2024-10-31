#pragma once

#include <server/database_manager.hpp>
#include <libcommon/requests/page.hpp>
#include <libcommon/table_model.hpp>


class PageExecutor: public PageRequest
{
public:
    PageExecutor(DatabaseManager& dbManager);
    virtual ~PageExecutor() = default;

    std::unique_ptr<Response> execute() override;

private:
    DatabaseManager& _dbManager;
    const std::string _SqlQueryTemplate;
    const std::string _orderBy;

    std::map<int, clib::TableModel> startPageThreads();
    std::pair<int, clib::TableModel> createPage(const int number);
};
