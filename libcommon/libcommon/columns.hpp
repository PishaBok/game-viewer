#pragma once

enum class Column
{
    username,
    password, 
    name,
    platform,
    year,
    genre,
    criticscore,
    rating
};

// std::map<Column, std::function<void(const QVariant)>> columnToVariant
// {
//     {Column::username, [](const std::string value)}
// }

struct ColumnToStringMap
{
    static constexpr std::string operator[](Column column) noexcept
    {
        switch (column)
        {
            case Column::username: return "username";
            case Column::password: return "password";
            case Column::name: return "name";
            case Column::platform: return "platform";
            case Column::year: return "year";
            case Column::genre: return "genre";
            case Column::criticscore: return "criticscore";
            case Column::rating: return "rating";
            default:
                return "undefined";
        }
    }

    static constexpr Column operator[](std::string_view columnName) noexcept
    {
        if (columnName == "username") {return Column::username;}
        else if (columnName == "password") {return Column::password;}
        else if (columnName == "name") {return Column::name;}
        else if (columnName == "platform") {return Column::platform;}
        else if (columnName == "year") {return Column::year;}
        else if (columnName == "genre") {return Column::genre;}
        else if (columnName == "criticscore") {return Column::criticscore;}
        else if (columnName == "rating") {return Column::rating;}
        else
        {
            return Column();
        }
    }
};