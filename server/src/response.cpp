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
    jsonObj["model"] = _model.toJson();

    return QJsonDocument(jsonObj);
}

void Response::setPageNumber(int page)
{
    _pageNumber = page;
}

void Response::setModel(const TableModel& model)
{
    _model = model;
}
