#include <QCoreApplication>
#include "server.hpp"



int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    Server server(5555);

    qDebug() << "Server started!";

    return app.exec();
}