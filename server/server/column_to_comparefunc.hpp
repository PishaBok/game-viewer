#pragma once

#include <libcommon/columns.hpp>

struct ColumnToCompareFunc
{
    static constexpr std::string operator[](Column column) noexcept
    {
        switch (column)
        {
            case Column::name: case Column::platform: case Column::genre:
                return "=";
            case Column::username: case Column::password: case Column::year:
            case Column::criticscore: case Column::rating:
                return "LIKE";
            default:
                return "=";
        }
    }
};