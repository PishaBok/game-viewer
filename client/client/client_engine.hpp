#pragma once

#include <thread>
#include <condition_variable>
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include <client/socket.hpp>
#include <libcommon/columns.hpp>
#include <libcommon/requests/page.hpp>
#include <libcommon/requests/page_count.hpp>
#include <libcommon/requests/unique_values.hpp>
#include <libcommon/requests/search.hpp>

class ClientEngine: public QObject
{
    Q_OBJECT
public:
    ClientEngine(QObject* parent = nullptr);
    ~ClientEngine();

    void start();

    // Обработчики кнопок
    void page(const int pageNumber);
    void filter(const std::map<Column, FilterParams>& filter);
    void searchOn(const std::map<Column, QString>& search);
    void searchOff();
    void searchRecord(const int recordNumber);
    void pageCount();
    void uniqueValues(Column column);
private:
    std::map<RequestType, std::function<std::unique_ptr<Response>()>> _responseFactory;
    std::map<RequestType, std::function<void(const std::unique_ptr<Response>&)>> _responseToFunc;

    // Сокет для подключения к серверу
    std::unique_ptr<Socket> _socket;

    // Настройки программы
    size_t _recordsOnPage;
    int _currentPage;
    int _currentSearchRecord;
    int _currentSearchRecordRemainder;
    std::map<Column, FilterParams> _filter;
    std::map<Column, QString> _searchMap;
    bool _searchState;

    // Данные от сервера
    int _pageCount;  // Количество страниц
    int _searchRecordCount;
    std::map<int, clib::TableModel> _savedPages;  // Скачанные страницы
    std::vector<int> _searchResult; // Страницы на которых есть записи соответствующие поиску




    bool findInCache(const int pageN);
    std::pair<int, int> recordNumberToPage(const int recordNumber);

    // Обработчики ответов сервера
    void pageResponse(const std::unique_ptr<Response>& response);
    void pageCountResponse(const std::unique_ptr<Response>& response);
    void uniqueValuesResponse(const std::unique_ptr<Response>& response);
    void searchResponse(const std::unique_ptr<Response>& response);

    void updateUI();
public slots:
    void connectedToServer();
    void processResponse(const QJsonObject& json);
signals:
    void sendToServer(const QByteArray& message);

    void updatePage(const clib::TableModel& model);
    void updatePageCounter(const QString &pageNumber, const QString& pageCount);
    void updateActiveFilter(const std::map<Column, FilterParams>& filters);
    void setEnabledButtons(const bool flag);
    void setDropListValue(const std::pair<Column, QStringList>& dlValue);
    void searchSwitch(const bool flag);
    void updateSearchCounter(const QString& searchNumber, const QString& searchCount);
    void highlightCard(const int cardNumber);
    void offHighLight();
};

