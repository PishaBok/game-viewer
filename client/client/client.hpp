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

#include <client/buttons.hpp>
#include <client/command.hpp>
#include <client/search_widget.hpp>
#include <client/filter_dialog.hpp>
#include <client/game_card.hpp>
#include <libcommon/table_model.hpp>
#include <libcommon/columns.hpp>

class Client : public QMainWindow
{
    Q_OBJECT
public:
    Client(PageCommand* pageCommand, FilterCommand* filterCommand, Invoker* invoker, QWidget* parent = nullptr);
    ~Client();

private:
    QLineEdit* _pageLabel; // Текущая страница/Количество страниц
    SearchWidget* _searchWidget; // Поля поиска
    QWidget* _filterLine; // Активные фильтры
    QWidget* _pageWidget; // Основная информация
    QVBoxLayout* _mainLayout;
    QMenuBar* _menuBar;
    QToolBar* _toolBar;
    
    std::vector<Column> _searchColumns;
    std::vector<Column> _filterColumns;

    // Комманды
    PageCommand* _pageCommand;
    FilterCommand* _filterCommand;
    Invoker* _invoker;
    std::map<QObject*, Button> _buttonMap;
    std::map<Button, Command*> _commandMap;

    // Создание основных виджетов
    QLineEdit* createPageLabel() const;
    SearchWidget* createSearchWidget() const;
    QWidget* createFilterLine() const;
    QWidget* createPageWidget() const;
    QMenuBar* createMenuBar();
    QToolBar* createToolBar();

    // Вспомогательные функции
    QStringList columnTitles(const std::vector<Column>& columns) const;
    std::map<Column, QString> stringMapToColumnMap(const std::map<QString, QString> stringMap) const;
    void clearLayout(QLayout* layout);

    void buttonPressed(QObject* sender);
private slots:
    void callDialog() const;

public slots:
    void updatePage(const clib::TableModel& model);
    void updatePageCounter(const QString& strForLabel);
    void setEnabledButtons(const bool flag);
};

