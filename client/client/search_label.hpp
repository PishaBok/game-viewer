#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>

#include <libcommon/columns.hpp>

class SearchLabel: public QWidget
{
    Q_OBJECT
public:
    SearchLabel(const std::vector<Column> columns, QWidget* parent = nullptr);

    std::map<Column, QString> data() const;
private:
    std::map<Column, QString> _values;
};
