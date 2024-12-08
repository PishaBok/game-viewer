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
#include <QComboBox>

#include <libcommon/columns.hpp>

class FilterDialog: public QDialog
{
    Q_OBJECT
public:
    FilterDialog(const std::vector<Column> columns, const std::map<Column, QStringList>& uniqueValuesByColumn = {}, QDialog* parent = nullptr);

    std::map<Column, FilterParams> data() const;
private:
    std::vector<Column> _columns;
    std::map<Column, QStringList> _uniqueValuesByColumn;

    // Для получения результата фильтра
    std::map<Column, std::string> _values;
    std::map<Column, CompareType> _types;



    QFrame* columnToWidget(Column column);
    QFrame* defaultFilter(Column column); // Классический виджет фильтра
    QFrame* rangeFilter(Column column); // Виджет с диапазоном
    QFrame* dropListFilter(Column column); // Виджет с выпадющим списком

    QFrame* createButtonsWidget();
    QFrame* createPopupWidget(Column column);

    std::pair<std::string, std::string> parseYear(const QString& yearStr, const char delimeter) const;
};