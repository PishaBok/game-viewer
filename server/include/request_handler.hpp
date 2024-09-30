#pragma once

#include <QObject>


#include "request.hpp"

class RequestHandler: public QObject
{
    Q_OBJECT
public:
    RequestHandler(std::unique_ptr<Request> request, QObject* parent = nullptr);

private:
    std::unique_ptr<Request> _request;
    std::atomic<bool> _running;

public slots:
    void processRequest();


signals:
    void requestCompleted(const Response& response);
};