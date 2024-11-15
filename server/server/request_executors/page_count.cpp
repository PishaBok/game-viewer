#include <server/request_executors/page_count.hpp>

PageCountExecutor::PageCountExecutor(DatabaseManager& dbManager)
    : _dbManager{dbManager}, _sqlQueryTemplate{"SELECT COUNT(*) FROM game WHERE 1 = 1"}
{}

std::unique_ptr<Response> PageCountExecutor::execute()
{
    std::unique_ptr<PageCountResponse> response{std::make_unique<PageCountResponse>(pageCount())};

    return response;
}


size_t PageCountExecutor::pageCount() const
{
    QString connectionName{"PageCount_" + clib::generateRandomString()};
    auto query{_dbManager.getQuery(connectionName, {_sqlQueryTemplate, ""}, 
                {RequestType::pageCount, _filter, 0, 0})};
    query->exec();

    size_t rowCount{0};

    if (query->next())
    {
        rowCount = query->value(0).toUInt(); 
    }

    return ceil(rowCount / static_cast<double>(_recordsOnPage));
}