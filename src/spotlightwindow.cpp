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
#include <QRandomGenerator>
#include <QTimer>
#include <QDateTime>

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
    QLabel *appsHeader = new QLabel("APPLICATIONS", this);
    appsHeader->setStyleSheet(
        "QLabel { color: #b0b3b8; font-size: 10px; font-weight: bold; letter-spacing: 0.15em; margin: 0 0 2px 2px; }"
    );
    layout->addWidget(appsHeader);
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
    for (int i = 0; i < appList->count(); ++i) {
        if (!appList->item(i)->isHidden()) {
            appList->setCurrentRow(i);
            break;
        }
    }
    appList->setFocus();
    connect(searchBar, &QLineEdit::textChanged, this, &SpotlightWindow::filterResults);
    searchBar->installEventFilter(this);
    searchBar->setFocus();

    // --- Floating Bottom Bar with Clock ---
    bottomBar = new QWidget(this);
    bottomBar->setFixedHeight(44);
    bottomBar->setStyleSheet(
        "background: rgba(40, 40, 40, 0.82);"
        "border-radius: 12px;"
        "margin-left: 12px; margin-right: 12px;"
        "box-shadow: 0 2px 12px 0 rgba(0,0,0,0.10);"
    );
    QHBoxLayout *barLayout = new QHBoxLayout(bottomBar);
    barLayout->setContentsMargins(16, 0, 16, 0);
    barLayout->addStretch(); // Push content to right
    clockLabel = new QLabel(bottomBar);
    clockLabel->setStyleSheet(
        "color: #e0e0e0; font-size: 15px; font-family: 'Menlo', 'Consolas', monospace; letter-spacing: 0.08em;"
    );
    barLayout->addWidget(clockLabel);
    layout->setStretchFactor(appList, 1); // Let appList expand
    layout->addWidget(bottomBar);

    clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, &SpotlightWindow::updateClock);
    clockTimer->start(1000);
    updateClock();
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
    for (int i = 0; i < appList->count(); ++i) {
        if (!appList->item(i)->isHidden()) {
            appList->setCurrentRow(i);
            break;
        }
    }
}

// Removed applyBlurEffect; using paintEvent for frosted glass effect.

void SpotlightWindow::updateClock() {
    QDateTime now = QDateTime::currentDateTime();
    QString day = now.toString("ddd").toUpper();
    QString date = now.toString("dd MMM yyyy").toUpper();
    QString time = now.toString("HH:mm:ss");
    clockLabel->setText(QString("%1 %2 %3").arg(day).arg(date).arg(time));
}


#include <QEvent>
#include <QKeyEvent>
bool SpotlightWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == searchBar && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Down || keyEvent->key() == Qt::Key_Up) {
            this->keyPressEvent(keyEvent);
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}


#include <QProcess>

void SpotlightWindow::keyPressEvent(QKeyEvent *event) {
    // If searchBar has focus and Down pressed, move focus to appList and select first visible item
    if (event->key() == Qt::Key_Down && searchBar->hasFocus()) {
        for (int i = 0; i < appList->count(); ++i) {
            if (!appList->item(i)->isHidden()) {
                appList->setCurrentRow(i);
                appList->setFocus();
                break;
            }
        }
        return;
    }
    // If appList has focus
    if (appList->hasFocus()) {
        int count = appList->count();
        // Find all visible indices
        QVector<int> visibleIndices;
        for (int i = 0; i < count; ++i) {
            if (!appList->item(i)->isHidden()) visibleIndices.append(i);
        }
        int current = appList->currentRow();
        int pos = visibleIndices.indexOf(current);
        // Up pressed
        if (event->key() == Qt::Key_Up) {
            if (pos == 0 && !visibleIndices.isEmpty()) {
                // At first visible, move focus to searchBar
                searchBar->setFocus();
                return;
            } else if (pos > 0) {
                appList->setCurrentRow(visibleIndices[pos - 1]);
                return;
            }
        }
        // Down pressed
        if (event->key() == Qt::Key_Down) {
            if (pos >= 0 && pos < visibleIndices.size() - 1) {
                appList->setCurrentRow(visibleIndices[pos + 1]);
                return;
            }
        }
    }
    if (event->key() == Qt::Key_Escape) {
        qApp->quit();
        return;
    }
    if (event->key() == Qt::Key_Down) {
        int count = appList->count();
        if (count == 0) return;
        int current = appList->currentRow();
        if (current < 0) {
            for (int i = 0; i < count; ++i) {
                if (!appList->item(i)->isHidden()) {
                    appList->setCurrentRow(i);
                    return;
                }
            }
        }
        int next = current + 1;
        while (next < count && appList->item(next)->isHidden()) ++next;
        if (next < count) appList->setCurrentRow(next);
        return;
    }
    if (event->key() == Qt::Key_Up) {
        int count = appList->count();
        if (count == 0) return;
        int current = appList->currentRow();
        if (current < 0) {
            for (int i = 0; i < count; ++i) {
                if (!appList->item(i)->isHidden()) {
                    appList->setCurrentRow(i);
                    return;
                }
            }
        }
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
    QWidget::paintEvent(event);
}
