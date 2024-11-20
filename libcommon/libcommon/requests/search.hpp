#pragma once

#include <set>

#include <libcommon/requests/base.hpp>

class SearchRequest: public Request
{
public:
    SearchRequest();
    SearchRequest(const int recordsOnPage, const std::map<Column, QString>& filter, const std::map<Column, QString>& search, const bool isCorrectOrder);
    virtual ~SearchRequest() = default;

    QJsonObject serialize() const override;
    void deserialize(const QJsonObject& jsonObject) override;

protected:
    int _recordsOnPage;
    std::map<Column, QString> _filter;
    std::map<Column, QString> _search;
    bool _isCorrectOrder;

private:
    bool validate(const QJsonObject& jsonObj) override;
    QJsonArray mapToJson(const std::map<Column, QString>& paramMap) const;
    std::map<Column, QString> jsonToMap(const QJsonArray& jsonArr) const;
};



class SearchResponse: public Response
{
public:
    SearchResponse();
    SearchResponse(const int recordCount, const std::set<int>& pageNumbers);
    SearchResponse(const std::pair<int, std::set<int>>& searcResult);
    virtual ~SearchResponse() = default;

    QJsonObject serialize() const override;
    void deserialize(const QJsonObject& jsonObject) override;

private:
    int _recordCount;
    std::set<int> _pageNumbers;
};