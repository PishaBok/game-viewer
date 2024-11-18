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


    bool compareRecordWithSearch(const QSqlRecord& record, const std::map<Column, QString>& search)
    {
        for (const auto &[column, searchValue]: search)
        {
            QString columnName = QString::fromStdString(columnToStringMap.at(column));

            if (!record.contains(columnName)) {continue;}

            std::string recordValue = record.value(columnName).toString().toStdString();

            if (!comparisonMap.at(column).compareFunc(recordValue, searchValue.toStdString()))
            {
                return false;
            }
        }
        return true;
    }

    bool compareRecordWithSearch(const std::map<Column, QString>& record, const std::map<Column, QString>& search)
    {
        for (const auto &[column, searchValue]: search)
        {
            if (!record.contains(column)) {continue;}

            std::string recordValue = record.at(column).toStdString();

            if (!comparisonMap.at(column).compareFunc(recordValue, searchValue.toStdString()))
            {
                return false;
            }
        }
        return true;
    }
}

