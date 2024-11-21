#pragma once

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QDialog>

class PageDialog: public QDialog
{
    Q_OBJECT
public:
    PageDialog(QDialog* parent = nullptr);

    int data() const;
private:
    QLineEdit* _pageEdit;
};