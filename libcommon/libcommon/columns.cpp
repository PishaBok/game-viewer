#include <libcommon/columns.hpp>

const std::map<Column, std::string> columnToStringMap = 
{
    {Column::userName, "username"}, {Column::password, "password"},
    {Column::gameId, "gameid"}, {Column::gameName, "gamename"},
    {Column::platform, "platform"}, {Column::year, "year"},
    {Column::genre, "genre"}, {Column::publisher, "publisher"},
    {Column::criticscore, "criticscore"}, {Column::rating, "rating"}
};

const std::map<std::string, Column> columnFromStringMap = 
{
    {"username", Column::userName}, {"password", Column::password},
    {"gameid", Column::gameId}, {"gamename", Column::gameName},
    {"platform", Column::platform}, {"year", Column::year},
    {"genre", Column::genre}, {"publisher", Column::publisher},
    {"criticscore", Column::criticscore}, {"rating", Column::rating}
};



const std::map<Column, std::string> columnToTitleMap = 
{
    {Column::userName, "Логин"}, {Column::password, "Пароль"},
    {Column::gameId, ""}, {Column::gameName, "Название"},
    {Column::platform, "Платформа"}, {Column::year, "Год выхода"},
    {Column::genre, "Жанр"}, {Column::publisher, "Издатель"},
    {Column::criticscore, "Оценка критиков"}, {Column::rating, "Возрастной рейтинг"}
};

const std::map<std::string, Column> columnFromTitleMap = 
{
    {"Логин", Column::userName}, {"Пароль", Column::password},
    {"", Column::gameId}, {"Название", Column::gameName},
    {"Платформа", Column::platform}, {"Год выхода", Column::year},
    {"Жанр", Column::genre}, {"Издатель", Column::publisher},
    {"Оценка критиков", Column::criticscore}, {"Возрастной рейтинг", Column::rating}
};


const std::map<Column, ComparisonSettings> comparisonMap =
{
    {Column::userName, {"=", compareEqual}}, {Column::password, {"=", compareEqual}},
    {Column::gameId, {"=", compareEqual}}, {Column::gameName, {"LIKE", compareStartWith}},
    {Column::platform, {"=", compareEqual}}, {Column::year, {"=", compareEqual}},
    {Column::genre, {"=", compareEqual}}, {Column::publisher, {"LIKE", compareStartWith}},
    {Column::criticscore, {"=", compareEqual}}, {Column::rating, {"=", compareEqual}}
};


bool compareEqual(std::string_view value, std::string_view search)
{
    return value == search;
}

bool compareStartWith(std::string_view value, std::string_view search)
{
    if (search.size() > value.size()) {return false;}

    return std::equal(search.begin(), search.end(), value.begin(), [](char a, char b)
    {
        return std::tolower(a) == std::tolower(b);
    });
}