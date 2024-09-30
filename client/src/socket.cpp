#include "socket.hpp"

Socket::Socket(const QString &host, const int port, QObject *parent) 
    : QObject(parent), _expectedLength{0}, _buffer{}
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

    while (_tcpSocket->canReadLine()) 
    {
        QByteArray line = _tcpSocket->readLine();
        if (_expectedLength == 0) 
        {
            // Получаем длину сообщения
            _expectedLength = line.toLongLong();
        } 
        else 
        {
            // Читаем данные, если длина уже известна
            _buffer.append(line);
            if (_buffer.size() >= _expectedLength) 
            {
                QJsonObject jsonObj{QJsonDocument::fromJson(_buffer).object()};
                _model.loadJson(jsonObj["model"].toObject());
                emit updateView(qobject_cast<QAbstractTableModel*>(&_model));
                _buffer.clear();
                _expectedLength = 0; // Сбрасываем
                break;
            }
        }
    }
}


void Socket::slotSendToServer()
{
    QFile file("/Users/pavel/Projects/tcpserver/client/test_request.json");
    file.open(QIODevice::ReadOnly);

    QByteArray data(file.readAll());

    _tcpSocket->write(data);

    file.close();
}