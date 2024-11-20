#include <client/client_engine.hpp>

ClientEngine::ClientEngine(QObject *parent)
    : QObject{parent}, _recordsOnPage{9}, _currentPage{1},
    _responseToFunc
    {
        {RequestType::page, std::bind(&ClientEngine::pageResponse, this, std::placeholders::_1)},
        {RequestType::pageCount, std::bind(&ClientEngine::pageCountResponse, this, std::placeholders::_1)}
    },
    _responseFactory
    {
        {RequestType::page, [](){return std::make_unique<PageResponse>();}},
        {RequestType::pageCount, [](){return std::make_unique<PageCountResponse>();}}
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
    PageRequest pageRequest{_currentPage, _filterMap};
    emit sendToServer(QJsonDocument(pageRequest.serialize()).toJson());
    pageCount();
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
    PageRequest request(pageNumber, _filterMap);
    emit sendToServer(QJsonDocument(request.serialize()).toJson());
}

void ClientEngine::filter(const std::map<Column, QString>& filter)
{
    if (filter == _filterMap) {return;}

    _filterMap = filter;
    _savedPages.clear();
    _pageCount = 1000;

    page(1);
    pageCount();
}

void ClientEngine::search(const std::map<Column, QString>& search)
{
    if (search == _searchMap) {return;}
}

void ClientEngine::pageCount()
{
    PageCountRequest request(_recordsOnPage, _filterMap);
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
    emit updatePageCounter(QString("%1/%2").arg(QString::number(_currentPage), QString::number(_pageCount)));
    emit setEnabledButtons(true);
}

void ClientEngine::pageCountResponse(const std::unique_ptr<Response>& response)
{
    QJsonObject data = response->data();
    _pageCount = data.value("pageCount").toInt();
    
    emit updatePageCounter(QString("%1/%2").arg(QString::number(_currentPage), QString::number(_pageCount)));
}






bool ClientEngine::findInCache(const int pageN)
{
    auto found = _savedPages.find(pageN);
    if (found != _savedPages.end())
    {
        emit updatePage(found->second);
        emit updatePageCounter(QString("%1/%2").arg(QString::number(_currentPage), QString::number(_pageCount)));
        return true;
    }

    return false;
}
