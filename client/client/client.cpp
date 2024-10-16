#include <client/client.hpp>

Client::Client(QWidget *parent)
    : QMainWindow{parent}
{

    _menuBar = createMenuBar();
    _toolBar = createToolBar();
    _view = new QTableView;
    _view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    setMenuBar(_menuBar);
    addToolBar(_toolBar);
    setCentralWidget(_view);
    resize(750, 660);
}

QMenuBar *Client::createMenuBar() const
{
    QMenuBar* menuBar = new QMenuBar;

    return menuBar;
}

QToolBar *Client::createToolBar() const
{
    QToolBar* toolBar = new QToolBar;

    QPushButton* stepBack = new QPushButton(QString::fromUtf8("\u2190"));
    QPushButton* stepForward = new QPushButton(QString::fromUtf8("\u2192"));
    QPushButton* search = new QPushButton(QString(QChar(0xD83D)) + QChar(0xDD0D));

    toolBar->addWidget(stepBack);
    toolBar->addWidget(stepForward);
    toolBar->addWidget(search);


    return toolBar;
}




void Client::updateView(QAbstractTableModel* model)
{
    _view->setModel(model);
}
