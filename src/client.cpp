#include "client.h"
#include "message.h"

Client::Client(QObject *parent)
    : QObject(parent)
    , m_socket(std::make_unique<QTcpSocket>())
{
    connect(m_socket.get(), &QIODevice::readyRead, this, &Client::read, Qt::DirectConnection);
    connect(m_socket.get(), QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &Client::displayError);
    connect(m_socket.get(), &QTcpSocket::connected, [&] () { emit connected(true); });
    connect(m_socket.get(), &QTcpSocket::disconnected, [&] () { emit connected(false); });
}

void Client::connectToHost(const QHostAddress &address, quint16 port)
{
    m_socket->connectToHost(address, port);
}

void Client::add(const QString &text)
{
    Person person;
    person.fromJson(text.toLocal8Bit());
    Message message(Message::Type::Add, person);
    m_socket->write(message.data());
}

void Client::remove(const QString &text)
{
    Person person;
    person.fromJson(text.toLocal8Bit());
    Message message(Message::Type::Delete, person);
    m_socket->write(message.data());
}

void Client::exportFile()
{
    Message message(Message::Type::Export, Person());
    m_socket->write(message.data());
}

void Client::read()
{
    Message message;
    QByteArray data = m_socket->readAll();
    int index = data.indexOf("|");

    while (index != -1) {
        message.setData(data.left(index));
        data.remove(0, index + 1);
        index = data.indexOf("|");

        qDebug() << "Client Get" << message;

        switch (message.type()) {
            case Message::Type::Add:
                emit addPerson(message.person().toJson());
                break;
            case Message::Type::Delete:
                emit removePerson(message.person().toJson());
                break;
            default:
                break;
        }
    }
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Error:" << socketError;
}
