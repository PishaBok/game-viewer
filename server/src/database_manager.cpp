#include "database_manager.hpp"

DatabaseManager::DatabaseManager(ConnectionParams conParams, QObject* parent) : 
    QObject(parent), _params{conParams},
    _queryString {R"(
    SELECT name, platform, year, genre, publisher, criticscore, developer, rating 
    FROM game 
    WHERE 1 = 1
    )"}, _orderBy{"ORDER BY id, name, platform, year, genre, publisher, criticscore, developer, rating"}
{}



bool DatabaseManager::connect(const QString &conName)
{
    std::scoped_lock<std::mutex> lock(_connectionsMutex);

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", conName);

    db.setDatabaseName(_params.database);
    db.setHostName(_params.host);
    db.setUserName(_params.user);
    db.setPassword(_params.password);

    _activeConnections.push_back(conName);

    return db.open();
}

void DatabaseManager::disconnect(const QString& connection)
{
    std::scoped_lock<std::mutex> lock(_connectionsMutex);

    auto found = std::find(_activeConnections.begin(), _activeConnections.end(), connection);
    if (!(found == _activeConnections.end()))
    {
        QSqlDatabase::database(connection).close();
        QSqlDatabase::removeDatabase(connection);
        _activeConnections.erase(found);
    }
}

std::unique_ptr<QSqlQuery> DatabaseManager::getQuery(const QString &connectionName, const QueryParams& params)
{
    auto found = std::find(_activeConnections.begin(), _activeConnections.end(), connectionName);
    if (found == _activeConnections.end())
    {
        connect(connectionName);
    }

    auto query{std::make_unique<QSqlQuery>(QSqlDatabase::database(connectionName))};

    QString queryStr{getQueryStr(params) + " LIMIT " + QString::number(params.limit) + " OFFSET " + QString::number(params.offset)};
    if (!query->prepare(queryStr)) 
    {
        qDebug() << "Ошибка подготовки запроса:" << query->lastError();
    }

    bindValues(*query, params.filter);

    return std::move(query);
}

QString DatabaseManager::getQueryStr(const QueryParams& params) const
{
    QString queryStr{_queryString};
    for (const auto& el: params.filter)
    {
        queryStr += " AND (" + el.first + " = :" + el.first + ")";
    }

    return queryStr + _orderBy;
}

void DatabaseManager::bindValues(QSqlQuery &query, const std::map<QString, QVariant> &values)
{
    for (const auto& value: values)
    {
        query.bindValue(":" + value.first, value.second);
    }
}