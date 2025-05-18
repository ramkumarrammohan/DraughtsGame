#include <QQmlContext>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "Board.h"
#include "Enums.h"
#include "Piece.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterType<Piece>("com.checkerboard", 1, 0,"Piece");
    qmlRegisterSingletonType(QUrl("qrc:/Qml/Theme.qml"),
                             "com.checkerboard.Theme", 1, 0, "Theme");
    qmlRegisterUncreatableType<Enums>("com.checkerboard.Enums", 1, 0, "Enums",
                                      "Enums are read only");

    Board board;
    engine.rootContext()->setContextProperty("board", &board);

    const QUrl url(QStringLiteral("qrc:/Qml/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
