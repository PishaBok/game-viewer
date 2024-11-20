#include <client/socket.hpp>

Socket::Socket(const QString &host, const int port, QObject *parent)
    : QObject(parent), _host{host}, _port{port}, _expectedLength{0}, _buffer{}
{
    _tcpSocket = new QTcpSocket(this);
    connect(_tcpSocket, &QTcpSocket::readyRead, this, &Socket::slotReadyRead);
}

Socket::~Socket()
{
    _tcpSocket->disconnectFromHost();
    _tcpSocket->deleteLater();
}

void Socket::connectToHost()
{
    _tcpSocket->connectToHost(_host, _port);
}

QTcpSocket* Socket::tcpSocketPtr()
{
    return _tcpSocket;
}

void Socket::slotReadyRead()
{
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
                emit processResponse(QJsonDocument::fromJson(_buffer).object());
                _buffer.clear();
                _expectedLength = 0; // Сбрасываем
                break;
            }
        }
    }
}


void Socket::sendToServer(const QByteArray& message)
{
    qint64 length{message.size()};
    QByteArray lengthData = QByteArray::number(length).append('\n');

    _tcpSocket->write(lengthData);
    _tcpSocket->write(message);
}
