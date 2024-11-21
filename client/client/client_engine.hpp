#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include <client/socket.hpp>
#include <libcommon/columns.hpp>
#include <libcommon/requests/page.hpp>
#include <libcommon/requests/page_count.hpp>

class ClientEngine: public QObject
{
    Q_OBJECT
public:
    ClientEngine(QObject* parent = nullptr);
    ~ClientEngine();

    void start();

    // Обработчики кнопок
    void page(const int pageNumber);
    void filter(const std::map<Column, QString>& filter);
    void search(const std::map<Column, QString>& search);
    void pageCount();
private:
    std::map<RequestType, std::function<std::unique_ptr<Response>()>> _responseFactory;
    std::map<RequestType, std::function<void(const std::unique_ptr<Response>&)>> _responseToFunc;

    // Сокет для подключения к серверу
    std::unique_ptr<Socket> _socket;

    // Настройки программы
    size_t _recordsOnPage;
    int _currentPage;
    int _currentSearchRecord;
    std::map<Column, QString> _filterMap;
    std::map<Column, QString> _searchMap;

    // Данные от сервера
    int _pageCount;  // Количество страниц
    std::map<int, clib::TableModel> _savedPages;  // Скачанные страницы
    int _searchRecordCount; // Количество записей соответстующих поиску
    std::vector<int> _searchPageNumbers; // Страницы на которых есть записи соответствующие поиску



    bool findInCache(const int pageN);

    // Обработчики ответов сервера
    void pageResponse(const std::unique_ptr<Response>& response);
    void pageCountResponse(const std::unique_ptr<Response>& response);
public slots:
    void connectedToServer();
    void processResponse(const QJsonObject& json);
signals:
    void sendToServer(const QByteArray& message);

    void updatePage(const clib::TableModel& model);
    void updatePageCounter(const QString &pageNumber, const QString& pageCount);
    void updateActiveFilter(const std::map<Column, QString>& filters);
    void setEnabledButtons(const bool flag);
};

