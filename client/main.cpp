#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QTcpSocket>
#include <thread>

#include "window.hpp"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    Window wd;
    wd.show();

    return app.exec();
}