#include "request.hpp"

Request::Request(const QJsonObject& data, DatabaseManager* dbManager, QObject* parent) : QObject(parent),
    _dbManager{dbManager}
{
    if (!validateJson(data))
    {
        throw std::invalid_argument("Invalid JSON format in Request!");
    }

    parseJson(data);
}

void Request::parseJson(const QJsonObject &json)
{
    _reqType = static_cast<ReqType>(json["reqType"].toInt());

    QJsonObject params{json["params"].toObject()};

    _params.page = params["page"].toInt();
    _params.filter = jsonToMap(params["filter"].toObject());
    _params.search = jsonToMap(params["search"].toObject());
}

std::map<QString, QVariant> Request::jsonToMap(const QJsonObject &json) const
{
    std::map<QString, QVariant> result;

    for (const QString& key: json.keys())
    {
        QJsonObject typeValueObj = json[key].toObject();
        result[":" + key] = parseTypedValue(typeValueObj); 
    }

    return std::move(result);
}

QVariant Request::parseTypedValue(const QJsonObject& typedValueObj) const
{
    QString type = typedValueObj["type"].toString();

    if (type == "int")
    {
        return typedValueObj["value"].toInt();
    }
    else if (type == "string")
    {
        return typedValueObj["value"].toString();
    }

    QVariant data{"Str"};


    return QVariant();
}

bool Request::validateJson(const QJsonObject &json) const
{
    if (!json.contains("reqType") || !json.value("reqType").isDouble() || json.value("reqType").toInt() >= ReqType::maxValue)
    {
        return false;
    }

    if (!json.contains("params") || !json.value("params").isObject())
    {
        return false;
    }

    return true;
}

TableModel Request::page()
{
    _dbManager->connect("client");
    QString queryStr{_dbManager->getQueryStr()};
    QSqlQuery query(QSqlDatabase::database("client"));
    if (!query.prepare(queryStr)) 
    {
        qDebug() << "Ошибка подготовки запроса:" << query.lastError();
    }

    bindValues(query, _params.filter);

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError();
    }

    QSqlQueryModel model;
    model.setQuery(std::move(query));

    return TableModel{model};
}

void Request::bindValues(QSqlQuery &query, const std::map<QString, QVariant> &values)
{
    for (const auto& value: values)
    {
        query.bindValue(value.first, value.second);
    }
}

Response Request::process()
{
    Response response;

    response.setPageNumber(_params.page);
    response.setModel(page());

    return std::move(response);
}


