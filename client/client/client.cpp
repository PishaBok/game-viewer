#include <client/client.hpp>

Client::Client(PageCommand* pageCommand, FilterCommand* filterCommand, Invoker* invoker, QWidget *parent)
    : QMainWindow(parent), _pageCommand{pageCommand}, _filterCommand{filterCommand}, _invoker{invoker},
    _searchColumns { Column::gameName, Column::publisher, Column::genre},
    _filterColumns { Column::gameName, Column::platform, Column::year, Column::genre, Column::publisher,
                    Column::criticscore, Column::rating },
    _commandMap
    {
        {Button::page, _pageCommand},
        {Button::filter, _filterCommand}
    }
{
    _searchWidget = createSearchWidget();
    _pageLabel = createPageLabel();
    _filterLine = createFilterLine();
    _pageWidget = createPageWidget();
    _menuBar = createMenuBar();
    _toolBar = createToolBar();

    QWidget* page = new QWidget;
    page->setObjectName("page");
    _mainLayout = new QVBoxLayout;
    _mainLayout->addWidget(_filterLine);
    _mainLayout->addWidget(_pageWidget);
    page->setLayout(_mainLayout);

    setCentralWidget(page);
    setMenuBar(_menuBar);
    addToolBar(_toolBar);
    resize(750, 660);
}

Client::~Client()
{}


QLineEdit *Client::createPageLabel() const
{
    QLineEdit* pageLabel = new QLineEdit(QString("%1/%2").arg("1", "ND"));
    pageLabel->setMaximumHeight(35);
    pageLabel->setReadOnly(true);
    pageLabel->setAlignment(Qt::AlignCenter);
    pageLabel->setObjectName("toolBarLineEdit");

    return pageLabel;
}

SearchWidget *Client::createSearchWidget() const
{
    return new SearchWidget(columnTitles(_searchColumns));
}

QWidget *Client::createFilterLine() const
{
    QWidget* filterLine = new QWidget;
    filterLine->setMaximumHeight(30);
    filterLine->setVisible(false);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(new QLabel("Filter1"));
    layout->addWidget(new QLabel("Filter2"));
    layout->setContentsMargins(0, 0, 0, 0);

    filterLine->setLayout(layout);

    return filterLine;
}
QWidget *Client::createPageWidget() const
{
    QWidget* pageWidget = new QWidget;

    QGridLayout* layout = new QGridLayout;
    layout->setSpacing(10);
    layout->setContentsMargins(0, 0, 0, 0);
    pageWidget->setLayout(layout);

    return pageWidget;
}

QMenuBar *Client::createMenuBar()
{
    QMenuBar* menuBar = new QMenuBar;

    QMenu* fileMenu = new QMenu("Файл");
    QAction* openAct = new QAction("Открыть");

    QMenu* walkTo = new QMenu("Переход");
    QAction* walkToAction = new QAction("Перейти к странице...");
    _buttonMap[walkToAction] = Button::page;
    //connect(walkToAction, &QAction::triggered, this, &Client::buttonPressed);

    QMenu* filter = new QMenu("Фильтр");
    QAction* filterAction = new QAction("Задать фильтр...");
    _buttonMap[filterAction] = Button::filter;
    connect(filterAction, &QAction::triggered, [this, filterAction]()
    {
        std::unique_ptr<FilterDialog> dialog = std::make_unique<FilterDialog>(columnTitles(_filterColumns));
        if (dialog->exec() == QDialog::Accepted)
        {
            _filterCommand->setFilter(stringMapToColumn(dialog->data()));
            buttonPressed(filterAction);
        }
    });

    //Заполняет меню бар элементами
    fileMenu->addAction(openAct);
    walkTo->addAction(walkToAction);
    filter->addAction(filterAction);
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(walkTo);
    menuBar->addMenu(filter);

    return menuBar;
}

QToolBar *Client::createToolBar()
{
    QToolBar* toolBar = new QToolBar;

    QPushButton* stepBack = new QPushButton(QString::fromUtf8("\u2190"));
    _buttonMap[stepBack] = Button::page;
    connect(stepBack, &QPushButton::clicked, [this, stepBack]()
    {
        _pageCommand->setPage(_pageLabel->text().left(_pageLabel->text().indexOf("/")).toInt() - 1);
        buttonPressed(stepBack);
    });

    QPushButton* stepForward = new QPushButton(QString::fromUtf8("\u2192"));
    _buttonMap[stepForward] = Button::page;
    connect(stepForward, &QPushButton::clicked, [this, stepForward]()
    {
        _pageCommand->setPage(_pageLabel->text().left(_pageLabel->text().indexOf("/")).toInt() + 1);
        buttonPressed(stepForward);
    });

    QPushButton* search = new QPushButton(QString(QChar(0xD83D)) + QChar(0xDD0D));
    //_buttonsMap[search] = Button::search;
    connect(search, &QPushButton::clicked, [this]()
    {

    });

    toolBar->addWidget(stepBack);
    toolBar->addWidget(stepForward);
    toolBar->addWidget(_pageLabel);
    toolBar->addWidget(_searchWidget);
    toolBar->addWidget(search);

    return toolBar;
}

QStringList Client::columnTitles(const std::vector<Column> &columns) const
{
    QStringList titles;

    for (const Column column: columns)
    {
        titles.append(QString::fromStdString(columnToTitleMap.at(column)));
    }

    return titles;
}

std::map<Column, QString> Client::stringMapToColumn(const std::map<QString, QString> stringMap) const
{
    std::map<Column, QString> resultMap;

    for (const auto& el: stringMap)
    {
        resultMap[columnFromTitleMap.at(el.first.toStdString())] = el.second;
    }

    return resultMap;
}

void Client::callDialog() const
{
    std::unique_ptr<FilterDialog> dialog = std::make_unique<FilterDialog>(columnTitles(_filterColumns));

    if (dialog->exec() == QDialog::Accepted)
    {
        
    }
}

void Client::clearLayout(QLayout* layout)
{
    while(QLayoutItem* item = layout->takeAt(0))
    {
        if (QWidget* widget = item->widget())
        {
            widget->deleteLater();
        }
        delete item;
    }
}

void Client::updatePageCounter(const QString &strForLabel)
{
    _pageLabel->setText(strForLabel);
}

void Client::buttonPressed(QObject* sender)
{
    Command* command = _commandMap[_buttonMap[sender]];
    if (command != nullptr)
    {
        _invoker->setCommand(command);
        _invoker->runCommand();
    }
}


void Client::updatePage(const clib::TableModel &model)
{
    clearLayout(_pageWidget->layout());
    QGridLayout* layout = qobject_cast<QGridLayout*>(_pageWidget->layout());

    for (int i = 0; i < model.rowCount(); ++i)
    {
        GameCard* gameCard = new GameCard({model.data(i, "gamename").toString(), model.data(i, "platform").toString(), model.data(i, "year").toInt(),
                                            model.data(i, "genre").toString(), model.data(i, "criticscore").toInt(), model.data(i, "rating").toString()});
        layout->addWidget(gameCard, i / 3, i % 3);
    }
}
