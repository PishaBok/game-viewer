#pragma once

#include <QObject>

#include <libcommon/request_types.hpp>
#include <server/requests/page.hpp>
#include <server/responses/page.hpp>

class RequestHandler: public QObject
{
    Q_OBJECT
public:
    RequestHandler(DatabaseManager& dbManager, QObject* parent = nullptr);


    void setRequest(const QJsonObject& json);
private:
    DatabaseManager& _dbManager;

    std::map<RequestType, std::function<std::unique_ptr<Request>(const QJsonObject&, DatabaseManager&)>> _requestFactory
    {
        {RequestType::page, [](const QJsonObject& json, DatabaseManager& dbManager) {return std::make_unique<PageRequest>(json, dbManager);}}
    };

    std::unique_ptr<Request> _request;
    std::atomic<bool> _running;

public slots:
    void processRequest();

signals:
    void requestCompleted(const QJsonDocument& response);
};
