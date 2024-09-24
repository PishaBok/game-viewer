#include "database_manager.hpp"

DatabaseManager::DatabaseManager(ConnectionParams conParams, QObject* parent) : 
    QObject(parent), _params{conParams},
    _queryString {R"(
    SELECT name, url, country, description, year, industry, employees 
    FROM organization 
    WHERE (country = :country)

    ORDER BY name, url, country, description, year, industry, employees
    )"}
{}

QString DatabaseManager::getQueryStr() const
{
    return _queryString;
}

bool DatabaseManager::connect(const QString &conName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", conName);

    db.setDatabaseName(_params.database);
    db.setHostName(_params.host);
    db.setUserName(_params.user);
    db.setPassword(_params.password);

    return db.open();
}
