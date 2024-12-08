#include <client/client.hpp>
#include "client.hpp"

Client::Client(ClientEngine* engine, Invoker* invoker, QWidget *parent)
    : QMainWindow(parent), _engine{engine}, _invoker{invoker},
    _pageNumberStr{"1"}, _pageCountStr{"ND"}, _searchNumberStr{"1"}, _searchCountStr{"ND"}, 
    _searchColumns {Column::gameName, Column::publisher, Column::genre},
    _filterColumns {Column::gameName, Column::platform, Column::year, Column::genre, Column::publisher,
                    Column::criticscore, Column::rating}
{
    _searchLabel = createSearchWidget();
    _pageInfoLabel = createPageLabel();
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
    return new SearchLabel(_searchColumns);
}

QFrame *Client::createSearchFrame()
{
    QFrame* searchFrame = new QFrame;
    searchFrame->setObjectName("searchFrame");
    searchFrame->setVisible(false);
    
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setAlignment(Qt::AlignmentFlag::AlignRight);
    layout->setSpacing(5);
    layout->setContentsMargins(5, 5, 5, 5);

    QPushButton* spinBoxLeftButton = new QPushButton("<");
    spinBoxLeftButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(spinBoxLeftButton, &QPushButton::clicked, [this]
    {
        auto command = std::make_shared<SearchRecordCommand>(_engine, _searchNumberStr.toInt() - 1);
        newCommand(command);
    });
    _searchInfoLabel = new QLineEdit(QString("%1/%2").arg(_searchNumberStr, _searchCountStr));
    _searchInfoLabel->setMaximumWidth(60);
    _searchInfoLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QPushButton* spinBoxRightButton = new QPushButton(">");
    spinBoxRightButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(spinBoxRightButton, &QPushButton::clicked, [this]
    {
        auto command = std::make_shared<SearchRecordCommand>(_engine, _searchNumberStr.toInt() + 1);
        newCommand(command);
    });

    QPushButton* doneButton = new QPushButton("Готово");
    connect(doneButton, &QPushButton::clicked, [this]
    {
        auto command = std::make_shared<SearchOffCommand>(_engine);
        newCommand(command);
    });

    layout->addWidget(spinBoxLeftButton);
    layout->addWidget(_searchInfoLabel);
    layout->addWidget(spinBoxRightButton);
    layout->addWidget(doneButton);

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

QPushButton* Client::createPushButton(const QString& title)
{
    QPushButton* button = new QPushButton(title);
    button->setObjectName("toolButton");
    _buttons.push_back(button);

    return button;
}

QMenuBar *Client::createMenuBar()
{
    QMenuBar* menuBar = new QMenuBar;

    QMenu* walkTo = new QMenu("Переход");
    QAction* walkToAction = new QAction("Перейти к странице...");
    connect(walkToAction, &QAction::triggered, [this, walkToAction]()
    {
        std::unique_ptr<PageDialog> dialog = std::make_unique<PageDialog>();
        if (dialog->exec() == QDialog::Accepted)
        {
            auto command = std::make_shared<PageCommand>(_engine, dialog->data());
            newCommand(command);
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

    QPushButton* stepBack = createPushButton(QString::fromUtf8("\u2190"));
    connect(stepBack, &QPushButton::clicked, [this]()
    {
        auto command = std::make_shared<PageCommand>(_engine, _pageNumberStr.toInt() - 1);
        newCommand(command);
    });

    QPushButton* stepForward = createPushButton(QString::fromUtf8("\u2192"));
    connect(stepForward, &QPushButton::clicked, [this]()
    {
        auto command = std::make_shared<PageCommand>(_engine, _pageNumberStr.toInt() + 1);
        newCommand(command);
    });

    QPushButton* search = createPushButton(QString(QChar(0xD83D)) + QChar(0xDD0D));
    connect(search, &QPushButton::clicked, [this]()
    {
        auto command = std::make_shared<SearchOnCommand>(_engine, _searchLabel->data());
        newCommand(command);
    });

    QPushButton* filter = createPushButton("Фильтр");
    filter->setMinimumWidth(65);
    connect(filter, &QPushButton::clicked, [this]()
    {
        std::unique_ptr<FilterDialog> dialog = std::make_unique<FilterDialog>(_filterColumns, _dropListValues);
        if (dialog->exec() == QDialog::Accepted)
        {
            auto command = std::make_shared<FilterCommand>(_engine, dialog->data());
            newCommand(command);
        }
    });

    toolBar->addWidget(stepBack);
    toolBar->addWidget(stepForward);
    toolBar->addWidget(_pageInfoLabel);
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
    _pageInfoLabel->setText(QString("%1/%2").arg(_pageNumberStr, _pageCountStr));
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

void Client::newCommand(std::shared_ptr<Command> command)
{
    if (command.get() != nullptr)
    {
        _invoker->setCommand(command);
        _invoker->runCommand();
    }
}


void Client::updatePage(const clib::TableModel &model)
{
    clearLayout(_pageWidget->layout());
    _activeCards.clear();

    QGridLayout* layout = qobject_cast<QGridLayout*>(_pageWidget->layout());
    for (int i = 0; i < model.rowCount(); ++i)
    {
        GameCard* gameCard = new GameCard({model.data(i, "gamename").toString(), model.data(i, "platform").toString(), model.data(i, "year").toInt(),
                                            model.data(i, "genre").toString(), model.data(i, "criticscore").toInt(), model.data(i, "rating").toString()});
        layout->addWidget(gameCard, i / 3, i % 3);
        _activeCards.insert({i, gameCard});
    }
}

void Client::setEnabledButtons(const bool flag)
{
    for(auto& button: _buttons)
    {
        button->setEnabled(flag);
    }
}


void Client::setDropListValue(const std::pair<Column, QStringList>& dlValue)
{
    _dropListValues.insert(dlValue);
}

void Client::searchSwitch(const bool flag)
{
    _searchFrame->setVisible(flag);
    if (!flag) {offHighLight();}
}

void Client::updateSearchCounter(const QString& searchNumber, const QString& searchCount)
{
    _searchNumberStr = searchNumber;
    _searchCountStr = searchCount;
    _searchInfoLabel->setText(QString("%1/%2").arg(_searchNumberStr, _searchCountStr));
}

void Client::highlightCard(const int cardNumber)
{
    offHighLight();
    qDebug() << "Card Number: " << cardNumber;
    if (cardNumber < 0) {return;}
    _activeCards.at(cardNumber)->setStyleSheet("");
    _activeCards.at(cardNumber)->setObjectName("card_highlighted");
    _activeCards.at(cardNumber)->style()->unpolish(_activeCards.at(cardNumber));
    _activeCards.at(cardNumber)->style()->polish(_activeCards.at(cardNumber));
}

void Client::offHighLight()
{
    for (auto& card: _activeCards)
    {
        card.second->setStyleSheet("");
        card.second->setObjectName("card");
        card.second->style()->unpolish(card.second);
        card.second->style()->polish(card.second);
    }
}