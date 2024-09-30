#pragma once

#include <thread>
#include <future>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QEventLoop>
#include <QSqlError>

struct ConnectionParams
{
    QString database;
    QString host;
    QString user;
    QString password;
};

struct QueryParams
{
    std::map<QString, QVariant> filter;
    int limit;
    int offset;
};

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    DatabaseManager(ConnectionParams conParams, QObject* parent = nullptr); 

    bool connect(const QString& conName);
    void disconnect(const QString& connection);
    std::unique_ptr<QSqlQuery> getQuery(const QString& connectionName, const QueryParams& params);
private:
    ConnectionParams _params;
    std::vector<QString> _activeConnections;
    QString _queryString;
    QString _orderBy;

    std::mutex _connectionsMutex;

    
    QString getQueryStr(const QueryParams& params) const;
    void bindValues(QSqlQuery& query, const std::map<QString, QVariant>& values); 
};