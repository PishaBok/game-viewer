#pragma once

#include <server/requests/base.hpp>
#include <server/responses/page.hpp>

class PageRequest: public Request
{
public:
    PageRequest(const QJsonObject& data, DatabaseManager& dbManager);
    virtual ~PageRequest() = default;

    std::unique_ptr<Response> process() override;

private:
    int _page;
    std::map<QString, QVariant> _filter;



    void parseJson(const QJsonObject& jsonObj) override;
    bool validateJson(const QJsonObject& jsonObj) override;

    std::vector<std::future<std::pair<int, clib::TableModel>>> startPageThreads();
    std::pair<int, clib::TableModel> createPage(const int number);

};
