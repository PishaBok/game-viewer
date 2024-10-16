#include <libcommon/functions.hpp>
#include "functions.hpp"

namespace clib
{
    QString generateRandomString() 
    {

    size_t length{15};
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;  // Источник энтропии
    std::mt19937 generator(rd());  // Генератор случайных чисел
    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    QString randomString;
    for (size_t i = 0; i < length; ++i) {
        randomString += characters[distribution(generator)];
    }

    return randomString;
    }

    QVariant parseTypedValue(const QJsonObject& typedValueObj)
    {
        QString type = typedValueObj["type"].toString();

        if (type == "int")
        {
            return typedValueObj["value"].toInt();
        }
        else if (type == "string")
        {
            return typedValueObj["value"].toString();
        }

        return QVariant();
    }
}

