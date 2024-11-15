#pragma once

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QResizeEvent>

struct CardData
{
    QString name;
    QString platform;
    int year;
    QString genre;
    int criticScore;
    QString rating;
};

class GameCard: public QFrame
{
    Q_OBJECT
public:
    GameCard(const CardData& data, QFrame* parent = nullptr);
    virtual ~GameCard();

protected:
    void resizeEvent(QResizeEvent* event) override;
};