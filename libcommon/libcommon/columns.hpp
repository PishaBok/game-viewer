#pragma once

#include <map>
#include <string>
#include <string_view>
#include <functional>

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
    rating
};

struct ComparisonSettings
{
    std::string compareWord;
    std::function<bool(std::string_view, std::string_view)> compareFunc;
};

extern const std::map<Column, std::string> columnToStringMap;
extern const std::map<std::string, Column> columnFromStringMap;

extern const std::map<Column, std::string> columnToTitleMap;
extern const std::map<std::string, Column> columnFromTitleMap;

extern bool compareEqual(std::string_view value, std::string_view search);
extern bool compareStartWith(std::string_view value, std::string_view search);
extern const std::map<Column, ComparisonSettings> comparisonMap;


// struct ColumnToStringMap
// {
//     static constexpr std::string operator[](Column column) noexcept
//     {
//         switch (column)
//         {
//             case Column::userName: return "username";
//             case Column::password: return "password";
//             case Column::gameId: return "gameid";
//             case Column::gameName: return "gamename";
//             case Column::platform: return "platform";
//             case Column::year: return "year";
//             case Column::genre: return "genre";
//             case Column::publisher: return "publisher";
//             case Column::criticscore: return "criticscore";
//             case Column::rating: return "rating";
//             default:
//                 return "undefined";
//         }
//     }

//     static constexpr Column operator[](std::string_view columnName) noexcept
//     {
//         if (columnName == "username") {return Column::userName;}
//         else if (columnName == "password") {return Column::password;}
//         else if (columnName == "gameid") {return Column::gameId;}
//         else if (columnName == "gamename") {return Column::gameName;}
//         else if (columnName == "platform") {return Column::platform;}
//         else if (columnName == "year") {return Column::year;}
//         else if (columnName == "genre") {return Column::genre;}
//         else if (columnName == "publisher") {return Column::publisher;}
//         else if (columnName == "criticscore") {return Column::criticscore;}
//         else if (columnName == "rating") {return Column::rating;}
//         else
//         {
//             return Column();
//         }
//     }
// };
