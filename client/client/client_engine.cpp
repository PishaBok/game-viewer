#include <client/client_engine.hpp>
#include "client_engine.hpp"

ClientEngine::ClientEngine(QObject *parent)
    : QObject{parent}, _recordsOnPage{9}, _currentPage{1}, _searchState{false},
    _currentSearchRecord{-1},
    _responseToFunc
    {
        {RequestType::page, std::bind(&ClientEngine::pageResponse, this, std::placeholders::_1)},
        {RequestType::pageCount, std::bind(&ClientEngine::pageCountResponse, this, std::placeholders::_1)},
        {RequestType::uniqueValues, std::bind(&ClientEngine::uniqueValuesResponse, this, std::placeholders::_1)},
        {RequestType::search, std::bind(&ClientEngine::searchResponse, this, std::placeholders::_1)}
    },
    _responseFactory
    {
        {RequestType::page, [](){return std::make_unique<PageResponse>();}},
        {RequestType::pageCount, [](){return std::make_unique<PageCountResponse>();}},
        {RequestType::uniqueValues, [](){return std::make_unique<UniqueValuesResponse>();}},
        {RequestType::search, [](){return std::make_unique<SearchResponse>();}}
    }
{}

ClientEngine::~ClientEngine()
{}

void ClientEngine::start()
{
    _socket = std::make_unique<Socket>("localhost", 9999);

    connect(_socket->tcpSocketPtr(), &QTcpSocket::connected, this, &ClientEngine::connectedToServer);
    connect(this, &ClientEngine::sendToServer, _socket.get(), &Socket::sendToServer);
    connect(_socket.get(), &Socket::processResponse, this, &ClientEngine::processResponse);

    _socket->connectToHost();
}

void ClientEngine::connectedToServer()
{
    PageRequest pageRequest{_currentPage, _filter};
    emit sendToServer(QJsonDocument(pageRequest.serialize()).toJson());
    pageCount();

    uniqueValues(Column::genre);
    uniqueValues(Column::rating);
}


void ClientEngine::page(const int pageNumber)
{
    // Проверка на выход за границы
    if (pageNumber < 1 || pageNumber > _pageCount) {return;}
    _currentPage = pageNumber;

    // Поиск в кэше
    if (findInCache(pageNumber)) {return;}

    emit setEnabledButtons(false);

    // Отправка запроса на сервер
    PageRequest request(pageNumber, _filter);
    emit sendToServer(QJsonDocument(request.serialize()).toJson());
}

void ClientEngine::filter(const std::map<Column, FilterParams>& filter)
{
    if (filter == _filter) {return;}

    _filter = filter;
    _savedPages.clear();
    _pageCount = 1000;

    page(1);
    pageCount();

    emit updateActiveFilter(_filter);
}

void ClientEngine::searchOn(const std::map<Column, QString>& search)
{
    _searchState = true;
    if (search == _searchMap) {return;}
    
    _searchMap = search;
    _searchResult.clear();

    SearchRequest request{_filter, _searchMap};
    emit sendToServer(QJsonDocument(request.serialize()).toJson());
}

void ClientEngine::searchOff()
{
    _searchState = false;
    emit searchSwitch(_searchState);
}

void ClientEngine::searchRecord(const int recordNumber)
{
    if (recordNumber < 1 || recordNumber > _searchResult.size()) {return;}
    int record = _searchResult.at(recordNumber - 1);

    qDebug() << record;
    auto [pageNumber, newNumber] = recordNumberToPage(record);
    _currentSearchRecord = recordNumber;
    emit updateSearchCounter(QString::number(_currentSearchRecord), QString::number(_searchRecordCount));

    if (pageNumber != _currentPage) {page(pageNumber);}
    else
    {   if (_currentSearchRecord >= 0)
        {
            emit highlightCard(_currentSearchRecord % _recordsOnPage);
        }
        
        _currentSearchRecord = -1;
    }
}

void ClientEngine::pageCount()
{
    PageCountRequest request(_recordsOnPage, _filter);
    emit sendToServer(QJsonDocument(request.serialize()).toJson());
}

void ClientEngine::uniqueValues(Column column)
{
    UniqueValuesRequest request{column};
    emit sendToServer(QJsonDocument(request.serialize()).toJson());
}

void ClientEngine::processResponse(const QJsonObject& json)
{
    // Получаем тип ответа сервера
    RequestType type = static_cast<RequestType>(json.value("type").toInt());

    auto response = _responseFactory[type]();
    response->deserialize(json);

    _responseToFunc[type](std::move(response));
}

void ClientEngine::pageResponse(const std::unique_ptr<Response>& response)
{
    _savedPages.clear();

    QJsonObject data = response->data();

    QJsonArray modelsArr = data.value("models").toArray();
    for (const QJsonValue& value: modelsArr)
    {
        QJsonObject jsonObj = value.toObject();
        clib::TableModel model;
        model.loadJson(jsonObj.value("model").toObject());
        _savedPages[jsonObj.value("page").toInt()] = model;
    }

    emit updatePage(_savedPages.at(_currentPage));
    emit updatePageCounter(QString::number(_currentPage), QString::number(_pageCount));
    emit setEnabledButtons(true);
    if (_currentSearchRecord >= 0)
    {
        emit highlightCard(_currentSearchRecord % _recordsOnPage);
    }
    _currentSearchRecord = -1;
}

std::pair<int, int> ClientEngine::recordNumberToPage(const int recordNumber)
{
    int page = ceil((recordNumber + 1) / static_cast<double>(_recordsOnPage));
    int newNumber = (recordNumber % _recordsOnPage);

    return {page, newNumber};
}

void ClientEngine::pageCountResponse(const std::unique_ptr<Response>& response)
{
    QJsonObject data = response->data();
    _pageCount = data.value("pageCount").toInt();
    
    emit updatePageCounter(QString::number(_currentPage), QString::number(_pageCount));
}

void ClientEngine::uniqueValuesResponse(const std::unique_ptr<Response> &response)
{
    QJsonObject data = response->data();
    Column column = static_cast<Column>(data["column"].toInt());

    QStringList values;
    for (const auto& value: data["values"].toArray())
    {
        values.append(value.toString());
    }

    emit setDropListValue({column, values});
}

void ClientEngine::searchResponse(const std::unique_ptr<Response>& response)
{
    QJsonObject data = response->data();

    for (const auto& value: data.value("recordIds").toArray())
    {
        _searchResult.push_back(value.toInt());
    }
    
    _searchRecordCount = _searchResult.size();
    searchRecord(1);

    emit searchSwitch(_searchState);
}

bool ClientEngine::findInCache(const int pageN)
{
    auto found = _savedPages.find(pageN);
    if (found != _savedPages.end())
    {
        emit updatePage(found->second);
        emit updatePageCounter(QString::number(_currentPage), QString::number(_pageCount));
        if (_currentSearchRecord >= 0)
        {
            emit highlightCard(_currentSearchRecord % _recordsOnPage);
        }
        _currentSearchRecord = -1;
        return true;
    }

    return false;
}
