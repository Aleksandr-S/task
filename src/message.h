#pragma once

#include "person.h"

#include <QByteArray>
#include <QDataStream>

class Message
{
public:
    enum class Type {
        Unknown,
        Add,
        Delete,
        Export
    };

    Message() {}
    Message(Type type, const Person &p)
        : m_type(type)
        , m_person(p)
    {}

    void setType(Type t) { m_type = t; }

    Type type() const { return m_type; }

    QString typeString() const {
        switch(m_type) {
            case Type::Add:     return "Add";
            case Type::Delete:  return "Delete";
            case Type::Export:  return "Export";
            default: break;
        }

        return "Unknown";
    }

    void setPerson(const Person &p) { m_person = p; }

    Person person() const { return m_person; }

private:
    Type m_type = Type::Unknown;
    Person m_person;

public:
    QByteArray data() const {
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_10);
        out << *this;

        return ba;
    }

    void setData(const QByteArray &data) {
        QDataStream in(data);
        in.setVersion(QDataStream::Qt_5_10);
        in >> *this;
    }

    friend QDataStream& operator<<(QDataStream &d, const Message &t) {
        d << static_cast<int>(t.m_type) << t.m_person.toJson();

        return d;
    }

    friend QDataStream& operator>>(QDataStream &d, Message &t) {
        int type {};
        d >> type;
        t.m_type = static_cast<Message::Type>(type);

        QByteArray jData;
        d >> jData;
        Person person;
        person.fromJson(jData);
        t.m_person = person;

        return d;
    }

    friend QDebug operator<<(QDebug dbg, const Message &t) {
        QDebugStateSaver saver(dbg);
        Q_UNUSED(saver)
        dbg.nospace() << "Message(" << t.typeString()
                      << ", "       << t.m_person
                      << ")";
        return dbg;
    }
};
