#pragma once

#include <QWidget>
#include <QStringList>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>


class SearchWidget: public QWidget
{
    Q_OBJECT
public:
    SearchWidget(QStringList titles, QWidget* parent = nullptr);

    std::map<QString, QString> getValues() const;
private:
    std::map<QLabel*, QLineEdit*> _titlesMap;
};
