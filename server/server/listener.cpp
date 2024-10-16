#include <server/listener.hpp>


Listener::Listener(const int port, QTcpServer* parent) : QTcpServer(parent)
{
    if (!listen(QHostAddress::Any, port))
    {
        qDebug() << "Server Error, Unable to start the server: " + errorString();
        close();
        return;
    }
}



void Listener::incomingConnection(qintptr socketDescriptor)
{
    emit newConnection(socketDescriptor);
}

