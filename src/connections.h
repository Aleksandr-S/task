#pragma once

#include <forward_list>

#include "serverthread.h"

#include <QMutex>

class Connections
{
private:
    Connections() {}
    virtual ~Connections() {}

    Connections(Connections const&) = delete;
    Connections& operator=(Person const&) = delete;

public:
    static Connections& instance() {
        static Connections c;
        return c;
    }

    void add(ServerThread *d);
    void remove(ServerThread *d);
    void appendMessage(const Message &message);

private:
    std::forward_list<ServerThread *> m_list;
    QMutex m_mutex;
};
