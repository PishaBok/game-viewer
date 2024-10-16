#pragma once

#include <chrono>
#include <string>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDebug>

class LogManager: public QObject
{
    Q_OBJECT
public:
    explicit LogManager(const QString& file, QObject* parent = nullptr);

private:
    QString _path;

    void write(const QString& message);
    std::string getCurrentTime();

public slots:
    void serverStarted();
    void clientConnected(const QString& clientId);
    void clientDisconnected(const QString& clientId);
};