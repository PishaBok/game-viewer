#pragma once

#include <set>

#include <libcommon/requests/base.hpp>

class SearchRequest: public Request
{
public:
    SearchRequest();
    SearchRequest(const std::map<Column, FilterParams>& filter, const std::map<Column, QString>& search);
    virtual ~SearchRequest() = default;

    QJsonObject serialize() const override;
    void deserialize(const QJsonObject& jsonObject) override;

protected:
    std::map<Column, FilterParams> _filter;
    std::map<Column, QString> _search;
private:
    bool validate(const QJsonObject& jsonObj) override;
    QJsonArray mapToJson(const std::map<Column, QString>& paramMap) const;
    std::map<Column, QString> jsonToMap(const QJsonArray& jsonArr) const;
};



class SearchResponse: public Response
{
public:
    SearchResponse();
    SearchResponse(const std::set<int>& recordIds);
    virtual ~SearchResponse() = default;

    QJsonObject serialize() const override;
    void deserialize(const QJsonObject& jsonObject) override;
private:
    std::set<int> _recordIds;
};