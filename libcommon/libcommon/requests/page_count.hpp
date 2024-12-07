#pragma once

#include <libcommon/requests/base.hpp>

class PageCountRequest: public Request
{
public:
    PageCountRequest();
    PageCountRequest(const int recordsOnPage, const std::map<Column, FilterParams>& filter);
    virtual ~PageCountRequest() = default;

    QJsonObject serialize() const override;
    void deserialize(const QJsonObject& jsonObject) override;
protected:
    int _recordsOnPage;
    std::map<Column, FilterParams> _filter;

private:
    bool validate(const QJsonObject& jsonObj) override;
};

class PageCountResponse: public Response
{
public:
    PageCountResponse();
    PageCountResponse(const int pageCount);
    virtual ~PageCountResponse() = default;

    QJsonObject serialize() const override;
    void deserialize(const QJsonObject& jsonObject) override;
private:
    int _pageCount;
};