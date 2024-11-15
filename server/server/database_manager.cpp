#include <server/database_manager.hpp>

DatabaseManager::DatabaseManager(ConnectionParams conParams, QObject* parent) : 
    QObject(parent), _dbParams{conParams}, _mainConnection{"mainConnection"}
{}

DatabaseManager::~DatabaseManager()
{}


bool DatabaseManager::connect(const QString &conName)
{
    std::scoped_lock<std::mutex> lock(_connectionsMutex);

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", conName);

    db.setDatabaseName(_dbParams.database);
    db.setHostName(_dbParams.host);
    db.setUserName(_dbParams.user);
    db.setPassword(_dbParams.password);

    return db.open();
}

void DatabaseManager::disconnect(const QString& connection)
{
    std::scoped_lock<std::mutex> lock(_connectionsMutex);

    QSqlDatabase::database(connection).close();
    QSqlDatabase::removeDatabase(connection);
}

std::unique_ptr<QSqlQuery> DatabaseManager::getQuery(const QString& connectionName, const std::pair<std::string, std::string>& sqlQuery, const QueryParams& params)
{
    connect(connectionName);

    auto [queryTemplate, orderBy] = sqlQuery;
    auto query{std::make_unique<QSqlQuery>(QSqlDatabase::database(connectionName))};
    QString queryStr{getQueryStr(queryTemplate, orderBy, params)};

    query->prepare(queryStr);
    bindValues(*query, params.filter);

    return std::move(query);
}

// bool DatabaseManager::compare(const QSqlQuery &data, const std::map<QString, QVariant> &search)
// {
//     for (const auto& searchValue: search)
//     {
//         QVariant value = data.value(searchValue.first);

//         switch(_columnsCompareMap.at(searchValue.first))
//         {
//             case CompareOperator::equal:
//                 if (value != searchValue.second) {return false;}
//                 break;
//             case CompareOperator::like:
//                 if (!value.toString().toLower().startsWith(searchValue.second.toString().toLower())) {return false;}
//                 break;
                 
//         }
//     }

//     return true;
// }

QString DatabaseManager::getQueryStr(const std::string queryTemplate, const std::string orderBy, const QueryParams &params) const
{
    QString queryStr{QString::fromStdString(queryTemplate)};
    for (const auto& el: params.filter)
    {
        if (el.second == "") {continue;}
        queryStr += " AND (" + ColumnToStringMap{}[el.first] + " " + ColumnToCompareFunc{}[el.first] + " :" + ColumnToStringMap{}[el.first] + ")";
    }

    queryStr += !orderBy.empty() ? " " + QString::fromStdString(orderBy) : "";
    queryStr += params.limit != 0 ? " LIMIT " + QString::number(params.limit) : "";
    queryStr += params.offset != 0 ? " OFFSET " + QString::number(params.offset) : "";

    return queryStr;
}

void DatabaseManager::bindValues(QSqlQuery &query, const std::map<Column, QString> &values)
{
    for (const auto& value: values)
    {
        QString stringVal = value.second;

        if (ColumnToCompareFunc{}[value.first] == "LIKE") {stringVal += "%";}
        query.bindValue(":" + QString::fromStdString(ColumnToStringMap{}[value.first]), stringVal);
    }
}