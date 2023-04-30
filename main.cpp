#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "contactsmodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/AndroidContactsApp/Main.qml"_qs);
    qmlRegisterType<ContactsModel>("Contacts", 1, 0, "ContactsModel");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
