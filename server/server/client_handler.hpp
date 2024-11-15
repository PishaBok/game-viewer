#pragma once

#include <thread>
#include <QObject>
#include <QTcpSocket>
#include <QThread>

#include <server/request_handler.hpp>

class ClientHandler : public QObject
{
    Q_OBJECT
public:
    ClientHandler(qintptr socketDescriptor, DatabaseManager& dbManager, QObject* parent = nullptr);

private:
    DatabaseManager& _dbManager;
    std::unique_ptr<QTcpSocket> _socket;
    qintptr _descriptor;
    qint64 _expectedLength;
    QByteArray _buffer;
    


    void newRequest(const QJsonObject& requestObj);
private slots:
    void slotReadyRead();
    void sendResponse(const QJsonDocument& response);
    void socketDisconnected();
signals:
    void finished(const QString& clientId);
};
