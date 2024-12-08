#pragma once

#include <libcommon/requests/base.hpp>

class UniqueValuesRequest: public Request
{
public:
    UniqueValuesRequest();
    UniqueValuesRequest(Column column);
    virtual ~UniqueValuesRequest() = default;

    QJsonObject serialize() const override;
    void deserialize(const QJsonObject& jsonObject) override;

protected:
    Column _column;

private:
    bool validate(const QJsonObject& jsonObj) override;
};


class UniqueValuesResponse: public Response
{
public:
    UniqueValuesResponse();
    UniqueValuesResponse(Column column, const QStringList& uniqueValues);
    virtual ~UniqueValuesResponse() = default;

    QJsonObject serialize() const override;
    void deserialize(const QJsonObject& jsonObject) override;
private:
    Column _column;
    QStringList _uniqueValues;
};




