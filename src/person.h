#pragma once

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

class Person: public QObject
{
    Q_OBJECT

public:
    enum class Country {
        Russia  = 8,
        Usa     = 1,
        Ukraine = 38
    };

    explicit Person(QObject *parent = Q_NULLPTR);
    Person(const QString &name,
           quint64 telephone,
           Person::Country country,
           qint32 id = 0,
           QObject *parent = Q_NULLPTR);
    Person(const Person &p, QObject *parent = Q_NULLPTR);

    qint32 id = 0;
    QString name = "Unknown";
    quint64 telephone = 0;
    Country country = Country::Russia;

public:
    QByteArray toJson(QJsonDocument::JsonFormat format = QJsonDocument::Compact) const;
    bool fromJson(const QByteArray &data);
    QString countryString() const;

    Person& operator=(const Person &p);
    bool operator==(const Person &d);
    friend bool operator!=(const Person &d1, const Person &d2);
    friend QDebug operator<<(QDebug dbg, const Person &p);
};

Q_DECLARE_METATYPE(Person)
