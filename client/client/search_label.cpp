#include <client/search_label.hpp>

SearchLabel::SearchLabel(const std::vector<Column> columns, QWidget* parent)
    : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout;

    for (const auto& column: columns)
    {
        QLineEdit* lineEdit = new QLineEdit(this);
        lineEdit->setObjectName("toolBarLineEdit");
        lineEdit->setFixedHeight(25);
        lineEdit->setPlaceholderText(QString::fromStdString(columnToTitleMap.at(column)));
        connect(lineEdit, &QLineEdit::textChanged, [this, lineEdit, column]()
        {
            _values[column] = lineEdit->text();
        });

        _values[column] = QString();

        layout->addWidget(lineEdit);
    }

    setLayout(layout);
}

std::map<Column, QString> SearchLabel::data() const
{
    return _values;
}
