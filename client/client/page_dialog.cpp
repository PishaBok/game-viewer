#include <client/page_dialog.hpp>

PageDialog::PageDialog(QDialog* parent)
    : QDialog(parent)
{
    QGridLayout* layout = new QGridLayout;

    QLabel* label = new QLabel("Перейти к странице:");
    _pageEdit = new QLineEdit;

    QPushButton* okButton = new QPushButton("Применить");
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    QPushButton* cancelButton = new QPushButton("Отмена");
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
 
    layout->addWidget(label, 0, 0);
    layout->addWidget(_pageEdit, 0, 1);
    layout->addWidget(okButton, 1, 1);
    layout->addWidget(cancelButton, 1, 0);

    setLayout(layout);
}

int PageDialog::data() const
{
    return _pageEdit->text().toInt();
}