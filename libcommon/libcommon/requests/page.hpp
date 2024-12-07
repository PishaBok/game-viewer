#pragma once

#include <libcommon/requests/base.hpp>
#include <libcommon/table_model.hpp>

class PageRequest: public Request
{
public:
    PageRequest();
    PageRequest(const int pageN, const std::map<Column, FilterParams>& filter);
    virtual ~PageRequest() = default;

    QJsonObject serialize() const override;
    void deserialize(const QJsonObject& jsonObject) override;

protected:
    int _page;
    std::map<Column, FilterParams> _filter;

private:
    bool validate(const QJsonObject& jsonObj) override;
};


class PageResponse: public Response
{
public:
    PageResponse();
    PageResponse(const std::map<int, clib::TableModel>& pages);
    virtual ~PageResponse() = default;

    QJsonObject serialize() const override;
    void deserialize(const QJsonObject& jsonObject) override;
private:
    std::map<int, clib::TableModel> _pages;
};