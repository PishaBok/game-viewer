#include <client/filter_dialog.hpp>

FilterDialog::FilterDialog(const QStringList& titles, QDialog* parent)
    : QDialog(parent)
{
    QGridLayout* layout = new QGridLayout;

    int i{0};
    for (; i < titles.size(); ++i)
    {
        QLabel* title = new QLabel(titles[i]);
        QLineEdit* lineEdit = new QLineEdit;

        layout->addWidget(title, i, 0);
        layout->addWidget(lineEdit, i, 1);

        _titlesMap[title] = lineEdit;
    }

    QPushButton* okButton = new QPushButton("Применить");
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    QPushButton* cancelButton = new QPushButton("Отмена");
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
 
    layout->addWidget(okButton, i, 1);
    layout->addWidget(cancelButton, i, 0);

    setLayout(layout);
    setWindowTitle("Фильтр");
}

std::map<QString, QString> FilterDialog::data() const
{
    std::map<QString, QString> result;

    for (const auto& el: _titlesMap)
    {
        result[el.first->text()] = el.second->text();
    }

    return result;
}