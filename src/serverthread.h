#pragma once

#include <vector>
#include <memory>

#include "message.h"
#include "database.h"

#include <QThread>
#include <QTcpSocket>
#include <QMutex>

class ServerThread: public QThread
{   
    Q_OBJECT

public:
    ServerThread(int socketDesc, std::shared_ptr<DataBase> db, QObject *parent);
    ~ServerThread();

    void run() override;
    void addMessage(const Message &message);

private slots:
    void read();

private:
    int m_socketDesc;
    bool m_running = true;
   std::unique_ptr<QTcpSocket> m_socket;

    std::vector<Message> m_messages;
    QMutex m_mutex;

    std::shared_ptr<DataBase> m_db;

signals:
    void error(QTcpSocket::SocketError socketError);
};
