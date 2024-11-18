#pragma once

#include <random>
#include <string>
#include <map>
#include <QString>
#include <QVariant>
#include <QJsonObject>
#include <QSqlRecord>

#include <libcommon/columns.hpp>


namespace clib
{
    QString generateRandomString();
    bool compareRecordWithSearch(const QSqlRecord& record, const std::map<Column, QString>& search);
    bool compareRecordWithSearch(const std::map<Column, QString>& record, const std::map<Column, QString>& search);
}

