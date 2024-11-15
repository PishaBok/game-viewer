#pragma once

#include <server/database_manager.hpp>
#include <libcommon/requests/page_count.hpp>

class PageCountExecutor: public PageCountRequest
{
public:
    PageCountExecutor(DatabaseManager& dbManager);
    virtual ~PageCountExecutor() = default;

    std::unique_ptr<Response> execute() override;

private:
    DatabaseManager& _dbManager;
    const std::string _sqlQueryTemplate;


    size_t pageCount() const;
};