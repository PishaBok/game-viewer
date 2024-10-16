#pragma once

#include <QTcpSocket>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QDebug>

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

    clib::TableModel _model;



    QJsonObject getTestRequest();

public slots:
    void slotReadyRead();
    void slotSendToServer();

signals:
    void updateView(QAbstractTableModel* model);

};
