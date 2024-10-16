#include <client/client_engine.hpp>


ClientEngine::ClientEngine(QObject *parent)
    : QObject{parent}, _socket("localhost", 9999)
{
    _socket.slotSendToServer();
}
