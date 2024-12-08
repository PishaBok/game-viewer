#pragma once

#include <vector>
#include <map>
#include <set>
#include <thread>
#include <future>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <libcommon/columns.hpp>

enum class RequestType
{
    page,
    search,
    pageCount,
    uniqueValues
};

class Response;   

class Request
{
public:
    Request(const RequestType reqType);
    virtual ~Request() = default;

    virtual std::unique_ptr<Response> execute();
    virtual QJsonObject serialize() const = 0;
    virtual void deserialize(const QJsonObject& jsonObject) = 0;
protected:
    const RequestType _typeId;

    virtual bool validate(const QJsonObject& jsonObj) = 0;
};


class Response
{
public:
    Response(const RequestType reqType);
    virtual ~Response() = default;

    virtual QJsonObject data() const;
    virtual QJsonObject serialize() const = 0;
    virtual void deserialize(const QJsonObject& jsonObject) = 0;

protected:
    const RequestType _typeId;
};



