#include "server.hpp"


Server::Server(const int port, QTcpServer* parent) : QTcpServer(parent),
    _dbManager{{"mydb", "localhost", "myuser", "1234"}}, _clientHandler{&_dbManager}
{
    if (!listen(QHostAddress::Any, port))
    {
        QMessageBox::critical(0, "Server Error",
                                "Unable to start the server: "
                                + errorString());
        close();
        return;
    }
}



void Server::incomingConnection(qintptr socketDescriptor)
{
    _clientHandler.newCLient(socketDescriptor);
}

