#include <client/socket.hpp>

Socket::Socket(const QString &host, const int port, QObject *parent) 
    : QObject(parent), _expectedLength{0}, _buffer{}
{
    _tcpSocket = new QTcpSocket(this);

    connect(_tcpSocket, &QTcpSocket::readyRead, this, &Socket::slotReadyRead);

    _tcpSocket->connectToHost(host, port);



    _testRequest = getTestRequest();
}

Socket::~Socket()
{
    qDebug() << "Socket Disconnected!";
    _tcpSocket->disconnectFromHost();
    _tcpSocket->deleteLater();
}

QJsonObject Socket::getTestRequest()
{
    QJsonObject json;

    // Создаем объект params
    QJsonObject params;
    params["page"] = 1;

    // Создаем объект filter
    QJsonObject filter;

    // Создаем вложенный объект year
    QJsonObject year;
    year["type"] = "int";
    year["value"] = 2011;

    // Создаем вложенный объект platform
    QJsonObject platform;
    platform["type"] = "string";
    platform["value"] = "PC";

    // Добавляем year и platform в filter
    filter["year"] = year;
    filter["platform"] = platform;

    // Добавляем filter в params
    params["filter"] = filter;

    // Добавляем пустой объект search в params
    params["search"] = QJsonObject();

    // Добавляем params в основной json-объект
    json["reqType"] = 1;
    json["params"] = params;

    return json;
}

void Socket::slotReadyRead()
{
    qDebug() << "Socket received a message";

    while (_tcpSocket->canReadLine()) 
    {
        QByteArray line = _tcpSocket->readLine();
        if (_expectedLength == 0) 
        {
            // Получаем длину сообщения
            _expectedLength = line.toLongLong();
        } 
        else 
        {
            // Читаем данные, если длина уже известна
            _buffer.append(line);
            if (_buffer.size() >= _expectedLength) 
            {
                QJsonObject jsonObj{QJsonDocument::fromJson(_buffer).object()};
                _model.loadJson(jsonObj["model"].toObject());
                emit updateView(qobject_cast<QAbstractTableModel*>(&_model));
                _buffer.clear();
                _expectedLength = 0; // Сбрасываем
                break;
            }
        }
    }
}


void Socket::slotSendToServer()
{
    // QFile file("/Users/pavel/Projects/tcpserver/client/test_request.json");
    // file.open(QIODevice::ReadOnly);

    // QByteArray data(file.readAll());

    QJsonDocument doc(_testRequest);
    _tcpSocket->write(doc.toJson());

   // file.close();
}
