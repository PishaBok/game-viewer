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

enum class CompareOperator
{
    equal,
    like
};

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(ConnectionParams conParams, QObject* parent = nullptr); 
    ~DatabaseManager();


    bool connect(const QString& conName);
    void disconnect(const QString& connection);
    std::unique_ptr<QSqlQuery> getQuery(const QString& connectionName, const QueryParams& params);
    bool compare(const QSqlQuery& data, const std::map<QString, QVariant>& search);
private:
    ConnectionParams _params;
    QString _mainConnection;
    std::map<QString, CompareOperator> _columnsCompareMap;
    std::map<CompareOperator, QString> _compareFunctions;

    std::vector<QString> _activeConnections;
    QString _queryString;
    QString _orderBy;

    std::mutex _connectionsMutex;

    
    void init();
    CompareOperator getOperator(int typeId);
    QString getQueryStr(const QueryParams& params) const;
    void bindValues(QSqlQuery& query, const std::map<QString, QVariant>& values); 
};