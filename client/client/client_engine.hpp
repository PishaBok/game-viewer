#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

#include <client/socket.hpp>
#include <client/buttons.hpp>
#include <libcommon/request_types.hpp>

class ClientEngine: public QObject
{
    Q_OBJECT
public:
    ClientEngine(QObject* parent = nullptr);
    ~ClientEngine();

private:
    std::map<Button, std::function<void()>> _buttonToFunc;
    std::map<RequestType, std::function<void(const QJsonValue&)>> _responseConvertMap;

    // Настройки программы
    int _currentPage;
    std::map<QString, QString> _filterMap;
    std::map<QString, QString> _searchMap;

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

    // Обработчики кнопок
    void page();
    void filter();
    void search();

    // Обработчики ответов сервера
    void pageResponse(const QJsonValue& data);
public slots:
    void initSocket();
    void processButton(const Button button);
    void dataReceived(const QJsonObject& json);
signals:
    void sendToServer(const QByteArray& message);

    void updatePage(const clib::TableModel& model);
};
