#pragma once

#include <QObject>

#include <client/socket.hpp>

class ClientEngine: public QObject
{
    Q_OBJECT
public:
    ClientEngine(QObject* parent = nullptr);

private:
    Socket _socket;
};
