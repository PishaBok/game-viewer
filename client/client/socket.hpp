#pragma once

#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QDebug>
#include <QThread>

#include <libcommon/table_model.hpp>


class Socket : public QObject
{
    Q_OBJECT
public:
    Socket(const QString& host, const int port, QObject* parent = nullptr);
    ~Socket();

private:
    QTcpSocket* _tcpSocket;
    qint64 _expectedLength;
    QByteArray _buffer;

    QJsonObject _testRequest;
public slots:
    void slotReadyRead();
    void sendToServer(const QByteArray& message);

signals:
    void processResponse(const QJsonObject& data);

};
