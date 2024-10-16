#pragma once

#include <QMainWindow>
#include <QTableView>
#include <QToolBar>
#include <QMenuBar>
#include <QPushButton>
#include <QHeaderView>

class Client : public QMainWindow
{
    Q_OBJECT
public:
    Client(QWidget* parent = nullptr);

private:
    QMenuBar* _menuBar;
    QToolBar* _toolBar;
    QTableView* _view;

    QMenuBar* createMenuBar() const;
    QToolBar* createToolBar() const;

public slots:
    void updateView(QAbstractTableModel* model);
};

