#include "person.h"

Person::Person(QObject *parent)
    : QObject(parent)
{
}

Person::Person(const QString &name,
               quint64 telephone,
               Person::Country country,
               qint32 id,
               QObject *parent)
    : QObject(parent)
    , id(id)
    , name(name)
    , telephone(telephone)
    , country(country)
{
}

Person::Person(const Person &p, QObject *parent)
    : QObject(parent)
    , id(p.id)
    , name(p.name)
    , telephone(p.telephone)
    , country(p.country)
{
}

QByteArray Person::toJson(QJsonDocument::JsonFormat format) const
{
    QJsonObject json;
    json["_id"] = id;
    json["name"] = name;
    json["telephone"] = QString("%1").arg(telephone);
    json["country"] = static_cast<int>(country);

    QJsonDocument doc(json);

    return doc.toJson(format);
}

bool Person::fromJson(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull())
        return false;

    QJsonObject obj = doc.object();
    id = obj.value("_id").toInt();
    name = obj.value("name").toString();
    telephone = obj.value("telephone").toString().toULongLong();
    country = static_cast<Person::Country>(obj.value("country").toInt());

    return true;
}

QString Person::countryString() const
{
    switch (country) {
        case Country::Russia:   return "Russia";
        case Country::Usa:      return "USA";
        case Country::Ukraine:  return "Ukraine";
        default:
            break;
    }

    return {};
}

Person& Person::operator=(const Person &p)
{
    id = p.id;
    name = p.name;
    telephone = p.telephone;
    country = p.country;

    return *this;
}

bool Person::operator==(const Person &d)
{
    return !(*this != d);
}

bool operator!=(const Person &d1, const Person &d2) {
    return (d1.id != d2.id)
            || (d1.name != d2.name)
            || (d1.telephone != d2.telephone)
            || (d1.country != d2.country);
}

QDebug operator<<(QDebug dbg, const Person &p) {
    QDebugStateSaver saver(dbg);
    Q_UNUSED(saver)
    dbg.nospace() << "Person("  << p.id
                  << ", "       << p.name
                  << ", "       << p.telephone
                  << ", "       << p.countryString()
                  << ")";

    return dbg;
}
