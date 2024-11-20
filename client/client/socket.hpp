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

    QTcpSocket* tcpSocketPtr();

private:
    QTcpSocket* _tcpSocket;
    QString _host;
    int _port;

    qint64 _expectedLength;
    QByteArray _buffer;

public slots:
    void connectToHost();
    void slotReadyRead();
    void sendToServer(const QByteArray& message);

signals:
    void processResponse(const QJsonObject& data);

};
