#pragma once

#include <thread>
#include <future>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QEventLoop>

struct ConnectionParams
{
    QString database;
    QString host;
    QString user;
    QString password;
};

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    DatabaseManager(ConnectionParams conParams, QObject* parent = nullptr); 

    bool connect(const QString& conName);
    QString getQueryStr() const;
private:
    ConnectionParams _params;
    QString _queryString;


    
};