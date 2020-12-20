#include "server.h"
#include "serverthread.h"
#include "persons.h"

Server::Server(QObject *parent)
    : QTcpServer(parent)
    , m_db(std::make_shared<DataBase>())
{
    m_db->start();
    connect(m_db.get(), &DataBase::persons, [&] (const std::vector<QVariant> &list) {
        int max_id = 0;

        for (const auto &v: list) {
            Person p = v.value<Person>();
            Persons::instance().add(p);

            if (p.id > max_id)
                max_id = p.id;
        }

        Persons::instance().setBeginId(max_id);
    });

    m_db->getAll();
}

void Server::incomingConnection(qintptr socketDesc)
{
    ServerThread *thread = new ServerThread(socketDesc, m_db, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}
