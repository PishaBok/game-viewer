#include "client_handler.hpp"

ClientHandler::ClientHandler(DatabaseManager* dbManager, QObject *parent) : QObject(parent),
    _dbManager{dbManager}
{}


void ClientHandler::newCLient(qintptr socketDescriptor)
{
    std::thread([socketDescriptor, this]()
    {
        // Создание цикла обработки событий
        QEventLoop loop;

        _clientList[socketDescriptor] = initClient(socketDescriptor, &loop);

        // Запуск цикла обработки событий
        loop.exec();

        qDebug() << "socket disconnected!";
        _clientList[socketDescriptor]->disconnectFromHost();

        _clientList.erase(socketDescriptor);

    }).detach();
}

std::unique_ptr<QTcpSocket> ClientHandler::initClient(qintptr socketDescriptor, QEventLoop* loop)
{
    // INITIALIZE
    std::unique_ptr<QTcpSocket> socket(std::make_unique<QTcpSocket>());
    if (!socket->setSocketDescriptor(socketDescriptor))
    {
        qDebug() << "Failed to set socket descriptor: " << socketDescriptor;
    }

    // CONNECT
    connect(socket.get(), &QTcpSocket::readyRead, [dbManager = _dbManager, socketPtr = socket.get()]()
    {
        try
        {
            // Получение запроса
            QByteArray data = socketPtr->readAll();
            Request request(QJsonDocument::fromJson(data).object(), dbManager);
            // Отправка ответа
            Response response = request.process();
            qDebug() << response.toJson();
            socketPtr->write(response.toJson().toJson());
        }
        catch(const std::invalid_argument& e)
        {
            qDebug() << "Error creating request: " << e.what();
        }
    });
    connect(socket.get(), &QTcpSocket::disconnected, [loop]
    {
        loop->quit();
    });

    // QByteArray data("Hello from server!");
    // socket->write(data);

    return std::move(socket);
}
