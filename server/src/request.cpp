#include "request.hpp"

Request::Request(const QJsonObject& data, DatabaseManager& dbManager, QObject* parent) : QObject(parent),
    _dbManager{dbManager}, _running{true}, _finished{false}
{
    if (!validateJson(data))
    {
        throw std::invalid_argument("Invalid JSON format in Request!");
    }

    parseJson(data);
}

Response Request::process()
{
    Response result;

    result.setPageNumber(_params.page);
    result.setModel(page(1));

    _finished = true;
    return result;
}

void Request::start()
{
    _running.store(true);
}

void Request::pause()
{
    _running.store(false);
}

bool Request::isFinished()
{
    return _finished;
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
        result[key] = parseTypedValue(typeValueObj); 
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
    if (!json.contains("reqType") || !json.value("reqType").isDouble() || json.value("reqType").toInt() >= static_cast<int>(ReqType::max))
    {
        return false;
    }

    if (!json.contains("params") || !json.value("params").isObject())
    {
        return false;
    }

    return true;
}




TableModel Request::page(const int number)
{
    QString connectionName{generateRandomString()};

    auto query{_dbManager.getQuery(connectionName, {_params.filter, 15000, 15 * (number - 1)})};
    query->exec();

    QSqlQueryModel model;
    model.setQuery(std::move(*query));

    TableModel tableModel{model};
    _dbManager.disconnect(connectionName);

    return tableModel;
}

