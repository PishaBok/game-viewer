#include <iostream>
#include <csignal>
#include <QObject>
#include <QCoreApplication>
#include <QThread>


#include <server/database_manager.hpp>
#include <server/log_manager.hpp>
#include <server/listener.hpp>
#include <server/client_handler.hpp>
#include <server/request_handler.hpp>

class SignalHandler: public QObject
{
    Q_OBJECT
public:
    SignalHandler(QObject* parent = nullptr) : QObject(parent)
    {
        signal(SIGINT, SignalHandler::handleSignal);
    }

    static void handleSignal(int signum)
    {
        std::cout << "Сервер остановлен\n";
        QCoreApplication::quit();
    }
};


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    DatabaseManager dbManager{{"mydb", "localhost", "myuser", "1234"}};
    LogManager logManager{"/Users/pavel/Projects/tcpserver/server/needs/server.log"};
    Listener listener(9999);
    SignalHandler signalHandler;

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

    std::cout << "Сервер запущен. Нажмите Ctrl+C для остановки.\n";

    return app.exec();
}

#include "main.moc"