#include <server/request_executors/page.hpp>

PageExecutor::PageExecutor(DatabaseManager &dbManager)
    : _dbManager{dbManager}, _sqlQueryTemplate{"SELECT id, gamename, platform, year, genre, publisher, criticscore, rating FROM game WHERE 1 = 1"},
    _orderBy{"ORDER BY gamename, platform, year, genre, publisher, criticscore, rating"}
{}

std::unique_ptr<Response> PageExecutor::execute()
{
    auto pages = startPageThreads();
    std::unique_ptr<PageResponse> response{std::make_unique<PageResponse>(pages)};

    return response;
}

std::map<int, clib::TableModel> PageExecutor::startPageThreads()
{
    const int cacheSize(1); // Параметр создания доп страниц, помимо основной
    const int pagesToMake{(cacheSize * 2) + 1};
    
    std::map<int, std::future<clib::TableModel>> tempPages;
    std::map<int, clib::TableModel> pages;

    // Запускаем создание страниц в отдельных потоках
    for (int i{0}; i < pagesToMake; ++i)
    {
        int pageId = (_page + i) - cacheSize;
        tempPages[pageId] = std::async(std::launch::async, &PageExecutor::createPage, this, pageId - 1);
    }

    for (auto& tempPage: tempPages)
    {
        pages[tempPage.first] = std::move(tempPage.second.get());
    }

    return std::move(pages);
}

clib::TableModel PageExecutor::createPage(const int number)
{
    QString connectionName{"Page_" + clib::generateRandomString()};
    auto query{_dbManager.getQuery(connectionName, {_sqlQueryTemplate, _orderBy}, {RequestType::page, _filter, 9, 9 * number})};
    query->exec();

    clib::TableModel tableModel{std::move(*query)};

    _dbManager.disconnect(connectionName);
    return tableModel;
}

