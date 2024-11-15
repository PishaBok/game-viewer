#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QTcpSocket>
#include <QFile>
#include <QDir>

#include <client/client_engine.hpp>
#include <client/command.hpp>
#include <client/client.hpp>


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QFile file (":/css/style.css");
    file.open(QFile::ReadOnly);
    app.setStyleSheet(file.readAll());

    // Создание движка
    ClientEngine engine;

    PageCommand pageCommand(&engine);
    FilterCommand filterCommand(&engine);
    Invoker invoker;

    Client wd(&pageCommand, &filterCommand, &invoker); 
    QObject::connect(&engine, &ClientEngine::updatePage, &wd, &Client::updatePage);
    QObject::connect(&engine, &ClientEngine::updatePageCounter, &wd, &Client::updatePageCounter);

    engine.start();
    wd.show();

    return app.exec();
}
