#include <libcommon/requests/unique_values.hpp>

UniqueValuesRequest::UniqueValuesRequest()
    : Request(RequestType::uniqueValues) 
{}

UniqueValuesRequest::UniqueValuesRequest(Column column)
    : Request(RequestType::uniqueValues), _column{column}
{}

QJsonObject UniqueValuesRequest::serialize() const
{
    QJsonObject result;
    result["type"] = static_cast<int>(_typeId);

    QJsonObject paramsObject;
    paramsObject["column"] = static_cast<int>(_column);

    result["params"] = paramsObject;

    return result;
}

void UniqueValuesRequest::deserialize(const QJsonObject &data)
{
    if (!validate(data.value("params").toObject())) 
    {
        qDebug() << "Wrong Json!";
        return;
    }

    QJsonObject paramsJson = data.value("params").toObject();
    _column = static_cast<Column>(paramsJson["column"].toInt());
}

bool UniqueValuesRequest::validate(const QJsonObject &jsonObj)
{
    if (!jsonObj.contains("column"))
    {
        return false;
    }

    return true;
}



UniqueValuesResponse::UniqueValuesResponse()
    : Response(RequestType::uniqueValues)
{}

UniqueValuesResponse::UniqueValuesResponse(Column column, const QStringList& uniqueValues)
    : Response(RequestType::uniqueValues), _column{column}, _uniqueValues{uniqueValues}
{}

QJsonObject UniqueValuesResponse::serialize() const
{
    QJsonObject result;
    result["type"] = static_cast<int>(_typeId);

    QJsonObject dataObj;
    dataObj["column"] = static_cast<int>(_column);
    QJsonArray valuesArray;
    for(const auto& value: _uniqueValues)
    { 
        valuesArray.append(value);
    }
    dataObj["values"] = valuesArray;

    result["data"] = dataObj;
    return result;
}

void UniqueValuesResponse::deserialize(const QJsonObject& jsonObject)
{
    _uniqueValues.clear();

    QJsonObject dataObj = jsonObject.value("data").toObject();
    _column = static_cast<Column>(dataObj["column"].toInt());
    for (const QJsonValue& jsonValue: dataObj.value("values").toArray())
    {
        _uniqueValues.append(jsonValue.toString());
    }
}