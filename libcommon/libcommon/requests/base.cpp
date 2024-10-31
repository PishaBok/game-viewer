#include <libcommon/requests/base.hpp>
#include "base.hpp"

Request::Request(const RequestType reqType)
    : _typeId{reqType}
{
}

std::unique_ptr<Response> Request::execute()
{
    return std::unique_ptr<Response>();
}

QJsonObject Response::data() const
{
    return serialize().value("data").toObject();
}


Response::Response(const RequestType reqType)
    : _typeId(reqType)
{}
