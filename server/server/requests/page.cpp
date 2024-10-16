#include <server/requests/page.hpp>

PageRequest::PageRequest(const QJsonObject &data, DatabaseManager &dbManager)
    : Request(dbManager)
{}

std::unique_ptr<Response> PageRequest::process()
{
    std::unique_ptr<PageResponse> result{std::make_unique<PageResponse>()};

    auto pages = startPageThreads();

    for(int i{0}; i < pages.size(); ++ i)
    {
        auto page = pages[i].get();
        result->addPage(page.first, page.second);
    }

    return result;
}

std::vector<std::future<std::pair<int, clib::TableModel>>> PageRequest::startPageThreads()
{
    const int cacheSize(1);
    const int pagesToMake{(cacheSize * 2) + 1};
    std::vector<std::future<std::pair<int, clib::TableModel>>> futureModels(pagesToMake);
 
    for (int i{0}; i < pagesToMake; ++i)
    {
        int pageId = _page - cacheSize + i;
        futureModels[i] = std::async(std::launch::async, &PageRequest::createPage, this, pageId);
    }

    return std::move(futureModels);
}

std::pair<int, clib::TableModel> PageRequest::createPage(const int number)
{
    QString connectionName{"Page_" + clib::generateRandomString()};
    auto query{_dbManager.getQuery(connectionName, {_filter, 15, 15 * (number - 1)})};
    query->exec();

    clib::TableModel tableModel{std::move(*query)};
    _dbManager.disconnect(connectionName);

    return {number, tableModel};
}

void PageRequest::parseJson(const QJsonObject &jsonObj)
{
    QJsonObject params{jsonObj.value("params").toObject()};

    _page = params.value("page").toInt();
    QJsonObject filterObj{params.value("filter").toObject()};
    for(const QString& key: filterObj.keys())
    {
        QJsonObject typeValueObj = filterObj.value(key).toObject();
        _filter[key] = clib::parseTypedValue(typeValueObj);
    }
}

bool PageRequest::validateJson(const QJsonObject &jsonObj)
{
    if (!jsonObj.contains("page") || !jsonObj.contains("filter"))
    {
        return false;
    }

    return true;
}
