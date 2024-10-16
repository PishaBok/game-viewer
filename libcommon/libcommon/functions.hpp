#pragma once

#include <random>
#include <string>
#include <QString>
#include <QVariant>
#include <QJsonObject>


namespace clib
{
    QString generateRandomString();
    QVariant parseTypedValue(const QJsonObject& typedValueObj);
}

