#include "connections.h"

void Connections::add(ServerThread *d)
{
    m_mutex.lock();
    m_list.push_back(d);
    m_mutex.unlock();
}

void Connections::remove(ServerThread *d)
{
    m_mutex.lock();
    auto it = std::find(m_list.begin(), m_list.end(), d);
    if(it != std::end(m_list))
        m_list.erase(it);

    m_mutex.unlock();
}

void Connections::appendMessage(const Message &message)
{
    m_mutex.lock();

    for(const auto &v: m_list)
        v->addMessage(message);

    m_mutex.unlock();
}
