#include "database.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDir>

#include <QDebug>

DataBase::DataBase(QObject *parent)
    : QThread(parent)
    , m_db(std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE", "stc")))
{
}

DataBase::~DataBase()
{
    m_stopping = true;
    quit();
    wait();
}

void DataBase::getAll()
{
    mutexReqDb.lock();
    Request req { RequestDb::GET_ALL, QVariant()};
    m_queue.push(std::move(req));
    mutexReqDb.unlock();
}

void DataBase::add(const Person &p)
{
    mutexReqDb.lock();
    Request req { RequestDb::ADD, QVariant::fromValue(p)};
    m_queue.push(std::move(req));
    mutexReqDb.unlock();
}

void DataBase::update(const Person &p)
{
    mutexReqDb.lock();
    Request req { RequestDb::UPDATE, QVariant::fromValue(p)};
    m_queue.push(std::move(req));
    mutexReqDb.unlock();
}

void DataBase::remove(const Person &p)
{
    mutexReqDb.lock();
    Request req { RequestDb::REMOVE, QVariant::fromValue(p)};
    m_queue.push(std::move(req));
    mutexReqDb.unlock();
}

void DataBase::run()
{
    qInfo() << "Connecting to DB...";
    while (!m_stopping && !m_connected) {
        connect();

        if (!m_connected) {
            unsigned long time = 0;
            while(!m_stopping && time < 3*1000) { msleep(100); time += 100; }
            continue;
        }

        createTable();

        while (!m_stopping) {
            if (!m_db->isOpen())
                break;

            mutexReqDb.lock();
            if (m_queue.empty()) {
                mutexReqDb.unlock();
                msleep(50);
                continue;
            }

            const Request &req = m_queue.back();

            switch (req.reqDb) {
            case RequestDb::ADD:
                rqAdd(req.value.value<Person>());
                break;
            case RequestDb::UPDATE:
                rqUpdate(req.value.value<Person>());
                break;
            case RequestDb::REMOVE:
                rqRemove(req.value.value<Person>());
                break;
            case RequestDb::GET_ALL:
                rqGetAll();
                break;
            default:
                break;
            }

            m_queue.pop();
            mutexReqDb.unlock();

            msleep(50);
        }
    }

    if (m_db->isOpen()) {
        const QString &name = m_db->connectionName();
        m_db->close();
        QSqlDatabase::removeDatabase(name);
    }
}

void DataBase::connect()
{
    m_db->setDatabaseName(QDir::currentPath() + "/database.db");

    m_connected = m_db->open();

    if (!m_connected)
        qCritical() << m_db->lastError().text();
}

void DataBase::createTable()
{
    int count = 0;
    QString str = QString("SELECT count(name) FROM sqlite_master WHERE name='persons';");
    QSqlQuery queryCh(str, *m_db);

    while (queryCh.next())
        count = queryCh.value(0).toInt();

    str = ("CREATE TABLE IF NOT EXISTS persons("
           "id INT PRIMARY KEY     NOT NULL,"
           "name           TEXT    NOT NULL,"
           "telephone      TEXT    NOT NULL,"
           "country        INT     NOT NULL"
           ");");
    QSqlQuery query(str, *m_db);
    query.exec();

    if (!count) {
        rqAdd(Person("Ivanov Ivan", 89991234455, Person::Country::Russia, 1));
        rqAdd(Person("Petrov Petr", 89991112233, Person::Country::Usa, 2));
        rqAdd(Person("Sidorov Ivan", 89993331122, Person::Country::Ukraine, 3));

        qInfo() << "DataBase" << m_db->databaseName() << "is created";
    }

    m_started = true;
}

void DataBase::rqGetAll()
{
    std::vector<QVariant> list;
    QString str = QString("SELECT "
                          "id, "
                          "name, "
                          "telephone, "
                          "country "
                          "FROM persons "
                          "ORDER BY id");
    QSqlQuery query(str, *m_db);

    while (query.next()) {
        Person person;
        person.id = query.value("id").toInt();
        person.name = query.value("name").toString();
        person.telephone = query.value("telephone").toString().toULongLong();
        person.country = static_cast<Person::Country>(query.value("country").toInt());

        list.push_back(QVariant::fromValue(person));
    }

    emit persons(list);
}

void DataBase::rqAdd(const Person &p)
{
    QString str("INSERT INTO persons("
                "id, "
                "name, "
                "telephone, "
                "country) "
                "VALUES (?, ?, ?, ?);");
    QSqlQuery query(*m_db);
    if (query.prepare(str)) {
        query.addBindValue(p.id);
        query.addBindValue(p.name);
        query.addBindValue(QString("%1").arg(p.telephone));
        query.addBindValue(static_cast<int>(p.country));
        query.exec();
    }
}

void DataBase::rqUpdate(const Person &p)
{
    QString str = QString("UPDATE persons SET "
                          "name=?, "
                          "telephone=? "
                          "country=?"
                          "WHERE id = %1;").arg(p.id);
    QSqlQuery query(str, *m_db);
    if (query.prepare(str)) {
        query.addBindValue(p.name);
        query.addBindValue(QString("%1").arg(p.telephone));
        query.addBindValue(static_cast<int>(p.country));
        query.exec();
    }
}

void DataBase::rqRemove(const Person &p)
{
    QString str = QString("DELETE FROM persons "
                          "WHERE id = %1;").arg(p.id);
    QSqlQuery query(str, *m_db);
    query.exec();
}
