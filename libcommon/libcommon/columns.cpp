#include <libcommon/columns.hpp>

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