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

    qDebug() << "Connection Succed!";

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

    for (const auto& el: params.filter)
    {
        if (el.second.value == "") {continue;}

        bindValue(*query, el);
    }

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
        if (el.second.value == "") {continue;}

        queryStr += " AND " + getFilterPart(el);
    }

    queryStr += !params.orderBy.empty() ? " " + QString::fromStdString(params.orderBy) : "";
    queryStr += params.limit != 0 ? " LIMIT " + QString::number(params.limit) : "";
    queryStr += params.offset != 0 ? " OFFSET " + QString::number(params.offset) : "";

    return queryStr;
}

QString DatabaseManager::getFilterPart(const std::pair<Column, FilterParams>& filter) const
{
    QString columnName = QString::fromStdString(columnToStringMap.at(filter.first));

    switch (filter.second.type)
    {
        case CompareType::endsWith: case CompareType::startsWith:
        case CompareType::includes:
            return columnName + " LIKE :" + columnName;
        case CompareType::equals:
            return columnName + " = :" + columnName;
        case CompareType::data:
            QStringList years = QString::fromStdString(filter.second.value).split("|");
            int leftBound = years.front().toInt();
            int rightBound = years.back().toInt();

            if (leftBound && rightBound) {return columnName + " BETWEEN :yearstart AND :yearend";} 
            else if (leftBound) {return columnName + " >= :yearstart";}
            else if (rightBound) {return columnName + " <= :yearend";}
            return QString();
    }
}

void DatabaseManager::bindValue(QSqlQuery& query, const std::pair<Column, FilterParams>& filter)
{
    QString columnName = QString::fromStdString(columnToStringMap.at(filter.first));
    QString filterValue = QString::fromStdString(filter.second.value);

    switch (filter.second.type)
    {
        case CompareType::endsWith:
            filterValue = "%" + filterValue;
            query.bindValue(":" + columnName, filterValue);
            break;
        case CompareType::startsWith:  
            filterValue += "%";
            query.bindValue(":" + columnName, filterValue);
            break;
        case CompareType::includes:
            filterValue = "%" + filterValue + "%";
            query.bindValue(":" + columnName, filterValue);
            break;
        case CompareType::equals:
            query.bindValue(":" + columnName, filterValue);
            break;
        case CompareType::data:
            QStringList years = filterValue.split("|");
            query.bindValue(":" + columnName + "start", years.front());
            query.bindValue(":" + columnName + "end", years.back());
            break;
    }
}

size_t DatabaseManager::recordCount(const std::string& tableName, const std::map<Column, FilterParams> filter)
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