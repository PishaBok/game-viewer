#include <client/filter_dialog.hpp>

FilterDialog::FilterDialog(const std::vector<Column> columns, QDialog* parent)
    : QDialog(parent), _columns{columns}
{
    QGridLayout* layout = new QGridLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(10, 10, 10, 10);

    int i{0};
    for (; i < columns.size(); ++i)
    {
        QFrame* filterWidget = columnToWidget(columns[i]);
        _values[columns[i]] = std::string();

        layout->addWidget(filterWidget, i, 0, 1, 2);
    }

    QPushButton* okButton = new QPushButton("Применить");
    okButton->setDefault(true);
    okButton->setFixedWidth(100);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    QPushButton* cancelButton = new QPushButton("Отмена");
    cancelButton->setFixedWidth(100);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
 
    layout->addWidget(okButton, i, 1, 1, 1, Qt::AlignCenter);
    layout->addWidget(cancelButton, i, 0, 1, 1, Qt::AlignCenter);

    setLayout(layout);
    setWindowTitle("Фильтр");
}

QFrame* FilterDialog::columnToWidget(Column column)
{
    switch(column)
    {
        case Column::year:
            return dataFilter(column);
        default:
            return defaultFilter(column);
    }
}

QFrame* FilterDialog::defaultFilter(Column column)
{
    QFrame* filterWidget = new QFrame;

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);

    QLabel* title = new QLabel(QString::fromStdString(columnToTitleMap.at(column)));
    QLineEdit* lineEdit = new QLineEdit;
    lineEdit->setObjectName("toolBarLineEdit");
    lineEdit->setFixedWidth(120);
    // При изменении текста обновляем значение
    connect(lineEdit, &QLineEdit::editingFinished, this, [this, lineEdit, column]()
    {
        _values[column] = lineEdit->text().toStdString();
    });

    QPushButton* settingButton = new QPushButton(QChar(0x2692));
    settingButton->setObjectName("toolButton");
    settingButton->setFixedWidth(50);
    QFrame* popupWidget = createPopupWidget(column);
    // При нажатии кнопки отрисовывает popupWidget
    connect(settingButton, &QPushButton::clicked, this, [this, settingButton, popupWidget]()
    {
        QPoint buttonPosition = settingButton->mapToGlobal(QPoint(0, 0));
        QSize buttonSize = settingButton->size();

        int popupX = buttonPosition.x() + buttonSize.width();
        int popupY = buttonPosition.y() - (popupWidget->height() / 2);
        popupWidget->move(popupX, popupY);

        QPropertyAnimation* animation = new QPropertyAnimation(popupWidget, "pos");

        QPoint windowPos = this->mapToGlobal(QPoint(0, 0));
        QSize windowSize = this->size();
        animation->setDuration(100);
        animation->setStartValue(popupWidget->pos());
        animation->setEndValue(QPoint(windowPos.x() + windowSize.width(), popupY));

        popupWidget->show();
        animation->start();
    });

    layout->addWidget(title);
    layout->addWidget(lineEdit);
    layout->addWidget(settingButton);
    filterWidget->setLayout(layout);

    return filterWidget;
}

QFrame* FilterDialog::dataFilter(Column column)
{
    QFrame* filterWidget = new QFrame;

    QHBoxLayout* layout = new QHBoxLayout;

    QLabel* fromLabel = new QLabel("с");
    QLabel* untilLabel = new QLabel("по");
    QLineEdit* fromEdit = new QLineEdit;
    connect(fromEdit, &QLineEdit::editingFinished, this, [this, fromEdit, column]()
    {
        auto [leftBound, rightBound] = parseYear(QString::fromStdString(_values.at(column)));
        leftBound = fromEdit->text().toStdString();
        _values[column] = leftBound + "|" + rightBound;
    });
    QLineEdit* untilEdit = new QLineEdit;
    connect(untilEdit, &QLineEdit::editingFinished, this, [this, untilEdit, column]()
    {
        auto [leftBound, rightBound] = parseYear(QString::fromStdString(_values.at(column)));
        rightBound = untilEdit->text().toStdString();
        _values[column] = leftBound + "|" + rightBound;
    });
    
    layout->addWidget(fromLabel);
    layout->addWidget(fromEdit);
    layout->addWidget(untilLabel);
    layout->addWidget(untilEdit);
    filterWidget->setLayout(layout);

    _types[column] = CompareType::data;

    return filterWidget;
}


QFrame* FilterDialog::createPopupWidget(Column column)
{
    QFrame* popupWidget = new QFrame(this);
    popupWidget->setWindowFlags(Qt::Popup);
    QVBoxLayout* layout = new QVBoxLayout;

    QRadioButton* firstButton = nullptr;
    for (CompareType type: {CompareType::startsWith, CompareType::endsWith, CompareType::includes, CompareType::equals})
    {
        QRadioButton* radioButton = new QRadioButton(QString::fromStdString(compareTypeToTitle.at(type)));
        connect(radioButton, &QRadioButton::clicked, this, [this, radioButton, column]()
        {
            _types[column] = compareTypeFromTitle.at(radioButton->text().toStdString());
        });

        layout->addWidget(radioButton);

        if (!firstButton) {firstButton = radioButton;}
    }
    // Инициализация значением по умолчанию
    if (firstButton) 
    {
        _types[column] = compareTypeFromTitle.at(firstButton->text().toStdString());
        firstButton->setChecked(true);
    }

    popupWidget->setLayout(layout);

    return popupWidget;
}


std::pair<std::string, std::string> FilterDialog::parseYear(const QString& yearStr) const
{
    QStringList years = yearStr.split("|");

    if (years.isEmpty()) {return {std::string(), std::string()};}

    return {years.front().toStdString(), years.back().toStdString()};
}


std::map<Column, FilterParams> FilterDialog::data() const
{
    std::map<Column, FilterParams> filterRes;

    for (auto column: _columns)
    {
        filterRes[column] = {_values.at(column), _types.at(column)};
    }

    return filterRes;
}

