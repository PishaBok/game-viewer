#pragma once

#include <map>
#include <server/responses/base.hpp>
#include <libcommon/table_model.hpp>

class PageResponse: public Response
{
public:
    PageResponse();
    virtual ~PageResponse() = default;
    QJsonDocument toJson() const override;
    void addPage(const int pageN, const clib::TableModel& model);


private:
    std::map<int, clib::TableModel> _pages;
};
