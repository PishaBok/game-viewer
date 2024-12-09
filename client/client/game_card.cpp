#include <client/game_card.hpp>

GameCard::GameCard(const CardData& data, QFrame* parent)
    : QFrame(parent), _data{data}
{
    setObjectName("card");
    QVBoxLayout* layout = new QVBoxLayout;

    QString titleStr = data.name;
    if (titleStr.length() > 20)
    {
        titleStr = titleStr.left(20 - 3) + "...";
    }

    QLabel* titleLabel = new QLabel(QString("%1 (%2)").arg(titleStr, data.platform));
    titleLabel->setObjectName("cardTitle");

    QLabel* yearLabel = new QLabel(QString("%1 г.").arg(data.year));
    yearLabel->setObjectName("cardLabel");
    QLabel* genreLabel = new QLabel(data.genre);
    genreLabel->setObjectName("cardLabel");
    QLabel* publisherLabel = new QLabel(data.publisher);
    publisherLabel->setObjectName("cardLabel");

    QLabel* scoreLabel = new QLabel(QString("Critics Score: %1").arg(data.criticScore));
    scoreLabel->setObjectName("cardLabel");
    QLabel* ratingLabel = new QLabel(QString("Rating: %1").arg(data.rating));
    ratingLabel->setObjectName("cardLabel");

    //setBackgroundIcon(data.icon);
    
    layout->addWidget(titleLabel);
    layout->addWidget(yearLabel);
    layout->addWidget(genreLabel);
    layout->addWidget(publisherLabel);
    layout->addWidget(scoreLabel);
    layout->addWidget(ratingLabel);
    layout->setStretch(0, 2);

    setLayout(layout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

GameCard::~GameCard()
{}

void GameCard::resizeEvent(QResizeEvent *event)

{
    // const double aspectRatio = 9.0 / 16.0;

    // int newWidth = event->size().width();
    // int newHeight = static_cast<int>(newWidth / aspectRatio);

    // if (newHeight > event->size().height())
    // {
    //     newHeight = event->size().height();
    //     newWidth = static_cast<int>(newHeight * aspectRatio);
    // }

    // resize(newWidth, newHeight);

    QWidget::resizeEvent(event);
}

void GameCard::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QPixmap pixmap = QPixmap::fromImage(_data.icon);

    painter.drawPixmap(this->rect(), pixmap);

    QWidget::paintEvent(event);
}
