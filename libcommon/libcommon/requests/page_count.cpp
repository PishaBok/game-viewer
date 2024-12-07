#include <libcommon/requests/page_count.hpp>

PageCountRequest::PageCountRequest()
    : Request(RequestType::pageCount)
{}

PageCountRequest::PageCountRequest(const int recordsOnPage, const std::map<Column, FilterParams>& filter)
    : Request(RequestType::pageCount), _recordsOnPage{recordsOnPage}, _filter(filter)
{}

QJsonObject PageCountRequest::serialize() const
{
    QJsonObject result;
    result["type"] = static_cast<int>(_typeId);

    QJsonObject paramsObject;
    paramsObject["recordsOnPage"] = _recordsOnPage;

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

    result["params"] = paramsObject;

    return result;
}

void PageCountRequest::deserialize(const QJsonObject& data)
{
    if (!validate(data.value("params").toObject())) 
    {
        qDebug() << "Wrong Json!";
        return;
    }

    QJsonObject paramsJson = data.value("params").toObject();
    _recordsOnPage = paramsJson.value("recordsOnPage").toInt();

    QJsonArray filterValues = paramsJson.value("filter").toArray();
    for (const QJsonValue& jsonValue: filterValues)
    {
        QJsonObject valueObj = jsonValue.toObject();
        Column column = static_cast<Column>(valueObj.value("column").toInt());
        FilterParams params = {valueObj.value("value").toString().toStdString(), static_cast<CompareType>(valueObj.value("compareType").toInt())};

        _filter.insert({column, params});
    }
}

bool PageCountRequest::validate(const QJsonObject& jsonObj)
{
    if (!jsonObj.contains("filter"))
    {
        return false;
    }

    return true;
}




PageCountResponse::PageCountResponse()
    : Response(RequestType::pageCount)
{}

PageCountResponse::PageCountResponse(const int pageCount)
    : Response(RequestType::pageCount), _pageCount{pageCount}
{}

QJsonObject PageCountResponse::serialize() const
{
    QJsonObject result;
    result["type"] = static_cast<int>(_typeId);

    QJsonObject dataObj;
    dataObj.insert("pageCount", _pageCount);

    result["data"] = dataObj;
    return result;
}

void PageCountResponse::deserialize(const QJsonObject& jsonObject)
{
    _pageCount = 0;

    QJsonObject dataObj = jsonObject.value("data").toObject();
    _pageCount = dataObj.value("pageCount").toInt();
}
