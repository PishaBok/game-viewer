#include <server/client_handler.hpp>


ClientHandler::ClientHandler(qintptr socketDescriptor, DatabaseManager& dbManager, QObject* parent) 
    : QObject(parent), _socket{std::make_unique<QTcpSocket>(this)}, _descriptor{socketDescriptor}, _dbManager{dbManager}
{
    _socket->setSocketDescriptor(socketDescriptor);

    connect(_socket.get(), &QTcpSocket::readyRead, this, &ClientHandler::newRequest);
    connect(_socket.get(), &QTcpSocket::disconnected, this, &ClientHandler::socketDisconnected);

}

void ClientHandler::newRequest()
{
    QThread* thread = new QThread;

    // Создание requestHandler
    RequestHandler* requestHandler = new RequestHandler(_dbManager);
    requestHandler->setRequest(QJsonDocument::fromJson(_socket->readAll()).object());
    requestHandler->moveToThread(thread);

    // Подключаем сигналы для управления потоком
    QObject::connect(thread, &QThread::started, requestHandler, &RequestHandler::processRequest);
    QObject::connect(requestHandler, &RequestHandler::requestCompleted, this, &ClientHandler::sendResponse);
    QObject::connect(requestHandler, &RequestHandler::requestCompleted, thread, &QThread::quit);
    QObject::connect(requestHandler, &RequestHandler::requestCompleted, requestHandler, &QObject::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
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

// void ClientHandler::newCLient(qintptr socketDescriptor)
// {
//     std::thread([socketDescriptor, this]()
//     {
//         // Создание цикла обработки событий
//         QEventLoop loop;

//         _clientList[socketDescriptor] = initClient(socketDescriptor, &loop);

//         // Запуск цикла обработки событий
//         loop.exec();

//         qDebug() << "socket disconnected!";
//         _clientList[socketDescriptor]->disconnectFromHost();

//         _clientList.erase(socketDescriptor);

//     }).detach();
// }

// std::unique_ptr<QTcpSocket> ClientHandler::initClient(qintptr socketDescriptor, QEventLoop* loop)
// {
//     // INITIALIZE
//     std::unique_ptr<QTcpSocket> socket(std::make_unique<QTcpSocket>());
//     if (!socket->setSocketDescriptor(socketDescriptor))
//     {
//         qDebug() << "Failed to set socket descriptor: " << socketDescriptor;
//     }

//     // CONNECT
//     connect(socket.get(), &QTcpSocket::readyRead, [dbManager = _dbManager, socketPtr = socket.get()]()
//     {
//         try
//         {
//             // Получение запроса
//             QByteArray data = socketPtr->readAll();
//             Request request(QJsonDocument::fromJson(data).object(), dbManager);
//             // Отправка ответа
//             Response response = request.process();
//             socketPtr->write(response.toJson().toJson());
//         }
//         catch(const std::invalid_argument& e)
//         {
//             qDebug() << "Error creating request: " << e.what();
//         }
//     });
//     connect(socket.get(), &QTcpSocket::disconnected, [loop]
//     {
//         loop->quit();
//     });

//     // QByteArray data("Hello from server!");
//     // socket->write(data);

//     return std::move(socket);
// }
