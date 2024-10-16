#include <server/database_manager.hpp>

DatabaseManager::DatabaseManager(ConnectionParams conParams, QObject* parent) : 
    QObject(parent), _params{conParams}, _mainConnection{"mainConnection"},
    _compareFunctions{{CompareOperator::equal, "="}, {CompareOperator::like, "LIKE"}},
    _queryString {R"(
    SELECT name, platform, year, genre, publisher, criticscore, developer, rating 
    FROM game 
    WHERE 1 = 1
    )"}, _orderBy{"ORDER BY id, name, platform, year, genre, publisher, criticscore, developer, rating"}
{
    init();
}

DatabaseManager::~DatabaseManager()
{
    for (const QString& con: _activeConnections)
    {
        disconnect(con);
    }
}


bool DatabaseManager::connect(const QString &conName)
{
    std::scoped_lock<std::mutex> lock(_connectionsMutex);

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", conName);

    db.setDatabaseName(_params.database);
    db.setHostName(_params.host);
    db.setUserName(_params.user);
    db.setPassword(_params.password);

    _activeConnections.push_back(conName);

    return db.open();
}

void DatabaseManager::disconnect(const QString& connection)
{
    std::scoped_lock<std::mutex> lock(_connectionsMutex);

    auto found = std::find(_activeConnections.begin(), _activeConnections.end(), connection);
    if (!(found == _activeConnections.end()))
    {
        QSqlDatabase::database(connection).close();
        QSqlDatabase::removeDatabase(connection);
        _activeConnections.erase(found);
    }
}

std::unique_ptr<QSqlQuery> DatabaseManager::getQuery(const QString& connectionName, const QueryParams& params)
{
    auto found = std::find(_activeConnections.begin(), _activeConnections.end(), connectionName);
    if (found == _activeConnections.end())
    {
        connect(connectionName);
    }

    auto query{std::make_unique<QSqlQuery>(QSqlDatabase::database(connectionName))};
    QString queryStr{getQueryStr(params)};
    query->prepare(queryStr);
    bindValues(*query, params.filter);

    return std::move(query);
}

bool DatabaseManager::compare(const QSqlQuery &data, const std::map<QString, QVariant> &search)
{
    for (const auto& searchValue: search)
    {
        QVariant value = data.value(searchValue.first);

        switch(_columnsCompareMap.at(searchValue.first))
        {
            case CompareOperator::equal:
                if (value != searchValue.second) {return false;}
                break;
            case CompareOperator::like:
                if (!value.toString().toLower().startsWith(searchValue.second.toString().toLower())) {return false;}
                break;
                 
        }
    }

    return true;
}

void DatabaseManager::init()
{
    connect(_mainConnection);

    QSqlQuery query{QSqlDatabase::database(_mainConnection)};
    query.exec(_queryString + " LIMIT 1");
    QSqlRecord record = query.record();
    for (size_t i{0}; i < record.count(); ++i)
    {
        QSqlField field = record.field(i);
        _columnsCompareMap[field.name()] = getOperator(field.metaType().id());
    }
}

CompareOperator DatabaseManager::getOperator(int typeId)
{
    switch (typeId) {
        case QMetaType::QString:
            return CompareOperator::like;
        case QMetaType::Int: case QMetaType::Double: case QMetaType::QDate:
        case QMetaType::QTime: case QMetaType::QDateTime: case QMetaType::Bool:
            return CompareOperator::equal;
        default:
            return CompareOperator::equal;
    }
}

QString DatabaseManager::getQueryStr(const QueryParams &params) const
{
    QString queryStr{_queryString};
    for (const auto& el: params.filter)
    {
        queryStr += " AND (" + el.first + " " + _compareFunctions.at(_columnsCompareMap.at(el.first)) + " :" + el.first + ")";
    }

    queryStr += (params.limit != 0) ? " LIMIT " + QString::number(params.limit) : "";
    queryStr += (params.offset != 0) ? " OFFSET " + QString::number(params.offset) : "";
    queryStr += " " + _orderBy;

    return queryStr;
}

void DatabaseManager::bindValues(QSqlQuery &query, const std::map<QString, QVariant> &values)
{
    for (const auto& value: values)
    {
        QString stringVal = value.second.toString();

        if (_columnsCompareMap.at(value.first) == CompareOperator::like) {stringVal += "%";}
        query.bindValue(":" + value.first, stringVal);
    }
}