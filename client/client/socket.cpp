#include <client/socket.hpp>

Socket::Socket(const QString &host, const int port, QObject *parent) 
    : QObject(parent), _expectedLength{0}, _buffer{}
{
    _tcpSocket = new QTcpSocket(this);
    connect(_tcpSocket, &QTcpSocket::readyRead, this, &Socket::slotReadyRead);
    _tcpSocket->connectToHost(host, port);
}

Socket::~Socket()
{
    qDebug() << "Socket Disconnected!" << " Sended from thread: " << QThread::currentThread();
    _tcpSocket->disconnectFromHost();
    _tcpSocket->deleteLater();
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
                emit dataReceived(QJsonDocument::fromJson(_buffer).object());
                _buffer.clear();
                _expectedLength = 0; // Сбрасываем
                break;
            }
        }
    }
}


void Socket::sendToServer(const QByteArray& message)
{
    _tcpSocket->write(message);
}
