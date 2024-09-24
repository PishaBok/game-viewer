#include "window.hpp"

Window::Window(QWidget *parent)
    : QMainWindow{parent}, _socket("localhost", 5555)
{
    _view = new QTableView;

    resize(700, 500);
    setCentralWidget(_view);

    connect(&_socket, &Socket::updateView, this, &Window::updateView);

    _socket.slotSendToServer();
}

void Window::updateView(QAbstractTableModel* model)
{
    _view->setModel(model);
}
