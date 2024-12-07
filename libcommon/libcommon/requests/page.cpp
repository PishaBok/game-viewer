#include <libcommon/requests/page.hpp>

PageRequest::PageRequest()
    : Request(RequestType::page)
{
}

PageRequest::PageRequest(const int pageN, const std::map<Column, FilterParams>& filter)
    : Request(RequestType::page), _page{pageN}, _filter{filter}
{}

QJsonObject PageRequest::serialize() const
{
    QJsonObject result;
    result["type"] = static_cast<int>(_typeId);

    QJsonObject paramsObject;
    paramsObject["page"] = _page;
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

void PageRequest::deserialize(const QJsonObject &data)
{
    if (!validate(data.value("params").toObject())) 
    {
        qDebug() << "Wrong Json!";
        return;
    }

    QJsonObject paramsJson = data.value("params").toObject();

    _page = paramsJson.value("page").toInt();

    QJsonArray filterValues = paramsJson.value("filter").toArray();
    for (const QJsonValue& jsonValue: filterValues)
    {
        QJsonObject valueObj = jsonValue.toObject();
        Column column = static_cast<Column>(valueObj.value("column").toInt());
        FilterParams params = {valueObj.value("value").toString().toStdString(), static_cast<CompareType>(valueObj.value("compareType").toInt())};

        _filter.insert({column, params});
    }
}

bool PageRequest::validate(const QJsonObject &jsonObj)
{
    if (!jsonObj.contains("page") || !jsonObj.contains("filter"))
    {
        return false;
    }

    return true;
}




PageResponse::PageResponse()
    : Response(RequestType::page)
{}

PageResponse::PageResponse(const std::map<int, clib::TableModel> &pages)
    : Response(RequestType::page), _pages{pages}
{}

QJsonObject PageResponse::serialize() const
{
    QJsonObject result;
    result["type"] = static_cast<int>(_typeId);

    QJsonObject dataObj;
    QJsonArray jsonArray;
    for(const auto& [page, model]: _pages)
    { 
        QJsonObject modelObj;
        modelObj.insert("page", page);
        modelObj.insert("model", model.toJson());
        jsonArray.append(modelObj);
    }
    dataObj.insert("models", jsonArray);


    result["data"] = dataObj;
    return result;
}

void PageResponse::deserialize(const QJsonObject& jsonObject)
{
    _pages.clear();

    QJsonObject dataObj = jsonObject.value("data").toObject();
    for (const QJsonValue& jsonValue: dataObj.value("models").toArray())
    {
        QJsonObject valueObject = jsonValue.toObject();
        clib::TableModel model;
        model.loadJson(valueObject.value("model").toObject());
        _pages[valueObject.value("page").toInt()] = std::move(model);
    }
}


