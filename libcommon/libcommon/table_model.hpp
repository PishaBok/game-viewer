#pragma once

#include <QAbstractTableModel>
#include <QSqlQueryModel>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QImage>
#include <QByteArray>
#include <QBuffer>

namespace clib
{
    class TableModel : public QAbstractTableModel
    {
        Q_OBJECT
    public:
        explicit TableModel(QObject* parent = nullptr);
        explicit TableModel(const QSqlQueryModel& other, QObject* parent = nullptr);
        explicit TableModel(QSqlQuery&& query, QObject* parent = nullptr);
        TableModel(const TableModel& other);
        TableModel(TableModel&& other) noexcept;
        TableModel& operator=(const TableModel& other);

        QVariant data(const QModelIndex& index, int nRole = Qt::DisplayRole) const override;
        QVariant data(const int row, const QString& columnName) const;
        bool setData(const QModelIndex& index, const QVariant& value, int nRole = Qt::EditRole) override;
        QVariant headerData(int nSection, Qt::Orientation orientation, int nRole = Qt::DisplayRole) const override;
        bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int nRole = Qt::EditRole) override;
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;

        QImage icon(const int row) const;
        void setIcon(const int row, const QImage& icon);

        void setRowCount(const int nRows);
        void setColCount(const int nCols); 
        void loadJson(const QJsonObject& jsonObj);
        QJsonObject toJson() const;



    private:
        int _nRows;
        int _nColumns;

        QHash<QModelIndex, QVariant> _data;
        QHash<int, QVariant> _horizontalHeaders;
        QHash<int, QString> _icons;
    };
}