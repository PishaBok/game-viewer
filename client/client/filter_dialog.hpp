#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStringList>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QPropertyAnimation>

#include <libcommon/columns.hpp>

class FilterDialog: public QDialog
{
    Q_OBJECT
public:
    FilterDialog(const std::vector<Column> columns, QDialog* parent = nullptr);

    std::map<Column, FilterParams> data() const;
private:
    std::vector<Column> _columns;

    // Для получения результата фильтра
    std::map<Column, std::string> _values;
    std::map<Column, CompareType> _types;



    QFrame* columnToWidget(Column column);
    QFrame* defaultFilter(Column column); // Классический виджет фильтра
    QFrame* dataFilter(Column column); // Виджет для года с диапазоном
    QFrame* createPopupWidget(Column column);

    std::pair<std::string, std::string> parseYear(const QString& yearStr) const;
};