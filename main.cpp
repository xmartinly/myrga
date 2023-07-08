
#include "myrga.h"

#include <QApplication>

#include <QLocale>
#include <QTranslator>

#ifdef VLD_MODULE
    #include "vld.h"
#endif



int main(int argc, char* argv[]) {
#ifdef Q_OS_WIN32
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
    QApplication a(argc, argv);
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString& locale : uiLanguages) {
        const QString baseName = "myrga_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    QFile qss(":/Resource/Qss/custom.qss");
    qss.open(QFile::ReadOnly);
    if(qss.isOpen()) {
        a.setStyleSheet(qss.readAll());
        qss.close();
    }
    MyRga w;
    w.show();
    return a.exec();
}
