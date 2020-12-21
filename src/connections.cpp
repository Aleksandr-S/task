#include "connections.h"

void Connections::add(ServerThread *d)
{
    m_mutex.lock();
    m_list.push_front(d);
    m_mutex.unlock();
}

void Connections::remove(ServerThread *d)
{
    m_mutex.lock();
    m_list.remove_if([&d] (ServerThread *l) { return l == d; });
    m_mutex.unlock();
}

void Connections::appendMessage(const Message &message)
{
    m_mutex.lock();

    for(const auto &v: m_list)
        v->addMessage(message);

    m_mutex.unlock();
}
