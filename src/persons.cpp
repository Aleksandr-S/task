#include "persons.h"

#include <QDir>
#include <QFile>

qint32 Persons::m_id = 0;

Person Persons::add(const Person &d)
{
    Person person(d);

    if (!d.id)
        person.id = generateId();

    m_mutex.lock();
    m_list.push_back(person);
    m_mutex.unlock();

    return person;
}

void Persons::remove(const Person &d)
{
    m_mutex.lock();
    m_list.erase(std::remove_if(m_list.begin(), m_list.end(),
                                [&] (const Person &p) { return p.id == d.id; }));
    m_mutex.unlock();
}

void Persons::toFile()
{
    QFile file(QDir::currentPath() + "/export.txt");
    if (!file.open(QIODevice::WriteOnly))
        return;

    QTextStream out(&file);
    out << "\"Persons\": {\n";
    int i = 0;
    int end = m_list.size();
    for (; i < end; ++i)
        out << m_list.at(i).toJson(QJsonDocument::Indented)
            << (i != (end - 1) ? ",\n" : "");

    out << "}";

    qInfo() << "Export to file" << file.fileName() << "is completed";

    file.close();
}

const std::vector<Person>& Persons::persons() const
{
    return m_list;
}
