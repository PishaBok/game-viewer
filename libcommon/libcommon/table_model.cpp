#include <libcommon/table_model.hpp>
#include "table_model.hpp"

namespace clib
{
    TableModel::TableModel(QObject *parent)
        : QAbstractTableModel{parent}, _nRows{0}, _nColumns{0}
    {}

    TableModel::TableModel(const QSqlQueryModel &other, QObject* parent)
        : QAbstractTableModel{parent}, _nRows{other.rowCount()}, _nColumns{other.columnCount()}
    {
        for (int col{0}, colCount{other.columnCount()}; col < colCount; ++col)
        {
            setHeaderData(col, Qt::Horizontal, other.headerData(col, Qt::Horizontal));
        }

        for (int row{0}, rowCount{other.rowCount()}; row < rowCount; ++row)
        {
            for (int col{0}, colCount{other.columnCount()}; col < colCount; ++col)
            {
                setData(this->index(row, col), other.data(other.index(row, col)));
            }
        }
    }

    TableModel::TableModel(QSqlQuery &&query, QObject* parent)
        : QAbstractTableModel{parent}, _nRows{}, _nColumns{}
    {
        QSqlQueryModel queryModel;
        queryModel.setQuery(std::move(query));

        _nRows = queryModel.rowCount();
        _nColumns = queryModel.columnCount();

        for (int col{0}, colCount{queryModel.columnCount()}; col < colCount; ++col)
        {
            setHeaderData(col, Qt::Horizontal, queryModel.headerData(col, Qt::Horizontal));
        }

        for (int row{0}, rowCount{queryModel.rowCount()}; row < rowCount; ++row)
        {
            for (int col{0}, colCount{queryModel.columnCount()}; col < colCount; ++col)
            {
                setData(this->index(row, col), queryModel.data(queryModel.index(row, col)));
            }
        }
    }

    TableModel::TableModel(const TableModel &other)
        : QAbstractTableModel{other.parent()},
        _nRows{other.rowCount()}, _nColumns{other.columnCount()}, _horizontalHeaders{other._horizontalHeaders}
    {
        emit headerDataChanged(Qt::Orientation::Horizontal, 0, _nColumns - 1);

        for (int row{0}, rowCount{other.rowCount()}; row < rowCount; ++row)
        {
            for (int col{0}, colCount{other.columnCount()}; col < colCount; ++col)
            {
                setData(this->index(row, col), other._data[other.index(row, col)]);
            }
        }
    }

    TableModel::TableModel(TableModel &&other) noexcept
        : QAbstractTableModel{other.parent()},
        _nRows{other.rowCount()}, _nColumns{other.columnCount()}, _horizontalHeaders{std::move(other._horizontalHeaders)}
    {
        emit headerDataChanged(Qt::Orientation::Horizontal, 0, _nColumns - 1);

        for (int row{0}, rowCount{other.rowCount()}; row < rowCount; ++row)
        {
            for (int col{0}, colCount{other.columnCount()}; col < colCount; ++col)
            {
                _data[this->index(row, col)] = std::move(other._data[other.index(row, col)]);
            }
        }
        
        emit dataChanged(this->index(0, 0), this->index(_nRows - 1, _nColumns - 1));

        other._nRows = 0;
        other._nColumns = 0;
    }

    TableModel& TableModel::operator=(const TableModel& other)
    {
        _nRows = other.rowCount();
        _nColumns = other.columnCount();

        _horizontalHeaders.clear();
        _data.clear();

        for (int col{0}, colCount{other.columnCount()}; col < colCount; ++col)
        {
            setHeaderData(col, Qt::Horizontal, other.headerData(col, Qt::Horizontal));
        }

        for (int row{0}, rowCount{other.rowCount()}; row < rowCount; ++row)
        {
            for (int col{0}, colCount{other.columnCount()}; col < colCount; ++col)
            {
                setData(this->index(row, col), other._data[other.index(row, col)]);
            }
        }

        return *this;
    }

    QVariant TableModel::data(const QModelIndex &index, int nRole) const
    {
        if (!index.isValid())
        {
            return QVariant();
        }

        return (nRole == Qt::DisplayRole || nRole == Qt::EditRole)
                    ? _data[index] : QVariant();
    }

    QVariant TableModel::data(const int row, const QString &columnName) const
    {
        int colIndex = -1;
        for (auto it = _horizontalHeaders.begin(); it != _horizontalHeaders.end(); ++it)
        {
            if (it.value().toString() == columnName)
            {
                colIndex = it.key();
                break;
            }
        }

        if (colIndex == -1) {return QVariant();}

        QModelIndex index = this->index(row, colIndex);
        
        return data(index);
    }

    bool TableModel::setData(const QModelIndex &index, const QVariant &value, int nRole)
    {
        if (index.isValid() && nRole == Qt::EditRole)
        {
            _data[index] = value;
            emit dataChanged(index, index);
            return true;
        }

        return false;
    }

    QVariant TableModel::headerData(int nSection, Qt::Orientation orientation, int nRole) const
    {
        if (nRole != Qt::DisplayRole)
        {
            return QVariant();
        }

        return (orientation == Qt::Orientation::Horizontal) ? _horizontalHeaders[nSection]
                                                            : QString::number(nSection + 1);
    }

    bool TableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int nRole)
    {
        if (_nColumns < section) {_nColumns = section;}

        if (orientation == Qt::Horizontal)
        {
            _horizontalHeaders[section] = value;
            emit headerDataChanged(Qt::Orientation::Horizontal, 0, _nColumns - 1);
            return true;
        }

        return false;
    }

    int TableModel::rowCount(const QModelIndex &parent) const
    {
        return _nRows;
    }

    int TableModel::columnCount(const QModelIndex &parent) const
    {
        return _nColumns;
    }

    Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
    {
        if (!index.isValid())
        {
            return Qt::ItemIsEnabled;
        }

        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }

    void TableModel::setRowCount(const int nRows)
    {
        _nRows = nRows;
    }

    void TableModel::setColCount(const int nCols)
    {
        _nColumns = nCols;
    }

    void TableModel::loadJson(const QJsonObject &jsonObj)
    {
        beginResetModel();

        _data.clear();
        _horizontalHeaders.clear();
        _nRows = 0;
        _nColumns = 0;

        QJsonArray headers{jsonObj["headers"].toArray()};
        for(int counter{0}; const QJsonValue& header: headers)
        {
            setHeaderData(counter, Qt::Horizontal, header.toString());
            ++counter;
        }

        setRowCount(jsonObj["data"].toArray().size());
        setColCount(_horizontalHeaders.size());

        for (int row{0}; const QJsonValue& rowValue: jsonObj["data"].toArray())
        {
            QJsonArray rowArray{rowValue.toArray()};
            for(int col{0}; const QJsonValue& cellData: rowArray)
            {
                setData(this->index(row, col), cellData.toVariant());
                ++col;
            }
            ++row;
        }

        endResetModel();
    }

    QJsonObject TableModel::toJson() const
    {
        QJsonObject jsonObj;

        QJsonArray headerArray;
        QJsonArray dataArray;

        // Сохраняем заголовки
        for (int col{0}; col < columnCount(); ++col)
        {
            QVariant columnName = headerData(col, Qt::Horizontal);
            headerArray.append(QJsonValue::fromVariant(columnName));
        }

        // Сохраняем данные
        for (int row{0}; row < _nRows; ++row)
        {
            QJsonArray rowArray;
            for (int col{0}; col < columnCount(); ++col)
            {
                QVariant data = this->data(this->index(row, col));
                rowArray.append(QJsonValue::fromVariant(data));
            }
            dataArray.append(rowArray); // Добавляем объект строки в массив
        }

        jsonObj.insert("headers", headerArray);
        jsonObj.insert("data", dataArray);

        return jsonObj;

        return QJsonObject();
    }
}