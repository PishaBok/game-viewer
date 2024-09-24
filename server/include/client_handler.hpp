#pragma once

#include <thread>
#include <QObject>
#include <QTcpSocket>
#include <QEventLoop>

#include "request.hpp"


class ClientHandler : public QObject
{
    Q_OBJECT
public:
    ClientHandler(DatabaseManager* dbManager, QObject* parent = nullptr);

    void newCLient(qintptr socketDescriptor);

private:
    DatabaseManager* _dbManager;
    std::map<qintptr, std::unique_ptr<QTcpSocket>> _clientList;  

    std::unique_ptr<QTcpSocket> initClient(qintptr socketDescriptor, QEventLoop* loop);
};