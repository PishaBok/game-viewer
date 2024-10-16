#pragma once

#include <QJsonObject>
#include <QjsonArray>
#include <QJsonDocument>

class Response
{
public:
    Response() = default;
    virtual ~Response() = default;

    virtual QJsonDocument toJson() const = 0;

};