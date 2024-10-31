#include <server/request_executors/page.hpp>

PageExecutor::PageExecutor(DatabaseManager &dbManager)
    : _dbManager{dbManager}, _SqlQueryTemplate{"SELECT name, platform, year, genre, publisher, criticscore, developer, rating FROM game WHERE 1 = 1"},
    _orderBy{"ORDER BY id, name, platform, year, genre, publisher, criticscore, developer, rating"}
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
    std::map<int, clib::TableModel> result;
    std::vector<std::future<std::pair<int, clib::TableModel>>> futureModels(pagesToMake);

    // Запускаем создание страниц в отдельных потоках
    for (int i{0}; i < pagesToMake; ++i)
    {
        int pageId = (_page + i) - cacheSize;
        futureModels[i] = std::async(std::launch::async, &PageExecutor::createPage, this, pageId);
    }

    // Дожидаемся создания всех страниц
    for (int i{0}; i < pagesToMake; ++i)
    {
        result.insert(futureModels[i].get());
    }

    return std::move(result);
}

std::pair<int, clib::TableModel> PageExecutor::createPage(const int number)
{
    QString connectionName{"Page_" + clib::generateRandomString()};
    auto query{_dbManager.getQuery(connectionName, {_SqlQueryTemplate, _orderBy}, {RequestType::page, _filter, 9, 9 * number})};
    query->exec();

    clib::TableModel tableModel{std::move(*query)};
    _dbManager.disconnect(connectionName);

    return {number, tableModel};
}

