#pragma once

#include <vector>

#include <QMutex>

#include "person.h"

class Persons
{
private:
    Persons() {}
    virtual ~Persons() {}

    Persons(Persons const&) = delete;
    Persons& operator=(Person const&) = delete;

public:
    static Persons& instance() {
        static Persons p;
        return p;
    }

    static void setBeginId(qint32 id) { m_id = id; }
    static qint32 generateId() { return ++m_id; }

    Person add(const Person &d);
    void remove(const Person &d);
    void toFile();

    const std::vector<Person> &persons() const;

private:
    static qint32 m_id;

    std::vector<Person> m_list;
    QMutex m_mutex;
};
