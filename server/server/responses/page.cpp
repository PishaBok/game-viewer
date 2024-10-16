#include <server/responses/page.hpp>

PageResponse::PageResponse()
{}

QJsonDocument PageResponse::toJson() const
{
    QJsonArray jsonArr;
    for(const auto& [key, value]: _pages)
    {
        QJsonObject modelObj;
        modelObj.insert("page", key);
        modelObj.insert("model", value.toJson());
        jsonArr.append(modelObj);
    }

    return QJsonDocument(jsonArr);
}

void PageResponse::addPage(const int pageN, const clib::TableModel &model)
{
    _pages[pageN] = model;
}
