#include <client/client_engine.hpp>
#include "client_engine.hpp"


ClientEngine::ClientEngine(QObject *parent)
    : QObject{parent}, _currentPage{1},
    _buttonToFunc
    {
        {Button::stepBack, std::bind(&ClientEngine::stepBack, this)},
        {Button::stepForward, std::bind(&ClientEngine::stepForward, this)},
        {Button::filter, std::bind(&ClientEngine::filter, this)},
        {Button::search, std::bind(&ClientEngine::search, this)}
    },
    _responseConvertMap
    {
        {RequestType::page, std::bind(&ClientEngine::pageResponse, this, std::placeholders::_1)}
    }
{}

ClientEngine::~ClientEngine()
{
    qDebug() << "Client destructor. Thread: " << QThread::currentThread();
}

void ClientEngine::initSocket()
{
    qDebug() << "Init socket. Thread: " << QThread::currentThread();
    _socket = std::make_unique<Socket>("localhost", 9999);

    connect(this, &ClientEngine::sendToServer, _socket.get(), &Socket::sendToServer);
    connect(_socket.get(), &Socket::dataReceived, this, &ClientEngine::dataReceived);

    page();
}

void ClientEngine::processButton(const Button button)
{
    _buttonToFunc[button]();
}

void ClientEngine::page()
{
    QJsonObject filterJson;

    for (auto it{_filterMap.begin()}; it != _filterMap.end(); ++it)
    {
        QJsonObject obj;
        obj.insert("type", _columnsType.at(it->first));
        obj.insert("value", it->second);

        filterJson.insert(it->first, obj);
    }

    QJsonObject paramsJson;
    paramsJson.insert("page", _currentPage);
    paramsJson.insert("filter", filterJson);

    QJsonObject messageJson;
    messageJson.insert("type", RequestType::page);
    messageJson.insert("params", paramsJson);

    emit sendToServer(QJsonDocument(messageJson).toJson());
}

void ClientEngine::filter()
{

}

void ClientEngine::search()
{

}


void ClientEngine::pageResponse(const QJsonValue& data)
{
    _savedPages.clear();

    QJsonArray modelsArr{data.toArray()};
    for (const QJsonValue& value: modelsArr)
    {
        QJsonObject jsonObj = value.toObject();
        clib::TableModel model;
        model.loadJson(jsonObj.value("model").toObject());
        _savedPages[jsonObj.value("page").toInt()] = model;
    }

    emit updatePage(_savedPages.at(_currentPage - 1));
}

void ClientEngine::stepBack()
{
    if (_currentPage > 1)
    {
        --_currentPage;
        page();
    }
}

void ClientEngine::stepForward()
{
    if (_currentPage < _pageCount)
    {
        ++_currentPage;
        page();
    }
}


void ClientEngine::dataReceived(const QJsonObject& json)
{
    qDebug() << "Data received. Thread: " << QThread::currentThread();
    // Получаем тип ответа сервера
    RequestType type = static_cast<RequestType>(json.value("type").toInt());
    // Вызывает соответствующую функцию-обработчик
    _responseConvertMap[type](json.value("data"));
}
