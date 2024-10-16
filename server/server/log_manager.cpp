#include <server/log_manager.hpp>

LogManager::LogManager(const QString &_path, QObject *parent)
    : QObject{parent}, _path{_path}
{
}


void LogManager::write(const QString& message)
{
    QFile file{_path};

    if (!file.open(QIODevice::Append | QIODevice::Text)) 
    {
        qWarning() << "Не удалось открыть файл для записи!";
        return;
    }

    QTextStream out(&file);
    out << message << "\n"; // Записываем сообщение в файл
    file.close();
}

std::string LogManager::getCurrentTime()
{
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream timeStream;
    timeStream << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");

    return timeStream.str();
}

void LogManager::serverStarted()
{
    write(QString::fromStdString(getCurrentTime()) + "  Server started");
}

void LogManager::clientConnected(const QString& clientId)
{
    write(QString::fromStdString(getCurrentTime()) + "  Client " + clientId + " has connected");
}

void LogManager::clientDisconnected(const QString& clientId)
{
    write(QString::fromStdString(getCurrentTime()) + "  Client " + clientId + " has disconnected");
}

