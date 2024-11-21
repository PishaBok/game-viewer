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
    Client(PageCommand* pageCommand, FilterCommand* filterCommand, Invoker* invoker, QWidget* parent = nullptr);
    ~Client();

private:
    QLineEdit* _pageLabel; // Текущая страница/Количество страниц
    SearchLabel* _searchLabel; // Поля поиска
    QFrame* _searchFrame;
    QFrame* _activeFilter; // Активные фильтры
    QWidget* _pageWidget; // Основная информация
    QVBoxLayout* _mainLayout;
    QMenuBar* _menuBar;
    QToolBar* _toolBar;

    QString _pageNumberStr;
    QString _pageCountStr;

    std::vector<Column> _searchColumns;
    std::vector<Column> _filterColumns;
    std::vector<QPushButton*> _buttons;

    // Комманды
    PageCommand* _pageCommand;
    FilterCommand* _filterCommand;
    Invoker* _invoker;
    std::map<QObject*, Command*> _senderToCommand;

    // Создание основных виджетов
    QLineEdit* createPageLabel() const;
    SearchLabel* createSearchWidget() const;
    QFrame* createSearchFrame() const;
    QFrame* createActiveFilter() const;
    QWidget* createPageWidget() const;
    QPushButton* createPushButton(const QString& title, Command* command);
    QMenuBar* createMenuBar();
    QToolBar* createToolBar();

    // Вспомогательные функции
    QStringList columnTitles(const std::vector<Column>& columns) const;
    std::map<Column, QString> stringMapToColumnMap(const std::map<QString, QString> stringMap) const;
    void clearLayout(QLayout* layout);

    void commandTriggered(QObject* sender);
public slots:
    void updatePage(const clib::TableModel& model);
    void updatePageCounter(const QString &pageNumber, const QString& pageCount);
    void updateActiveFilter(const std::map<Column, QString>& filters);
    void setEnabledButtons(const bool flag);
};

