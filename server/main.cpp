#include <QCoreApplication>
#include <QThread>

#include "database_manager.hpp"
#include "log_manager.hpp"
#include "listener.hpp"
#include "client_handler.hpp"
#include "request_handler.hpp"


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    DatabaseManager dbManager{{"mydb", "localhost", "myuser", "1234"}};
    LogManager logManager{"/Users/pavel/Projects/tcpserver/server/needs/server.log"};
    Listener listener(5555);

    QObject::connect(&listener, &Listener::serverStarted, &logManager, &LogManager::serverStarted);


    QObject::connect(&listener, &Listener::newConnection, [&dbManager, &listener, &logManager](qintptr socketDescriptor) 
    {
        QThread* thread = new QThread;

        ClientHandler* clientHandler = new ClientHandler(socketDescriptor, dbManager);
        clientHandler->moveToThread(thread);

        // Подключаем сигналы для управления потоком
        QObject::connect(clientHandler, &ClientHandler::finished, thread, &QThread::quit);
        QObject::connect(clientHandler, &ClientHandler::finished, clientHandler, &QObject::deleteLater);
        QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);

        // Подключение сигналов для логов
        logManager.clientConnected(QString::number(socketDescriptor));
        QObject::connect(clientHandler, &ClientHandler::finished, &logManager, &LogManager::clientDisconnected);

        thread->start();
    });

    emit listener.serverStarted();

    return app.exec();
}