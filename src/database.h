#pragma once

#include <queue>
#include <memory>

#include "person.h"

#include <QSqlDatabase>
#include <QThread>
#include <QMutex>


class DataBase: public QThread
{
    Q_OBJECT

public:
    enum class RequestDb {
        ADD,
        UPDATE,
        REMOVE,
        GET_ALL,
    };

    struct Request {
        RequestDb reqDb;
        QVariant value;
    };

    explicit DataBase(QObject *parent = Q_NULLPTR);
    ~DataBase() override;

    void getAll();
    void add(const Person &p);
    void update(const Person &p);
    void remove(const Person &p);

    bool isStarted() { return m_started; }

private:
    void connect();

    void createTable();

    void rqGetAll();
    void rqAdd(const Person &p);
    void rqUpdate(const Person &p);
    void rqRemove(const Person &p);

protected:
    void run() override;

signals:
    void persons(const std::vector<QVariant> &);

private:
    std::unique_ptr<QSqlDatabase> m_db;
    bool m_connected = false;
    bool m_stopping = false;
    bool m_started = false;

    std::queue<Request> m_queue;
    QMutex mutexReqDb;
};
