#include <libcommon/requests/search.hpp>

SearchRequest::SearchRequest()
    : Request(RequestType::search)
{}

SearchRequest::SearchRequest(const std::map<Column, FilterParams>& filter, const std::map<Column, QString>& search)
    : Request(RequestType::search), _filter{filter}, _search{search}
{}

QJsonObject SearchRequest::serialize() const
{
    QJsonObject result;
    result["type"] = static_cast<int>(_typeId);

    QJsonObject paramsObject;

    QJsonArray filterArray;
    for (const auto& [column, filterSetting]: _filter)
    {
        QJsonObject filterObj;
        filterObj["column"] = static_cast<int>(column);
        filterObj["value"] = QString::fromStdString(filterSetting.value);
        filterObj["compareType"] = static_cast<int>(filterSetting.type);
        filterArray.append(filterObj);
    }
    paramsObject["filter"] = filterArray;

    QJsonArray searchArray = mapToJson(_search);
    paramsObject["search"] = searchArray;

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

    QJsonArray filterValues = paramsJson.value("filter").toArray();
    for (const QJsonValue& jsonValue: filterValues)
    {
        QJsonObject valueObj = jsonValue.toObject();
        Column column = static_cast<Column>(valueObj.value("column").toInt());
        FilterParams params = {valueObj.value("value").toString().toStdString(), static_cast<CompareType>(valueObj.value("compareType").toInt())};

        _filter.insert({column, params});
    }
    _search = jsonToMap(paramsJson.value("search").toArray());
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

SearchResponse::SearchResponse(const std::set<int>& recordIds)
    : Response(RequestType::search), _recordIds{recordIds}
{}

QJsonObject SearchResponse::serialize() const
{
    QJsonObject result;
    result["type"] = static_cast<int>(_typeId);

    QJsonObject dataObj;

    QJsonArray idsArray;
    for (const int id: _recordIds)
    {
        idsArray.append(id);
    }
    dataObj["recordIds"] = idsArray;

    result["data"] = dataObj;
    return result;
}

void SearchResponse::deserialize(const QJsonObject& jsonObject)
{
    _recordIds.clear();

    QJsonObject dataObj = jsonObject.value("data").toObject();

    for (const QJsonValue& numberValue: dataObj.value("recordIds").toArray())
    {
        _recordIds.insert(numberValue.toInt());
    }
}