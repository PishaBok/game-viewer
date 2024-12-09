#include <server/request_executors/page.hpp>

PageExecutor::PageExecutor(DatabaseManager &dbManager)
    : _dbManager{dbManager}, _sqlQueryTemplate{"SELECT gameid, gamename, platform, year, genre, publisher, criticscore, rating FROM game WHERE 1 = 1"},
    _sqlQueryIcons{"SELECT gameicon FROM game WHERE 1 = 1"},
    _orderBy{"ORDER BY gamename, genre, publisher, platform, year, criticscore, rating"}
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
    QueryParams queryParams{_sqlQueryTemplate, _orderBy, _filter, 9, 9 * number};
    auto query{_dbManager.getQuery(queryParams)};
    query->exec();
    // {
    //     for (int col{0}; col < queryModel.columnCount(); ++col)
    //     {
    //         QVariant data;
    //         if (col == imageCol) {data = getImageValue(queryModel.data(queryModel.index(row, col)).toString());}
    //         else {data = queryModel.data(queryModel.index(row, col));}
    //         tableModel.setData(tableModel.index(row, col), data);
    //     }
    // }

    clib::TableModel tableModel{std::move(*query)};
    _dbManager.cleanupQuery(query.get());

    loadIcons(tableModel, queryParams);

    return tableModel;
}

void PageExecutor::loadIcons(clib::TableModel& model, QueryParams queryParams)
{
    queryParams.strQueryTemplate = _sqlQueryIcons;
    auto query{_dbManager.getQuery(queryParams)};
    query->exec();

    for (int row{0}; query->next(); ++row)
    {
        QImage image = getImage(query->value(0).toString());
        model.setIcon(row, image);
    }

    _dbManager.cleanupQuery(query.get());
}

QImage PageExecutor::getImage(const QString& path)
{
    QImage image("../" + path);

    return image;
}

