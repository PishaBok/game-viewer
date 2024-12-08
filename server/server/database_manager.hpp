#pragma once

#include <thread>
#include <future>
#include <sstream>
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

struct ConnectionParams
{
    QString database;
    QString host;
    QString user;
    QString password;
};

struct QueryParams
{
    std::string strQueryTemplate;
    std::string orderBy;
    std::map<Column, FilterParams> filter;
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
    QString generateConnectionName() const;
    bool connect(const QString& conName);
    void disconnect(const QString& connection);
    std::unique_ptr<QSqlQuery> getQuery(const QueryParams& params);
    std::unique_ptr<QSqlQuery> getQuery(const QString& queryStr);
    void cleanupQuery(QSqlQuery* query);
    size_t recordCount(const std::string& tableName, const std::map<Column, FilterParams> filter);
private:
    // Настройки менеджера
    ConnectionParams _dbParams;
    QString _mainConnection;
    inline static int _connectionCounter{0};

    std::map<QSqlQuery*, QString> _queryMap;
    std::mutex _connectionsMutex;


    QString getQueryStr(const QueryParams& params) const;
    QString getFilterPart(const std::pair<Column, FilterParams>& filter) const;
    void bindValue(QSqlQuery& query, const std::pair<Column, FilterParams>& filter);
};