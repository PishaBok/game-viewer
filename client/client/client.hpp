#pragma once

#include <QMainWindow>
#include <QToolBar>
#include <QMenuBar>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QTableView>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QThread>
#include <QTimer>
#include <QSpinBox>
#include <QFrame>

#include <client/command.hpp>
#include <client/search_label.hpp>
#include <client/game_card.hpp>
#include <client/filter_dialog.hpp>
#include <client/page_dialog.hpp>
#include <libcommon/table_model.hpp>
#include <libcommon/columns.hpp>

class Client : public QMainWindow
{
    Q_OBJECT
public:
    Client(ClientEngine* engine, Invoker* invoker, QWidget* parent = nullptr);
    ~Client();

private:
    QLineEdit* _pageInfoLabel; // Текущая страница/Количество страниц
    QLineEdit* _searchInfoLabel; // Текущий поиск/Количество строк поиска
    SearchLabel* _searchLabel; // Поля поиска
    QFrame* _searchFrame; // Переключение результатов поиска
    QFrame* _activeFilter; // Активные фильтры
    QWidget* _pageWidget; // Основная информация
    QVBoxLayout* _mainLayout;
    QMenuBar* _menuBar;
    QToolBar* _toolBar;

    QString _pageNumberStr;
    QString _pageCountStr;
    QString _searchNumberStr;
    QString _searchCountStr;
    std::map<Column, QStringList> _dropListValues;

    std::vector<Column> _searchColumns;
    std::vector<Column> _filterColumns;
    std::vector<QPushButton*> _buttons;
    std::map<int, GameCard*> _activeCards;

    // Движок
    ClientEngine* _engine;
    Invoker* _invoker;

    // Создание основных виджетов
    QLineEdit* createPageLabel() const;
    SearchLabel* createSearchWidget() const;
    QFrame* createSearchFrame();
    QFrame* createActiveFilter() const;
    QWidget* createPageWidget() const;
    QPushButton* createPushButton(const QString& title);
    QMenuBar* createMenuBar();
    QToolBar* createToolBar();

    // Вспомогательные функции
    QStringList columnTitles(const std::vector<Column>& columns) const;
    void clearLayout(QLayout* layout);

    void newCommand(std::shared_ptr<Command> command);
public slots:
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

