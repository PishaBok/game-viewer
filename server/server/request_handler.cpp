#include <server/request_handler.hpp>

RequestHandler::RequestHandler(DatabaseManager& dbManager, QObject* parent)
    : QObject{parent}, _dbManager{dbManager}
{}

void RequestHandler::setRequest(const QJsonObject &json)
{
    // Создаем запрос согласно типу "type"
    RequestType reqType = static_cast<RequestType>(json.value("type").toInt());
    _requestInProcess = _requestFactory[reqType](_dbManager);
    // Устанавливаем параметры в созданный запрос из поля "params"
    _requestInProcess->deserialize(json);
}

void RequestHandler::processRequest()
{
    auto future = std::async(std::launch::async, &Request::execute, _requestInProcess.get());

    // while (!_request->isFinished())
    // {
    //     // Тут надо стопить выполнение реквеста каждый N секунд на M секунд
    // }

    emit requestCompleted(QJsonDocument{future.get()->serialize()});
}
