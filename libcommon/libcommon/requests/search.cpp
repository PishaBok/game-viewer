#include <libcommon/requests/search.hpp>

SearchRequest::SearchRequest()
    : Request(RequestType::search)
{}

SearchRequest::SearchRequest(const int recordsOnPage, const std::map<Column, QString>& filter, const std::map<Column, QString>& search, const bool isCorrectOrder)
    : Request(RequestType::search), _recordsOnPage{recordsOnPage}, _filter{filter}, _search{search}, _isCorrectOrder{isCorrectOrder}
{}

QJsonObject SearchRequest::serialize() const
{
    QJsonObject result;
    result["type"] = static_cast<int>(_typeId);

    QJsonObject paramsObject;

    paramsObject["recordsOnPage"] = _recordsOnPage;
    QJsonArray filterArray = mapToJson(_filter);
    QJsonArray searchArray = mapToJson(_search);
    paramsObject["filter"] = filterArray;
    paramsObject["search"] = searchArray;
    paramsObject["isCorrectOrder"] = _isCorrectOrder;

    result["params"] = paramsObject;

    return result;
}

void SearchRequest::deserialize(const QJsonObject& data)
{
    if (!validate(data.value("params").toObject())) 
    {
        qDebug() << "Wrong Json!";
        return;
    }

    QJsonObject paramsJson = data.value("params").toObject();

    _recordsOnPage = paramsJson.value("recordsOnPage").toInt();
    _filter = jsonToMap(paramsJson.value("filter").toArray());
    _search = jsonToMap(paramsJson.value("search").toArray());
    _isCorrectOrder = paramsJson.value("isCorrectOrder").toBool();
}

bool SearchRequest::validate(const QJsonObject& jsonObj)
{
    if (!jsonObj.contains("filter") || !jsonObj.contains("search"))
    {
        return false;
    }

    return true;
}

QJsonArray SearchRequest::mapToJson(const std::map<Column, QString>& paramMap) const
{
    QJsonArray resultJson;
    for (const auto& [column, value]: paramMap)
    {
        QJsonObject filterObj;
        filterObj["column"] = static_cast<int>(column);
        filterObj["value"] = value;
        resultJson.append(filterObj);
    }

    return resultJson;
}

std::map<Column, QString> SearchRequest::jsonToMap(const QJsonArray& jsonArr) const
{
    std::map<Column, QString> resultMap;

    for (const QJsonValue& jsonValue: jsonArr)
    {
        QJsonObject valueObj = jsonValue.toObject();
        resultMap.insert({static_cast<Column>(valueObj.value("column").toInt()), valueObj.value("value").toString()});
    }

    return resultMap;
}




SearchResponse::SearchResponse()
    : Response(RequestType::search)
{}

SearchResponse::SearchResponse(const int recordCount, const std::set<int>& pageNumbers)
    : Response(RequestType::search), _recordCount{recordCount}, _pageNumbers{pageNumbers}
{}

SearchResponse::SearchResponse(const std::pair<int, std::set<int>>& searcResult)
    : Response(RequestType::search), _recordCount{searcResult.first}, _pageNumbers{searcResult.second}
{}

QJsonObject SearchResponse::serialize() const
{
    QJsonObject result;
    result["type"] = static_cast<int>(_typeId);

    QJsonObject dataObj;

    dataObj["recordCount"] = _recordCount;

    QJsonArray pageArray;
    for (const int pageNumber: _pageNumbers)
    {
        pageArray.append(pageNumber);
    }
    dataObj["pageNumbers"] = pageArray;

    result["data"] = dataObj;
    return result;
}

void SearchResponse::deserialize(const QJsonObject& jsonObject)
{
    _recordCount = 0;
    _pageNumbers.clear();

    QJsonObject dataObj = jsonObject.value("data").toObject();

    _recordCount = dataObj.value("recordCount").toInt();

    for (const QJsonValue& numberValue: dataObj.value("pageNumbers").toArray())
    {
        _pageNumbers.insert(numberValue.toInt());
    }
}