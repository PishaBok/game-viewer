#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QTcpSocket>
#include <QFile>
#include <QDir>

#include <client/client_engine.hpp>
#include <client/client.hpp>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QFile file (":/css/style.css");
    file.open(QFile::ReadOnly);
    app.setStyleSheet(file.readAll());

    qDebug() << "Main thread: " << QThread::currentThread();

    Client wd;

    QThread* engineThread = new QThread;
    ClientEngine* engine = new ClientEngine;
    engine->moveToThread(engineThread);

    QObject::connect(engineThread, &QThread::started, engine, &ClientEngine::initSocket);
    QObject::connect(&app, &QApplication::aboutToQuit, engineThread, &QThread::quit);
    QObject::connect(engineThread, &QThread::finished, engine, &QObject::deleteLater);
    QObject::connect(engineThread, &QThread::finished, engineThread, &QObject::deleteLater);

    QObject::connect(&wd, &Client::processButton, engine, &ClientEngine::processButton);
    QObject::connect(engine, &ClientEngine::updatePage, &wd, &Client::updatePage);

    engineThread->start();
    wd.show();

    return app.exec();
}
