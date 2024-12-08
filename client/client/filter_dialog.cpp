#include <client/filter_dialog.hpp>

FilterDialog::FilterDialog(const std::vector<Column> columns, const std::map<Column, QStringList>& uniqueValuesByColumn, QDialog* parent)
    : QDialog(parent), _columns{columns}, _uniqueValuesByColumn{uniqueValuesByColumn}
{
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(10, 10, 10, 10);

    int i{0};
    for (; i < columns.size(); ++i)
    {
        QFrame* filterWidget = columnToWidget(columns[i]);
        filterWidget->setObjectName("filterDialogWidget");
        _values[columns[i]] = std::string();

        layout->addWidget(filterWidget);

        QFrame* line = new QFrame;
        line->setObjectName("delimeterLine");
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        layout->addWidget(line);
    }

    layout->addWidget(createButtonsWidget());

    setLayout(layout);
    setWindowTitle("Фильтр");
}

QFrame* FilterDialog::createButtonsWidget()
{
    QFrame* buttonsWidget = new QFrame;

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setSpacing(5);
    layout->setContentsMargins(5, 5, 5, 5);

    QPushButton* okButton = new QPushButton("Применить");
    okButton->setDefault(true);
    okButton->setFixedWidth(100);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    QPushButton* cancelButton = new QPushButton("Отмена");
    cancelButton->setFixedWidth(100);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    layout->addWidget(okButton);
    layout->addWidget(cancelButton);

    buttonsWidget->setLayout(layout);

    return buttonsWidget;
}

QFrame* FilterDialog::columnToWidget(Column column)
{
    switch(column)
    {
        case Column::year: case Column::criticscore:
            return rangeFilter(column);
        case Column::genre: case Column::rating:
            return dropListFilter(column);
        default:
            return defaultFilter(column);
    }
}

QFrame* FilterDialog::defaultFilter(Column column)
{
    QFrame* filterWidget = new QFrame;

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setSpacing(5);
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
    settingButton->setFixedWidth(40);
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

    layout->addWidget(title, 0, Qt::AlignLeft);
    layout->addWidget(lineEdit);
    layout->addWidget(settingButton);
    filterWidget->setLayout(layout);

    return filterWidget;
}

QFrame* FilterDialog::rangeFilter(Column column)
{
    QFrame* filterWidget = new QFrame;
    const char delimeter{':'};

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setSpacing(10);
    layout->setContentsMargins(5, 5, 5, 5);

    QLabel* title = new QLabel(QString::fromStdString(columnToTitleMap.at(column)));
    QLabel* fromLabel = new QLabel("с");
    QLabel* untilLabel = new QLabel("по");
    QLineEdit* fromEdit = new QLineEdit;
    fromEdit->setObjectName("toolBarLineEdit");
    fromEdit->setFixedWidth(60);
    connect(fromEdit, &QLineEdit::editingFinished, this, [this, fromEdit, column, delimeter]()
    {
        auto [leftBound, rightBound] = parseYear(QString::fromStdString(_values.at(column)), delimeter);
        leftBound = fromEdit->text().toStdString();
        _values[column] = leftBound + delimeter + rightBound;
    });
    QLineEdit* untilEdit = new QLineEdit;
    untilEdit->setObjectName("toolBarLineEdit");
    untilEdit->setFixedWidth(60);
    connect(untilEdit, &QLineEdit::editingFinished, this, [this, untilEdit, column, delimeter]()
    {
        auto [leftBound, rightBound] = parseYear(QString::fromStdString(_values.at(column)), delimeter);
        rightBound = untilEdit->text().toStdString();
        _values[column] = leftBound + delimeter + rightBound;
    });
    
    layout->addWidget(title, 3, Qt::AlignLeft);
    layout->addWidget(fromLabel);
    layout->addWidget(fromEdit);
    layout->addWidget(untilLabel);
    layout->addWidget(untilEdit);
    
    filterWidget->setLayout(layout);

    _types[column] = CompareType::range;

    return filterWidget;
}

QFrame* FilterDialog::dropListFilter(Column column)
{
    QFrame* filterWidget = new QFrame;

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setSpacing(10);
    layout->setContentsMargins(5, 5, 5, 5);

    QLabel* title = new QLabel(QString::fromStdString(columnToTitleMap.at(column)));
    
    QComboBox* comboBox = new QComboBox;
    comboBox->addItem("");
    if (!_uniqueValuesByColumn.empty())
    {
        comboBox->addItems(_uniqueValuesByColumn.at(column));
    }
    connect(comboBox, &QComboBox::currentTextChanged, [this, comboBox, column]()
    {   
        _values[column] = comboBox->currentText().toStdString();
    });
    
    layout->addWidget(title, 0, Qt::AlignLeft);
    layout->addWidget(comboBox);
    filterWidget->setLayout(layout);

    _types[column] = CompareType::equals;

    return filterWidget;
}


QFrame* FilterDialog::createPopupWidget(Column column)
{
    QFrame* popupWidget = new QFrame(this);
    popupWidget->setWindowFlags(Qt::Popup);
    QVBoxLayout* layout = new QVBoxLayout;

    // Создание кнопок
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


std::pair<std::string, std::string> FilterDialog::parseYear(const QString& yearStr, const char delimeter) const
{
    QStringList years = yearStr.split(delimeter);

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

