#include <server/request_executors/page_count.hpp>

PageCountExecutor::PageCountExecutor(DatabaseManager& dbManager)
    : _dbManager{dbManager}
{}

std::unique_ptr<Response> PageCountExecutor::execute()
{
    std::unique_ptr<PageCountResponse> response{std::make_unique<PageCountResponse>(pageCount())};

    return response;
}


size_t PageCountExecutor::pageCount() const
{
    return ceil(_dbManager.recordCount("game", _filter) / static_cast<double>(_recordsOnPage));
}