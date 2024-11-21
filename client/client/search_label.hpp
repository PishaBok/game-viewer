#pragma once

#include <QWidget>
#include <QStringList>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>

class SearchLabel: public QWidget
{
    Q_OBJECT
public:
    SearchLabel(QStringList titles, QWidget* parent = nullptr);

    std::map<QString, QString> data() const;
private:
    std::map<QLabel*, QLineEdit*> _titlesMap;
};
