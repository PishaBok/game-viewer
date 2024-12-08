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

    ClientEngine engine;
    Invoker invoker;

    Client wd(&engine, &invoker); 
    QObject::connect(&engine, &ClientEngine::updatePage, &wd, &Client::updatePage);
    QObject::connect(&engine, &ClientEngine::updatePageCounter, &wd, &Client::updatePageCounter);
    QObject::connect(&engine, &ClientEngine::updateActiveFilter, &wd, &Client::updateActiveFilter);
    QObject::connect(&engine, &ClientEngine::setEnabledButtons, &wd, &Client::setEnabledButtons);
    QObject::connect(&engine, &ClientEngine::setDropListValue, &wd, &Client::setDropListValue);
    QObject::connect(&engine, &ClientEngine::searchSwitch, &wd, &Client::searchSwitch);
    QObject::connect(&engine, &ClientEngine::updateSearchCounter, &wd, &Client::updateSearchCounter);
    QObject::connect(&engine, &ClientEngine::highlightCard, &wd, &Client::highlightCard);

    engine.start();
    wd.show();

    return app.exec();
}
