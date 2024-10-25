#include <client/search_widget.hpp>

SearchWidget::SearchWidget(QStringList titles, QWidget* parent)
    : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout;

    for (const auto& title: titles)
    {
        QLabel* label = new QLabel(title, this);
        QLineEdit* lineEdit = new QLineEdit(this);

        layout->addWidget(label);
        layout->addWidget(lineEdit);

        _titlesMap[label] = lineEdit;
    }

    setLayout(layout);
}

std::map<QString, QString> SearchWidget::getValues() const
{
    std::map<QString, QString> result;

    for (auto it{_titlesMap.begin()}; it != _titlesMap.end(); ++it)
    {
        result[it->first->text()] = it->second->text();
    }

    return result;
}
