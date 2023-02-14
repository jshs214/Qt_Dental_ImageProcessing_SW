
#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "xraydb.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "TeamUnsigned_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    /* DB 프로그램 동작 */
    XrayDB xrayDB;
    xrayDB.show();

    return a.exec();
}
