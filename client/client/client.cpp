#include <client/client.hpp>

Client::Client(QWidget *parent)
    : QMainWindow(parent), _filterColumns{"название", "год", "жанр"}
{
    _searchWidget = createSearchWidget();
    _filterWidget = createSearchWidget();
    _pageLabel = createPageLabel();
    _pageWidget = createPageWidget();
    _menuBar = createMenuBar();
    _toolBar = createToolBar();

    setMenuBar(_menuBar);
    addToolBar(_toolBar);
    setCentralWidget(_pageWidget);
    resize(750, 660);
}

Client::~Client()
{
    qDebug() << "Front destructor. Sended from thread: " << QThread::currentThread();
}


QLineEdit *Client::createPageLabel() const
{
    QLineEdit* pageLabel = new QLineEdit(QString("%1/%2").arg("1", "ND"));
    pageLabel->setMaximumWidth(65);
    pageLabel->setReadOnly(true);
    pageLabel->setAlignment(Qt::AlignCenter);
    pageLabel->setObjectName("toolBarLineEdit");

    return pageLabel;
}

SearchWidget *Client::createSearchWidget() const
{
    return new SearchWidget(_filterColumns);
}

QWidget *Client::createPageWidget() const
{
    QWidget* pageWidget = new QWidget;
    pageWidget->setObjectName("page");

    QGridLayout* layout = new QGridLayout;
    pageWidget->setLayout(layout);

    return pageWidget;
}

QMenuBar *Client::createMenuBar() const
{
    QMenuBar* menuBar = new QMenuBar;

    return menuBar;
}

QToolBar *Client::createToolBar()
{
    QToolBar* toolBar = new QToolBar;

    QPushButton* stepBack = new QPushButton(QString::fromUtf8("\u2190"));
    _buttonsMap[stepBack] = Button::stepBack;
    connect(stepBack, &QPushButton::clicked, this, &Client::buttonPressed);

    QPushButton* stepForward = new QPushButton(QString::fromUtf8("\u2192"));
    _buttonsMap[stepForward] = Button::stepForward;
    connect(stepForward, &QPushButton::clicked, this, &Client::buttonPressed);

    QPushButton* search = new QPushButton(QString(QChar(0xD83D)) + QChar(0xDD0D));
    _buttonsMap[search] = Button::search;
    connect(search, &QPushButton::clicked, this, &Client::buttonPressed);


    toolBar->addWidget(stepBack);
    toolBar->addWidget(stepForward);
    toolBar->addWidget(_pageLabel);
    toolBar->addWidget(_searchWidget);
    toolBar->addWidget(search);

    return toolBar;
}

QWidget *Client::createGameCard(const CardData& cardData) const
{
    QWidget* card = new QWidget;
    card->setObjectName("card");
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout* layout = new QVBoxLayout(card);

    QLabel* titleLabel = new QLabel(QString("%1 (%2)").arg(cardData.name, cardData.platform));
    titleLabel->setStyleSheet("font-weight: bold; font-size: 16px;");

    QLabel* yearLabel = new QLabel(QString("%1 г.").arg(cardData.year));
    QLabel* genreLabel = new QLabel(cardData.genre);

    QLabel* scoreLabel = new QLabel(QString("Critics Score: %1").arg(cardData.criticScore));
    QLabel* ratingLabel = new QLabel(QString("Rating: %1").arg(cardData.rating));

    layout->addWidget(titleLabel);
    layout->addWidget(yearLabel);
    layout->addWidget(genreLabel);
    layout->addWidget(scoreLabel);
    layout->addWidget(ratingLabel);

    layout->setStretch(0, 2);

    card->setLayout(layout);

    return card;
}

QDialog *Client::filterDialog() const
{

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

void Client::buttonPressed()
{
    Button button = _buttonsMap.at(sender());

    emit processButton(button);
}


void Client::updatePage(const clib::TableModel &model)
{
    clearLayout(_pageWidget->layout());
    QGridLayout* layout = qobject_cast<QGridLayout*>(_pageWidget->layout());

    for (int i = 0; i < model.rowCount(); ++i)
    {
        QWidget* gameCard = createGameCard({model.data(i, "name").toString(), model.data(i, "platform").toString(), model.data(i, "year").toInt(),
                                            model.data(i, "genre").toString(), model.data(i, "criticscore").toInt(), model.data(i, "rating").toString()});
        layout->addWidget(gameCard, i / 3, i % 3);
    }
}
