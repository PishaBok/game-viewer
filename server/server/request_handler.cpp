#include <server/request_handler.hpp>

RequestHandler::RequestHandler(DatabaseManager& dbManager, QObject* parent)
    : QObject{parent}, _dbManager{dbManager}
{}

void RequestHandler::setRequest(const QJsonObject &json)
{
    RequestType reqType = static_cast<RequestType>(json.value("reqType").toInt());

    _request = _requestFactory[reqType](json, _dbManager);
}

void RequestHandler::processRequest()
{
    auto future = std::async(std::launch::async, &Request::process, _request.get());

    // while (!_request->isFinished())
    // {
    //     // Тут надо стопить выполнение реквеста каждый N секунд на M секунд
    // }

    emit requestCompleted(future.get()->toJson());
}
