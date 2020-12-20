#pragma once

#include <memory>

#include "database.h"

#include <QTcpServer>

class Server: public QTcpServer
{
    Q_OBJECT

public:
    Server(QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDesc) override;

private:
    std::shared_ptr<DataBase> m_db;
};
