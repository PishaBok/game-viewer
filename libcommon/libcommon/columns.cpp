#include <libcommon/columns.hpp>

const std::map<Column, std::string> columnToStringMap = 
{
    {Column::userName, "username"}, {Column::password, "password"},
    {Column::gameId, "gameid"}, {Column::gameName, "gamename"},
    {Column::platform, "platform"}, {Column::year, "year"},
    {Column::genre, "genre"}, {Column::publisher, "publisher"},
    {Column::criticscore, "criticscore"}, {Column::rating, "rating"},
    {Column::gameimg, "gameimg"}
};

const std::map<std::string, Column> columnFromStringMap = 
{
    {"username", Column::userName}, {"password", Column::password},
    {"gameid", Column::gameId}, {"gamename", Column::gameName},
    {"platform", Column::platform}, {"year", Column::year},
    {"genre", Column::genre}, {"publisher", Column::publisher},
    {"criticscore", Column::criticscore}, {"rating", Column::rating},
    {"gameimg", Column::gameimg}
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




const std::map<CompareType, std::string> compareTypeToTitle =
{
    {CompareType::endsWith, "оканчивается на..."},
    {CompareType::startsWith, "начинается с..."},
    {CompareType::includes, "включает..."},
    {CompareType::equals, "совпадает с..."}
};

const std::map<std::string, CompareType> compareTypeFromTitle =
{
    {"оканчивается на...", CompareType::endsWith},
    {"начинается с...", CompareType::startsWith},
    {"включает...", CompareType::includes},
    {"совпадает с...", CompareType::equals}
};

const std::map<Column, std::function<bool(std::string_view, std::string_view)>> defautCompareMap =
{
    {Column::userName, compareEquals}, {Column::password, compareEquals},
    {Column::gameId, compareStartsWith}, {Column::gameName, compareStartsWith},
    {Column::platform, compareStartsWith}, {Column::year, compareData},
    {Column::genre, compareStartsWith}, {Column::publisher, compareStartsWith},
    {Column::criticscore, compareData}, {Column::rating, compareStartsWith}
};





bool compareStartsWith(std::string_view value, std::string_view search)
{
    if (search.size() > value.size()) {return false;}

    return std::equal(search.begin(), search.end(), value.begin(), [](char a, char b)
    {
        return std::tolower(a) == std::tolower(b);
    });
}

bool compareEndsWith(std::string_view value, std::string_view search)
{
    if (search.size() > value.size()) {return false;}

    auto valueIt = value.end() - search.size();
    return std::equal(search.begin(), search.end(), valueIt, [](char a, char b) {
        return std::tolower(a) == std::tolower(b);
    });
}

bool compareIncludes(std::string_view value, std::string_view search)
{
    if (search.empty()) {return true;}
    if (search.size() > value.size()) {return false;}

    auto it = std::search(value.begin(), value.end(), search.begin(), search.end(),
                          [](char a, char b) {
                              return std::tolower(a) == std::tolower(b);
                          });
    return it != value.end();
}

bool compareEquals(std::string_view value, std::string_view search)
{
    return value == search;
}

bool compareData(std::string_view value, std::string_view search)
{
    auto searchValues = splitData(search, ':');

    int intValue{};
    std::from_chars(value.data(), value.data() + value.size(), intValue);
    int leftBound{};
    std::from_chars(searchValues.front().data(), searchValues.front().data() + searchValues.front().size(), leftBound);
    int rightBound{};
    std::from_chars(searchValues.back().data(), searchValues.back().data() + searchValues.back().size(), rightBound);

    if (intValue >= leftBound && intValue <= rightBound)
    {
        return true;
    }

    return false;
}

std::vector<std::string_view> splitData(std::string_view dataSearch, char delimeter)
{
    std::vector<std::string_view> result;
    std::size_t start = 0;
    std::size_t end = 0;

    while ((end = dataSearch.find(delimeter, start)) != std::string_view::npos)
    {
        result.emplace_back(dataSearch.substr(start, end - start));
        start = end + 1;
    }

    if (start < dataSearch.size()) 
    {
        result.emplace_back(dataSearch.substr(start));
    }

    return result;
}