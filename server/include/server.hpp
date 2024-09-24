#pragma once

#include <thread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>

#include "client_handler.hpp"

class Server : public QTcpServer
{   
    Q_OBJECT
public:
    Server(const int port, QTcpServer* parent = nullptr);
    
protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    DatabaseManager _dbManager;
    ClientHandler _clientHandler;
};  