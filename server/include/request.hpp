#pragma once

#include <thread>
#include <future>
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSqlError>
#include <QThread>

#include "database_manager.hpp"
#include "response.hpp"
#include "functions.hpp"

enum class ReqType
{
    get,
    post,
    max
};

struct Params
{
    int page;
    std::map<QString, QVariant> filter;
    std::map<QString, QVariant> search;
};

class Request : public QObject
{
    Q_OBJECT
public:
    Request(const QJsonObject& data, DatabaseManager& dbManager, QObject* parent = nullptr);

    Response process();
    void start();
    void pause();

    bool isFinished();


private:
    DatabaseManager& _dbManager;
    ReqType _reqType;
    Params _params;

    std::atomic<bool> _running;
    bool _finished;




    // PARSE JSON
    void parseJson(const QJsonObject& json);
    std::map<QString, QVariant> jsonToMap(const QJsonObject& json) const;
    QVariant parseTypedValue(const QJsonObject& typedValueObj) const;
    bool validateJson(const QJsonObject& json) const;

    // PROCESS REQEST
    TableModel getPage(const int number);
};