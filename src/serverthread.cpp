#include <vector>

#include "serverthread.h"
#include "connections.h"
#include "persons.h"

#include <QDataStream>

ServerThread::ServerThread(int socketDesc, std::shared_ptr<DataBase> db, QObject *parent)
    : QThread(parent)
    , m_socketDesc(socketDesc)
    , m_db(db)
{
    Connections::instance().add(this);
}

ServerThread::~ServerThread()
{
    Connections::instance().remove(this);
    m_running = false;
}

void ServerThread::run()
{
    while (!m_db->isStarted())
        msleep(50);

    m_socket = std::make_unique<QTcpSocket>();
    if (!m_socket->setSocketDescriptor(m_socketDesc)) {
        emit error(m_socket->error());
        return;
    }

    const std::vector<Person> &list = Persons::instance().persons();
    for (const auto &v: list) {
        QByteArray data("|");
        data.prepend(Message(Message::Type::Add, v).data());
        m_socket->write(data);
    }

    if (m_socket->state() != QAbstractSocket::UnconnectedState)
        m_socket->waitForBytesWritten(500);

    while (m_running && m_socket->state() != QAbstractSocket::UnconnectedState) {
        if (m_socket->waitForReadyRead(100))
            read();

        m_mutex.lock();
        for (const auto &v : m_messages) {
            QByteArray data("|");
            data.prepend(v.data());
            m_socket->write(data);
        }
        m_socket->waitForBytesWritten(500);
        m_messages.clear();
        m_mutex.unlock();
    }
}

void ServerThread::addMessage(const Message &message)
{
    m_mutex.lock();
    m_messages.push_back(message);
    m_mutex.unlock();
}

void ServerThread::read()
{
    Message message;
    message.setData(m_socket->readAll());
    qDebug() << "Server Get" << message;

    Persons& persons = Persons::instance();
    switch (message.type()) {
        case Message::Type::Add:
            message.setPerson(persons.add(message.person()));
            m_db->add(message.person());
            break;
        case Message::Type::Delete:
            persons.remove(message.person());
            m_db->remove(message.person());
            break;
        case Message::Type::Export:
            persons.toFile();
            return;
        default:
            break;
    }

    Connections::instance().appendMessage(message);
}
