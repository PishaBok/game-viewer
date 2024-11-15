#pragma once

#include <thread>
#include <future>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQueryModel>
#include <QEventLoop>
#include <QSqlError>
#include <libcommon/functions.hpp>
#include <libcommon/columns.hpp>
#include <libcommon/request_types.hpp>

struct ConnectionParams
{
    QString database;
    QString host;
    QString user;
    QString password;
};

struct QueryParams
{
    RequestType request;
    std::map<Column, QString> filter;
    int limit;
    int offset;
};


class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(ConnectionParams conParams, QObject* parent = nullptr); 
    ~DatabaseManager();

    // Интерфейс
    bool connect(const QString& conName);
    void disconnect(const QString& connection);
    std::unique_ptr<QSqlQuery> getQuery(const QString& connectionName, const std::pair<std::string, std::string>& sqlQuery, const QueryParams& params);

    //bool compare(const QSqlQuery& data, const std::map<QString, QVariant>& search);
private:
    // Настройки менеджера
    ConnectionParams _dbParams;
    QString _mainConnection;

    std::mutex _connectionsMutex;


    QString getQueryStr(const std::string queryTemplate, const std::string orderBy, const QueryParams& params) const;
    void bindValues(QSqlQuery& query, const std::map<Column, QString>& values); 
};