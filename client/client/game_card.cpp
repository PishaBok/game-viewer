#include <client/game_card.hpp>

GameCard::GameCard(const CardData& data, QFrame* parent)
    : QFrame(parent)
{
    setObjectName("card");
    QVBoxLayout* layout = new QVBoxLayout;

    QString titleStr = data.name;
    if (titleStr.length() > 20)
    {
        titleStr = titleStr.left(20 - 3) + "...";
    }

    QLabel* titleLabel = new QLabel(QString("%1 (%2)").arg(titleStr, data.platform));
    titleLabel->setStyleSheet("font-weight: bold; font-size: 16px;");

    QLabel* yearLabel = new QLabel(QString("%1 г.").arg(data.year));
    QLabel* genreLabel = new QLabel(data.genre);

    QLabel* scoreLabel = new QLabel(QString("Critics Score: %1").arg(data.criticScore));
    QLabel* ratingLabel = new QLabel(QString("Rating: %1").arg(data.rating));

    layout->addWidget(titleLabel);
    layout->addWidget(yearLabel);
    layout->addWidget(genreLabel);
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