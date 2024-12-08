#include <server/request_executors/unique_values.hpp>

UniqueValuesExecutor::UniqueValuesExecutor(DatabaseManager& dbManager)
    : _dbManager(dbManager)
{}

std::unique_ptr<Response> UniqueValuesExecutor::execute()
{
    std::unique_ptr<UniqueValuesResponse> response{std::make_unique<UniqueValuesResponse>(_column, getUniqueValues())};

    return response;
}


QStringList UniqueValuesExecutor::getUniqueValues()
{
    QString strQuery = "SELECT DISTINCT " + QString::fromStdString(columnToStringMap.at(_column)) + " FROM game";

    auto query = _dbManager.getQuery(strQuery);
    query->exec();

    QStringList values;

    while(query->next())
    {
        QString value = query->value(0).toString();
        if (value.isEmpty()) {continue;}
        values.append(query->value(0).toString());
    }

    _dbManager.cleanupQuery(query.get());

    return std::move(values);
}