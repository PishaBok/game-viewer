#include <server/requests/base.hpp>

Request::Request(DatabaseManager& dbManager)
    : _dbManager{dbManager}
{}

void Request::setRequest(const QJsonObject &data)
{
    if (!validateJson(data))
    {
        throw std::invalid_argument("Invalid JSON format in Request!");
    }

    parseJson(data);
}


// #include "request.hpp"

// Request::Request(const QJsonObject& data, DatabaseManager& dbManager, QObject* parent) : QObject(parent),
//     _dbManager{dbManager}, _baseConnectionName{"request:" + QString::number(reinterpret_cast<uintptr_t>(this))}, 
//     _finished{false}
// {
//     if (!validateJson(data))
//     {
//         throw std::invalid_argument("Invalid JSON format in Request!");
//     }

//     parseJson(data);
// }

// Response Request::process()
// {
//     Response result;

//     _rowCount = std::async(std::launch::async, &Request::rowCount, this);
//     _searchOffset = std::async(std::launch::async, &Request::startSearchThreads, this);



//     result.setPageNumber(_params.page);
//     result.setModel(createPage(_params.page));

//     _finished = true;
//     return result;
// }


// bool Request::isFinished()
// {
//     return _finished;
// }

// int Request::rowCount()
// {
//     QString connectionName{_baseConnectionName + "_rowCount_" + generateRandomString()};
//     auto query{_dbManager.getQuery(connectionName, {_params.filter, 0, 0})};
//     query->exec();
//     int rowCount{0};
//     while (query->next())
//     {
//         ++rowCount;
//     }

//     _dbManager.disconnect(connectionName);

//     return rowCount;
// }

// int Request::startSearchThreads()
// {
//     const int rowsToThread{1000};
//     int threadCount{static_cast<int>(ceil(_rowCount.get() / static_cast<double>(rowsToThread)))};
//     std::vector<std::thread> threads(threadCount);

//     std::atomic<int> result{_rowCount.get() + 1};

//     for (int i{0}; i < threadCount; ++i)
//     {
//         threads[i] = std::thread{&Request::locate, this, _baseConnectionName + "_search:" + QString::number(i) + "_" + generateRandomString(),
//                  rowsToThread, rowsToThread * i, std::ref(result)};
//     }
//     for (int i{0}; i < threadCount; ++i)
//     {
//         threads[i].join();
//     }

//     return result;
// }

// void Request::locate(const QString &connectionName, const int limit, const int offset, std::atomic<int> &resVar)
// {
//     auto query{_dbManager.getQuery(connectionName, {_params.filter, limit, offset})};
//     query->exec();

//     int row{offset};
//     while(query->next())
//     {
//         if (_dbManager.compare(*query, _params.search))
//         {
//             resVar.store(resVar.load() < offset ? resVar.load() : row);
//             break;
//         }
//         ++row;
//     }
//     _dbManager.disconnect(connectionName);
// }

// std::vector<TableModel> Request::startPageThreads()
// {
//     const int cacheSize(1);
//     const int pagesToMake{(cacheSize * 2) + 1};
//     std::vector<std::future<TableModel>> futureModels(pagesToMake);
//     std::vector<TableModel> result(pagesToMake);
 
//     for (int i{0}; i < pagesToMake; ++i)
//     {
//         futureModels[i] = std::async(std::launch::async, &Request::createPage, this, _params.page - cacheSize + i);
//     }

//     for (int i{0}; i < pagesToMake; ++i)
//     {
//         result[i] = futureModels[i].get();
//     }

//     return std::move(result);
// }

// TableModel Request::createPage(const int number)
// {
//     QString connectionName{_baseConnectionName + "_page_" + generateRandomString()};
//     auto query{_dbManager.getQuery(connectionName, {_params.filter, 15, 15 * (number - 1)})};
//     query->exec();

//     TableModel tableModel{std::move(*query)};
//     _dbManager.disconnect(connectionName);

//     return tableModel;
// }







// void Request::parseJson(const QJsonObject &json)
// {
//     _reqType = static_cast<ReqType>(json["reqType"].toInt());

//     QJsonObject params{json["params"].toObject()};

//     _params.page = params["page"].toInt();
//     _params.filter = jsonToMap(params["filter"].toObject());
//     _params.search = jsonToMap(params["search"].toObject());
// }

// std::map<QString, QVariant> Request::jsonToMap(const QJsonObject &json) const
// {
//     std::map<QString, QVariant> result;

//     for (const QString& key: json.keys())
//     {
//         QJsonObject typeValueObj = json[key].toObject();
//         result[key] = parseTypedValue(typeValueObj); 
//     }

//     return std::move(result);
// }

// QVariant Request::parseTypedValue(const QJsonObject& typedValueObj) const
// {
//     QString type = typedValueObj["type"].toString();

//     if (type == "int")
//     {
//         return typedValueObj["value"].toInt();
//     }
//     else if (type == "string")
//     {
//         return typedValueObj["value"].toString();
//     }

//     return QVariant();
// }

// bool Request::validateJson(const QJsonObject &json) const
// {
//     if (!json.contains("reqType") || !json.value("reqType").isDouble() || json.value("reqType").toInt() >= static_cast<int>(ReqType::max))
//     {
//         return false;
//     }

//     if (!json.contains("params") || !json.value("params").isObject())
//     {
//         return false;
//     }

//     return true;
// }
