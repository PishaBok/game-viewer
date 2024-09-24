#include "response.hpp"

Response::Response(QObject *parent) 
    : QObject(parent)
{

}

Response::Response(const Response &other) 
    : QObject(other.parent()),
    _pageNumber(other._pageNumber), _model(other._model)
{}

Response::Response(Response &&other) noexcept
    : QObject(other.parent()),
    _pageNumber(other._pageNumber), _model(std::move(other._model))
{
    other.setParent(nullptr);
    other._pageNumber = 0;
}

QJsonDocument Response::toJson() const
{
    QJsonObject jsonObj;

    jsonObj["page"] = _pageNumber;
    jsonObj["model"] = modelToJson(_model);

    return QJsonDocument(jsonObj);
}

QJsonObject Response::modelToJson(const TableModel &model) const
{
    QJsonObject jsonObj;

    for (int col{0}; col < model.columnCount(); ++col)
    {
        QJsonArray jsonArr;
        QString columnName = model.headerData(col, Qt::Horizontal).toString();
        for (int row{0}, rowCount{model.rowCount()}; row < rowCount; ++row)
        {
            QVariant data = model.data(model.index(row, col));
            jsonArr.append(QJsonValue::fromVariant(data));
        }
        jsonObj.insert(columnName, jsonArr);
    }

    return jsonObj;
}

void Response::setPageNumber(int page)
{
    _pageNumber = page;
}

void Response::setModel(const TableModel& model)
{
    _model = model;
}
