#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSqlError>

#include "database_manager.hpp"
#include "response.hpp"

enum ReqType
{
    page,
    filter,
    search,
    maxValue
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
    Request(const QJsonObject& data, DatabaseManager* dbManager, QObject* parent = nullptr);

    Response process();

private:
    DatabaseManager* _dbManager;
    ReqType _reqType;
    Params _params;


    

    // PARSE JSON
    void parseJson(const QJsonObject& json);
    std::map<QString, QVariant> jsonToMap(const QJsonObject& json) const;
    QVariant parseTypedValue(const QJsonObject& typedValueObj) const;
    bool validateJson(const QJsonObject& json) const;

    // PROCESS REQEST
    TableModel page();

    void bindValues(QSqlQuery& query, const std::map<QString, QVariant>& values);

};