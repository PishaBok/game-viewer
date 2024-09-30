#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "table_model.hpp"

class Response: public QObject
{
    Q_OBJECT
public:
    Response(QObject* parent = nullptr);

    Response(const Response& other);

    Response(Response&& other) noexcept;

    QJsonDocument toJson() const;
    void setPageNumber(int page);
    void setModel(const TableModel& model);
private:
    int _pageNumber;
    TableModel _model;
};