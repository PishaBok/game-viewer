#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QTcpSocket>
#include <thread>

#include <client/client_engine.hpp>
#include <client/client.hpp>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    ClientEngine engine;
    Client wd;
    wd.show();

    return app.exec();
}
