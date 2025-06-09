#include "spotlightwindow.h"
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QKeyEvent>
#include <QApplication>

SpotlightWindow::SpotlightWindow(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(640, 520);
    setStyleSheet("border-radius: 16px; box-shadow: 0 8px 32px 0 rgba(31, 38, 135, 0.37);");
    setupUI();
}

void SpotlightWindow::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);
    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("Search apps...");
    searchBar->setStyleSheet(
        "QLineEdit {"
        "  padding: 10px 14px;"
        "  border-radius: 8px;"
        "  background: rgba(255,255,255,0.10);"
        "  color: #fff;"
        "  font-size: 17px;"
        "  border: none;"
        "}"
    );
    layout->addWidget(searchBar);
    appList = new QListWidget(this);
    appList->setStyleSheet(
        "QListWidget { background: transparent; color: #fff; font-size: 15px; border: none; }"
        "QListWidget::item { padding: 10px 12px; border-radius: 6px; }"
        "QListWidget::item:selected { background: rgba(255,255,255,0.13); }"
        "QScrollBar:vertical {"
        "  background: transparent;"
        "  width: 6px;"
        "  margin: 2px 2px 2px 2px;"
        "  border-radius: 3px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background: rgba(255,255,255,0.18);"
        "  min-height: 20px;"
        "  border-radius: 3px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "  height: 0;"
        "  subcontrol-origin: margin;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "  background: none;"
        "}"
    );
    layout->addWidget(appList);
    loadApplications();
    connect(searchBar, &QLineEdit::textChanged, this, &SpotlightWindow::filterResults);
}

#include <QDir>
#include <QFileInfoList>
#include <QFileIconProvider>
#include <QStandardPaths>

void SpotlightWindow::loadApplications() {
    appList->clear();
    allAppNames.clear();
    allAppIcons.clear();
    QFileIconProvider iconProvider;
    QStringList appDirs = { "/Applications", QDir::homePath() + "/Applications" };
    QSet<QString> seen;
    allAppPaths.clear();
    for (const QString &dirPath : appDirs) {
        QDir dir(dirPath);
        QFileInfoList apps = dir.entryInfoList({"*.app"}, QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QFileInfo &info : apps) {
            QString appName = info.baseName();
            if (seen.contains(appName)) continue;
            seen.insert(appName);
            QIcon icon = iconProvider.icon(info);
            allAppNames.append(appName);
            allAppIcons.append(icon);
            allAppPaths.append(info.absoluteFilePath());
            QListWidgetItem *item = new QListWidgetItem(icon, appName, appList);
            appList->addItem(item);
        }
    }
}

void SpotlightWindow::filterResults(const QString &text) {
    for (int i = 0; i < appList->count(); ++i) {
        QListWidgetItem* item = appList->item(i);
        bool match = item->text().contains(text, Qt::CaseInsensitive);
        item->setHidden(!match);
    }
}

// Removed applyBlurEffect; using paintEvent for frosted glass effect.

#include <QProcess>

void SpotlightWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        qApp->quit();
        return;
    }
    if (event->key() == Qt::Key_Down) {
        int count = appList->count();
        if (count == 0) return;
        int current = appList->currentRow();
        int next = current + 1;
        while (next < count && appList->item(next)->isHidden()) ++next;
        if (next < count) appList->setCurrentRow(next);
        return;
    }
    if (event->key() == Qt::Key_Up) {
        int count = appList->count();
        if (count == 0) return;
        int current = appList->currentRow();
        int prev = current - 1;
        while (prev >= 0 && appList->item(prev)->isHidden()) --prev;
        if (prev >= 0) appList->setCurrentRow(prev);
        return;
    }
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        int row = appList->currentRow();
        if (row >= 0 && row < allAppPaths.size() && !appList->item(row)->isHidden()) {
            QProcess::startDetached("open", QStringList() << allAppPaths[row]);
        }
        return;
    }
    QWidget::keyPressEvent(event);
}

void SpotlightWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QColor bg(30, 30, 30, 180); // semi-transparent dark
    painter.setBrush(bg);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 16, 16);
    // Optionally, add noise or gradient for more realistic frosted glass
    QWidget::paintEvent(event);
}

