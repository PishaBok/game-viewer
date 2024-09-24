#pragma once

#include <QMainWindow>
#include <QTableView>

#include "socket.hpp"

class Window : public QMainWindow
{
    Q_OBJECT
public:
    Window(QWidget* parent = nullptr);

private:
    Socket _socket;
    QTableView* _view;

public slots:
    void updateView(QAbstractTableModel* model);
};

