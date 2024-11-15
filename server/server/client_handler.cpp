#include <server/client_handler.hpp>


ClientHandler::ClientHandler(qintptr socketDescriptor, DatabaseManager& dbManager, QObject* parent) 
    : QObject(parent), _socket{std::make_unique<QTcpSocket>(this)}, _descriptor{socketDescriptor}, _dbManager{dbManager}
{
    _socket->setSocketDescriptor(socketDescriptor);

    connect(_socket.get(), &QTcpSocket::readyRead, this, &ClientHandler::slotReadyRead);
    connect(_socket.get(), &QTcpSocket::disconnected, this, &ClientHandler::socketDisconnected);

}

void ClientHandler::newRequest(const QJsonObject& requestObj)
{
    RequestHandler requestHandler{_dbManager};
    QObject::connect(&requestHandler, &RequestHandler::requestCompleted, this, &ClientHandler::sendResponse);

    requestHandler.setRequest(requestObj);
    requestHandler.processRequest();




    // QThread* thread = new QThread;

    // // Создание requestHandler
    // RequestHandler* requestHandler = new RequestHandler(_dbManager);
    // requestHandler->setRequest(requestObj);
    // requestHandler->moveToThread(thread);

    // // Подключаем сигналы для управления потоком
    // QObject::connect(thread, &QThread::started, requestHandler, &RequestHandler::processRequest);
    // QObject::connect(requestHandler, &RequestHandler::requestCompleted, this, &ClientHandler::sendResponse);
    // QObject::connect(requestHandler, &RequestHandler::requestCompleted, thread, &QThread::quit);
    // QObject::connect(requestHandler, &RequestHandler::requestCompleted, requestHandler, &QObject::deleteLater);
    // QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    // thread->start();
}

void ClientHandler::slotReadyRead()
{
    while (_socket->bytesAvailable() > 0) 
    {
        QByteArray line = _socket->readLine();
        
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
                std::thread{&ClientHandler::newRequest, this, QJsonDocument::fromJson(_buffer).object()}.detach();
                _buffer.clear();
                _expectedLength = 0; // Сбрасываем
            }
        }
    }
}

void ClientHandler::sendResponse(const QJsonDocument& response)
{
    QByteArray jsonData{response.toJson()};
    qint64 length{jsonData.size()};
    QByteArray lengthData = QByteArray::number(length).append('\n');

    _socket->write(lengthData);
    _socket->write(jsonData);
}

void ClientHandler::socketDisconnected()
{
    emit finished(QString::number(_descriptor));
}

