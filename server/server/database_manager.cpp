#include <server/database_manager.hpp>

DatabaseManager::DatabaseManager(ConnectionParams conParams, QObject* parent) : 
    QObject(parent), _dbParams{conParams}, _mainConnection{"mainConnection"}
{}

DatabaseManager::~DatabaseManager()
{}


QString DatabaseManager::generateConnectionName() const
{
    std::ostringstream oss;
    oss << std::this_thread::get_id();  // Получаем текущий thread id
    return QString("db_connection_%1").arg(QString::fromStdString(oss.str()));
}

bool DatabaseManager::connect(const QString &conName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", conName);

    db.setDatabaseName(_dbParams.database);
    db.setHostName(_dbParams.host);
    db.setUserName(_dbParams.user);
    db.setPassword(_dbParams.password);

    return db.open();
}

void DatabaseManager::disconnect(const QString& connection)
{
    QSqlDatabase::database(connection).close();
    QSqlDatabase::removeDatabase(connection);
}

std::unique_ptr<QSqlQuery> DatabaseManager::getQuery(const QueryParams& params)
{
    std::scoped_lock<std::mutex> lock(_connectionsMutex);

    QString connectionName{generateConnectionName()};
    connect(connectionName);
    auto query{std::make_unique<QSqlQuery>(QSqlDatabase::database(connectionName))};
    _queryMap[query.get()] = connectionName;

    QString queryStr{getQueryStr(params)};

    query->prepare(queryStr);
    bindValues(*query, params.filter);

    return std::move(query);
}

void DatabaseManager::cleanupQuery(QSqlQuery* query)
{
    std::scoped_lock<std::mutex> lock(_connectionsMutex);

    QString connectionName = _queryMap.at(query);
    if (!connectionName.isEmpty())
    {
        disconnect(connectionName);
        _queryMap.erase(query);
    }
}


QString DatabaseManager::getQueryStr(const QueryParams &params) const
{
    QString queryStr{QString::fromStdString(params.strQueryTemplate)};
    for (const auto& el: params.filter)
    {
        if (el.second == "") {continue;}
        queryStr += " AND (" + columnToStringMap.at(el.first) + " " + comparisonMap.at(el.first).compareWord + " :" + columnToStringMap.at(el.first) + ")";
    }

    queryStr += !params.orderBy.empty() ? " " + QString::fromStdString(params.orderBy) : "";
    queryStr += params.limit != 0 ? " LIMIT " + QString::number(params.limit) : "";
    queryStr += params.offset != 0 ? " OFFSET " + QString::number(params.offset) : "";

    return queryStr;
}

void DatabaseManager::bindValues(QSqlQuery &query, const std::map<Column, QString> &values)
{
    for (const auto& value: values)
    {
        QString stringVal = value.second;

        if (comparisonMap.at(value.first).compareWord == "LIKE") {stringVal += "%";}
        query.bindValue(":" + QString::fromStdString(columnToStringMap.at(value.first)), stringVal);
    }
}


size_t DatabaseManager::recordCount(const std::string& tableName, const std::map<Column, QString> filter)
{
    std::string strQuery{"SELECT COUNT(*) FROM "+ tableName + " WHERE 1 = 1"};

    auto query = getQuery({strQuery, "", filter, 0, 0});
    query->exec();

    size_t recordCount{0};
    if (query->next()) 
    {
        recordCount = query->value(0).toUInt(); 
    }

    cleanupQuery(query.get());
    return recordCount;
}