#pragma once

#include <thread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>

class Listener : public QTcpServer
{   
    Q_OBJECT
public:
    Listener(const int port, QTcpServer* parent = nullptr);
    
protected:
    void incomingConnection(qintptr socketDescriptor) override;

signals:
    void serverStarted();
    void newConnection(qintptr socketDescriptor);
};  