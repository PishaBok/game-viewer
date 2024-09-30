#include "request_handler.hpp"

RequestHandler::RequestHandler(std::unique_ptr<Request> request, QObject* parent)
    : QObject{parent}, _request{std::move(request)}, _running{true}
{}

void RequestHandler::processRequest()
{
    auto future = std::async(std::launch::async, &Request::process, _request.get());

    // while (!_request->isFinished())
    // {
    //     // Тут надо стопить выполнение реквеста каждый N секунд на M секунд
    // }

    emit requestCompleted(future.get());
}