#include <client/client.hpp>

Client::Client(PageCommand* pageCommand, FilterCommand* filterCommand, Invoker* invoker, QWidget *parent)
    : QMainWindow(parent), _pageCommand{pageCommand}, _filterCommand{filterCommand}, _invoker{invoker},
    _pageNumberStr{"1"}, _pageCountStr{"ND"},
    _searchColumns {Column::gameName, Column::publisher, Column::genre},
    _filterColumns {Column::gameName, Column::platform, Column::year, Column::genre, Column::publisher,
                    Column::criticscore, Column::rating}
{
    _searchLabel = createSearchWidget();
    _pageLabel = createPageLabel();
    _searchFrame = createSearchFrame();
    _activeFilter = createActiveFilter();
    _pageWidget = createPageWidget();
    _menuBar = createMenuBar();
    _toolBar = createToolBar();

    QWidget* page = new QWidget;
    page->setObjectName("page");
    _mainLayout = new QVBoxLayout;
    _mainLayout->setSpacing(6);
    _mainLayout->setContentsMargins(0, 0, 0, 0);

    _mainLayout->addWidget(_searchFrame);
    _mainLayout->addWidget(_activeFilter);
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
    QLineEdit* pageLabel = new QLineEdit(QString("%1/%2").arg(_pageNumberStr, _pageCountStr));
    pageLabel->setFixedSize(80, 25);
    pageLabel->setReadOnly(true);
    pageLabel->setAlignment(Qt::AlignCenter);
    pageLabel->setObjectName("toolBarLineEdit");

    return pageLabel;
}

SearchLabel *Client::createSearchWidget() const
{
    return new SearchLabel(columnTitles(_searchColumns));
}

QFrame *Client::createSearchFrame() const
{
    QFrame* searchFrame = new QFrame;
    searchFrame->setObjectName("searchFrame");
    searchFrame->setVisible(false);
    
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setAlignment(Qt::AlignmentFlag::AlignRight);
    layout->setContentsMargins(5, 5, 5, 5);

    QSpinBox* searchBox = new QSpinBox;
    searchBox->setObjectName("searchBox");
    searchBox->setFixedSize(65, 18);
    searchBox->setFocusPolicy(Qt::NoFocus);

    layout->addWidget(searchBox);

    searchFrame->setLayout(layout);

    return searchFrame;
}

QFrame *Client::createActiveFilter() const
{
    QFrame* filterLine = new QFrame;
    filterLine->setMaximumHeight(30);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    filterLine->setLayout(layout);

    return filterLine;
}

QWidget *Client::createPageWidget() const
{
    QWidget* pageWidget = new QWidget;

    QGridLayout* layout = new QGridLayout;
    layout->setContentsMargins(10, 0, 10, 10);
    pageWidget->setLayout(layout);

    return pageWidget;
}

QPushButton* Client::createPushButton(const QString& title, Command* command)
{
    QPushButton* button = new QPushButton(title);
    button->setObjectName("toolButton");
    _buttons.push_back(button);

    _senderToCommand[button] = command;
   
    return button;
}

QMenuBar *Client::createMenuBar()
{
    QMenuBar* menuBar = new QMenuBar;

    QMenu* walkTo = new QMenu("Переход");
    QAction* walkToAction = new QAction("Перейти к странице...");
    _senderToCommand[walkToAction] = _pageCommand;
    connect(walkToAction, &QAction::triggered, [this, walkToAction]()
    {
        std::unique_ptr<PageDialog> dialog = std::make_unique<PageDialog>();
        if (dialog->exec() == QDialog::Accepted)
        {
            _pageCommand->setPage(dialog->data());
            commandTriggered(walkToAction);
        }
    });

    //Заполняет меню бар элементами
    walkTo->addAction(walkToAction);
    menuBar->addMenu(walkTo);

    return menuBar;
}

QToolBar *Client::createToolBar()
{
    QToolBar* toolBar = new QToolBar;

    QPushButton* stepBack = createPushButton(QString::fromUtf8("\u2190"), _pageCommand);
    connect(stepBack, &QPushButton::clicked, [this, stepBack]()
    {
        _pageCommand->setPage(_pageNumberStr.toInt() - 1);
        commandTriggered(stepBack);
    });

    QPushButton* stepForward = createPushButton(QString::fromUtf8("\u2192"), _pageCommand);
    connect(stepForward, &QPushButton::clicked, [this, stepForward]()
    {
        _pageCommand->setPage(_pageNumberStr.toInt() + 1);
        commandTriggered(stepForward);
    });

    QPushButton* search = createPushButton(QString(QChar(0xD83D)) + QChar(0xDD0D), _filterCommand);
    connect(search, &QPushButton::clicked, [this]()
    {

    });

    QPushButton* filter = createPushButton("Фильтр", _filterCommand);
    filter->setMinimumWidth(65);
    connect(filter, &QPushButton::clicked, [this, filter]()
    {
        std::unique_ptr<FilterDialog> dialog = std::make_unique<FilterDialog>(_filterColumns);
        if (dialog->exec() == QDialog::Accepted)
        {
            _filterCommand->setFilter(dialog->data());
            commandTriggered(filter);
        }
    });

    toolBar->addWidget(stepBack);
    toolBar->addWidget(stepForward);
    toolBar->addWidget(_pageLabel);
    toolBar->addWidget(_searchLabel);
    toolBar->addWidget(search);
    toolBar->addWidget(filter);

    toolBar->setMovable(false);

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


void Client::updatePageCounter(const QString &pageNumber, const QString& pageCount)
{
    _pageNumberStr = pageNumber;
    _pageCountStr = pageCount;
    _pageLabel->setText(QString("%1/%2").arg(_pageNumberStr, _pageCountStr));
}

void Client::updateActiveFilter(const std::map<Column, FilterParams> &filters)
{
    clearLayout(_activeFilter->layout());
    QHBoxLayout* layout = static_cast<QHBoxLayout*>(_activeFilter->layout());
    for (const auto& filter: filters)
    {
        QString filterValue = QString::fromStdString(filter.second.value);

        if (filterValue.isEmpty()) {continue;}
        QLabel* label = new QLabel(QString("%1: %2").arg(QString::fromStdString(columnToTitleMap.at(filter.first)), filterValue));
        layout->addWidget(label);
    }

    _activeFilter->setVisible(!filters.empty());
}

void Client::commandTriggered(QObject* sender)
{
    Command* command = _senderToCommand.at(sender);
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

void Client::setEnabledButtons(const bool flag)
{
    for(auto& button: _buttons)
    {
        button->setEnabled(flag);
    }
}
