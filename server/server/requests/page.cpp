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
        int pageId = (_page + i) - cacheSize;
        futureModels[i] = std::async(std::launch::async, &PageRequest::createPage, this, pageId);
    }

    return std::move(futureModels);
}

std::pair<int, clib::TableModel> PageRequest::createPage(const int number)
{
    QString connectionName{"Page_" + clib::generateRandomString()};
    auto query{_dbManager.getQuery(connectionName, {_filter, 9, 9 * number})};
    query->exec();

    clib::TableModel tableModel{std::move(*query)};
    _dbManager.disconnect(connectionName);

    return {number, tableModel};
}

void PageRequest::parseJson(const QJsonObject &jsonObj)
{
    _page = jsonObj.value("page").toInt();
    qDebug() << _page;
    QJsonObject filterObj{jsonObj.value("filter").toObject()};
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




PageResponse::PageResponse()
{}

QJsonDocument PageResponse::toJson() const
{
    QJsonObject result;
    result.insert("type", RequestType::page);

    QJsonArray jsonArr;
    for(const auto& [key, value]: _pages)
    {
        QJsonObject modelObj;
        modelObj.insert("page", key);
        modelObj.insert("model", value.toJson());
        jsonArr.append(modelObj);
    }

    result.insert("data", jsonArr);

    return QJsonDocument(result);
}

void PageResponse::addPage(const int pageN, const clib::TableModel &model)
{
    _pages[pageN] = model;
}