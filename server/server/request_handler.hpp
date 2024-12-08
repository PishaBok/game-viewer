#pragma once

#include <QObject>

#include <server/request_executors/page.hpp>
#include <server/request_executors/page_count.hpp>
#include <server/request_executors/search.hpp>
#include <server/request_executors/unique_values.hpp>

class RequestHandler: public QObject
{
    Q_OBJECT
public:
    RequestHandler(DatabaseManager& dbManager, QObject* parent = nullptr);


    void setRequest(const QJsonObject& json);
private:
    DatabaseManager& _dbManager;

    std::map<RequestType, std::function<std::unique_ptr<Request>(DatabaseManager&)>> _requestFactory
    {
        {RequestType::page, [](DatabaseManager& dbManager) {return std::make_unique<PageExecutor>(dbManager);}},
        {RequestType::pageCount, [](DatabaseManager& dbManager) {return std::make_unique<PageCountExecutor>(dbManager);}},
        {RequestType::uniqueValues, [](DatabaseManager& dbManager) {return std::make_unique<UniqueValuesExecutor>(dbManager);}},
        {RequestType::search, [](DatabaseManager& dbManager) {return std::make_unique<SearchExecutor>(dbManager);}}
    };

    std::unique_ptr<Request> _requestInProcess;
    std::atomic<bool> _running;
public slots:
    void processRequest();
signals:
    void requestCompleted(const QJsonDocument& response);
};
