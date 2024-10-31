#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include <client/socket.hpp>
#include <client/buttons.hpp>
#include <libcommon/request_types.hpp>
#include <libcommon/columns.hpp>
#include <libcommon/requests/page.hpp>

class ClientEngine: public QObject
{
    Q_OBJECT
public:
    ClientEngine(QObject* parent = nullptr);
    ~ClientEngine();

private:
    std::map<Button, std::function<void()>> _buttonToFunc;
    std::map<RequestType, std::function<void(const std::unique_ptr<Response>&)>> _responseToFunc;
    std::map<RequestType, std::function<std::unique_ptr<Response>()>> _responseFactory
    {
        {RequestType::page, []() {return std::make_unique<PageResponse>();}}
    };

    // Настройки программы
    int _currentPage;
    std::map<Column, QString> _filterMap;
    std::map<Column, QString> _searchMap;

    // Данные от сервера
    int _pageCount{1000};
    std::map<QString, QString> _columnsType
    {
        {"name", "string"},
        {"platform", "string"},
        {"year", "int"},
        {"genre", "string"},
        {"criticscore", "int"},
        {"rating", "string"}
    };
    std::map<int, clib::TableModel> _savedPages;

    // Сокет для подключения к серверу
    std::unique_ptr<Socket> _socket;

    void stepBack();
    void stepForward();
    bool findInCache(const int pageN);

    // Обработчики кнопок
    void pageButton();
    void filterButton();
    void searchButton();

    // Обработчики ответов сервера
    void pageResponse(const std::unique_ptr<Response>& response);
public slots:
    void initSocket();
    void processButton(const Button button);
    void processResponse(const QJsonObject& json);
signals:
    void sendToServer(const QByteArray& message);

    void updatePage(const clib::TableModel& model);
};
