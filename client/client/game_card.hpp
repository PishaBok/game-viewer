#pragma once

#include <QFrame>
#include <QLabel>
#include <QBuffer>
#include <QImage>
#include <QPixmap>
#include <QPalette>
#include <QBrush>
#include <QPainter>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QPaintEvent>

struct CardData
{
    QString name;
    QString platform;
    int year;
    QString genre;
    QString publisher;
    int criticScore;
    QString rating;
    QImage icon;
};

class GameCard: public QFrame
{
    Q_OBJECT
public:
    GameCard(const CardData& data, QFrame* parent = nullptr);
    virtual ~GameCard();

private:
    CardData _data;
protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
};