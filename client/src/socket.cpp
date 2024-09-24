#include "socket.hpp"

Socket::Socket(const QString &host, const int port, QObject *parent) : QObject(parent)
{
    _tcpSocket = new QTcpSocket(this);

    connect(_tcpSocket, &QTcpSocket::readyRead, this, &Socket::slotReadyRead);

    _tcpSocket->connectToHost(host, port);
}

Socket::~Socket()
{
    qDebug() << "Socket Disconnected!";
    _tcpSocket->disconnectFromHost();
}

void Socket::slotReadyRead()
{
    qDebug() << "Socket received a message";

    QByteArray data = _tcpSocket->readAll();

    QJsonObject jsonObj{QJsonDocument::fromJson(data).object()};

    qDebug() << jsonObj["page"];

    _model.loadJson(jsonObj["model"].toObject());

    emit updateView(qobject_cast<QAbstractTableModel*>(&_model));
}


void Socket::slotSendToServer()
{
    QFile file("/Users/pavel/Projects/Stilsoft/myproject/client/test_request.json");
    file.open(QIODevice::ReadOnly);

    QByteArray data(file.readAll());

    _tcpSocket->write(data);

    file.close();
}