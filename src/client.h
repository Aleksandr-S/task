#pragma once

#include <memory>

#include <QTcpSocket>
#include <QDataStream>

class Client: public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);

    void connectToHost(const QHostAddress &address, quint16 port);

    Q_INVOKABLE void add(const QString &text);
    Q_INVOKABLE void remove(const QString &text);
    Q_INVOKABLE void exportFile();

private slots:
    void read();
    void displayError(QAbstractSocket::SocketError socketError);

signals:
   void addPerson(const QString &jData);
   void removePerson(const QString &jData);
   void connected(bool state);

private:
    std::unique_ptr<QTcpSocket> m_socket;
};
