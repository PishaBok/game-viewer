#pragma once

#include <vector>
#include <map>
#include <thread>
#include <future>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <server/database_manager.hpp>
#include <libcommon/request_types.hpp>

class Response;

class Request
{
public:
    Request(DatabaseManager& dbManager);
    virtual ~Request() = default;

    void setJson(const QJsonObject &data);
    virtual std::unique_ptr<Response> process() = 0;

protected:
    DatabaseManager& _dbManager;


    virtual void parseJson(const QJsonObject& jsonObj) = 0;
    virtual bool validateJson(const QJsonObject& jsonObj) = 0;
};

class Response
{
public:
    Response() = default;
    virtual ~Response() = default;

    virtual QJsonDocument toJson() const = 0;

};



// #pragma once

// #include <thread>
// #include <future>
// #include <QObject>
// #include <QJsonObject>
// #include <QJsonArray>
// #include <QJsonDocument>
// #include <QSqlError>
// #include <QThread>

// #include "database_manager.hpp"
// #include "response.hpp"
// #include "functions.hpp"

// enum class ReqType
// {
//     get,
//     post,
//     max
// };

// struct Params
// {
//     int page;
//     std::map<QString, QVariant> filter;
//     std::map<QString, QVariant> search;
// };

// class Request : public QObject
// {
//     Q_OBJECT
// public:
//     Request(const QJsonObject& data, DatabaseManager& dbManager, QObject* parent = nullptr);

//     Response process();
//     bool isFinished();


// private:
//     DatabaseManager& _dbManager;
//     ReqType _reqType;
//     Params _params;

//     QString _baseConnectionName;
//     std::shared_future<int> _rowCount;
//     std::shared_future<int> _searchOffset;

//     bool _finished;




//     // PARSE JSON
//     void parseJson(const QJsonObject& json);
//     std::map<QString, QVariant> jsonToMap(const QJsonObject& json) const;
//     QVariant parseTypedValue(const QJsonObject& typedValueObj) const;
//     bool validateJson(const QJsonObject& json) const;

//     // PROCESS REQEST
//     int rowCount();
//     int startSearchThreads();
//     void locate(const QString &connectionName, const int limit, const int offset, std::atomic<int> &resVar);

//     std::vector<TableModel> startPageThreads();
//     TableModel createPage(const int number);
// };
