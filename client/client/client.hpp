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

#include <client/buttons.hpp>
#include <client/search_widget.hpp>
#include <libcommon/table_model.hpp>

struct CardData
{
    QString name;
    QString platform;
    int year;
    QString genre;
    int criticScore;
    QString rating;
};

class Client : public QMainWindow
{
    Q_OBJECT
public:
    Client(QWidget* parent = nullptr);
    ~Client();

private:
    QLineEdit* _pageLabel;
    SearchWidget* _searchWidget;
    SearchWidget* _filterWidget;
    QWidget* _pageWidget;
    QMenuBar* _menuBar;
    QToolBar* _toolBar;


    std::map<QObject*, Button> _buttonsMap;

    QStringList _filterColumns;

    QLineEdit* createPageLabel() const;
    SearchWidget* createSearchWidget() const;
    QWidget* createPageWidget() const;
    QMenuBar* createMenuBar() const;
    QToolBar* createToolBar();
    QWidget* createGameCard(const CardData& cardData) const;

    QDialog* filterDialog() const;

    void clearLayout(QLayout* layout);

private slots:
    void buttonPressed();

public slots:
    void updatePage(const clib::TableModel& model);

signals:
    void processButton(const Button button);
};

