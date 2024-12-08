#pragma once

#include <map>
#include <string>
#include <string_view>
#include <functional>
#include <charconv>

// Рабочие столбцы
enum class Column
{
    userName,
    password,

    gameId, 
    gameName,
    platform,
    year,
    genre,
    publisher,
    criticscore,
    rating,
    gameimg
};

// Метода фильтра/поиска
enum class CompareType
{
    startsWith,
    endsWith,
    includes,
    equals,
    range
};

// Параметр фильтра для каждого столбца
struct FilterParams
{
    std::string value;
    CompareType type;

    bool operator==(const FilterParams &other) const {
        return value == other.value && type == other.type;
    }
};

// Для работы с базой данных
extern const std::map<Column, std::string> columnToStringMap;
extern const std::map<std::string, Column> columnFromStringMap;

// Для UI столбцы
extern const std::map<Column, std::string> columnToTitleMap;
extern const std::map<std::string, Column> columnFromTitleMap;

// Для UI сравнение
extern const std::map<CompareType, std::string> compareTypeToTitle;
extern const std::map<std::string, CompareType> compareTypeFromTitle;

// Для поиска
extern const std::map<Column, std::function<bool(std::string_view, std::string_view)>> defautCompareMap;

extern bool compareStartsWith(std::string_view value, std::string_view search);
extern bool compareEndsWith(std::string_view value, std::string_view search);
extern bool compareIncludes(std::string_view value, std::string_view search);
extern bool compareEquals(std::string_view value, std::string_view search);
extern bool compareData(std::string_view value, std::string_view search);

extern std::vector<std::string_view> splitData(std::string_view dataSearch, char delimeter);





