#include <QApplication>
#include <QFontDatabase>
#include <QFile>
#include <QDebug>
#include "spotlightwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Robust font path check
    QString fontPath = "fonts/Poppins-Regular.ttf";
    if (!QFile::exists(fontPath))
        fontPath = "../fonts/Poppins-Regular.ttf";
    int fontId = QFontDatabase::addApplicationFont(fontPath);
    QStringList loadedFamilies = QFontDatabase::applicationFontFamilies(fontId);
    qDebug() << "Loaded font families:" << loadedFamilies;
    if (!loadedFamilies.isEmpty()) {
        QString family = loadedFamilies.at(0);
        QFont poppins(family);
        poppins.setStyleStrategy(QFont::PreferAntialias);
        a.setFont(poppins);
        // Force font for all widgets via stylesheet
        a.setStyleSheet(QString("* { font-family: '%1'; }").arg(family));
    }

    SpotlightWindow window;
    window.show();
    return a.exec();
}