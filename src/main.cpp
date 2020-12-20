#include <memory>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QHostAddress>
#include <QQmlContext>
#include <QTime>

#include "server.h"
#include "client.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    quint16 port = 55555;
    std::unique_ptr<Server> server = std::make_unique<Server>();

    if (server->listen(QHostAddress::LocalHost, port))
        qInfo() << "Server started on port" << port;
    else
        delete server.release();

    std::unique_ptr<Client> client = std::make_unique<Client>();
    client->connectToHost(QHostAddress::LocalHost, port);

    QQuickStyle::setStyle("Material");

    QQmlApplicationEngine engine;
    engine.addImportPath(":/qml");
    engine.rootContext()->setContextProperty("client", client.get());
    engine.rootContext()->setContextProperty("isServer", QVariant::fromValue(false));
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);

    }, Qt::QueuedConnection);
    engine.load(url);

    if (server.get())
        engine.rootContext()->setContextProperty("isServer", QVariant::fromValue(true));

    return app.exec();
}
