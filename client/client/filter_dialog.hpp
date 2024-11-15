#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStringList>
#include <QPushButton>

class FilterDialog: public QDialog
{
    Q_OBJECT
public:
    FilterDialog(const QStringList& titles, QDialog* parent = nullptr);

    std::map<QString, QString> data() const;
private:
    std::map<QLabel*, QLineEdit*> _titlesMap;
};