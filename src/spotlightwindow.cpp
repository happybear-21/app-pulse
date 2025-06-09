#include "spotlightwindow.h"
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QKeyEvent>
#include <QApplication>

SpotlightWindow::SpotlightWindow(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(420, 320);
    setStyleSheet("border-radius: 16px; box-shadow: 0 8px 32px 0 rgba(31, 38, 135, 0.37);");
    setupUI();
}

void SpotlightWindow::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);
    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("Search...");
    searchBar->setStyleSheet(
        "QLineEdit {"
        "  padding: 10px 16px;"
        "  border-radius: 8px;"
        "  background: rgba(255,255,255,0.12);"
        "  color: #fff;"
        "  font-size: 18px;"
        "  border: none;"
        "}"
    );
    resultsList = new QListWidget(this);
    resultsList->setStyleSheet(
        "QListWidget {"
        "  background: transparent;"
        "  color: #fff;"
        "  font-size: 16px;"
        "  border: none;"
        "}"
        "QListWidget::item {"
        "  padding: 8px 12px;"
        "  border-radius: 6px;"
        "}"
        "QListWidget::item:selected {"
        "  background: rgba(255,255,255,0.15);"
        "}"
    );
    // Dummy data
    allResults = {"Open Google", "Open VSCode", "Run AI Command", "Find File: report.pdf", "Search: Cascade AI"};
    resultsList->addItems(allResults);
    connect(searchBar, &QLineEdit::textChanged, this, &SpotlightWindow::filterResults);
    layout->addWidget(searchBar);
    layout->addWidget(resultsList);
}

void SpotlightWindow::filterResults(const QString &text) {
    resultsList->clear();
    for (const QString &item : allResults) {
        if (item.contains(text, Qt::CaseInsensitive)) {
            resultsList->addItem(item);
        }
    }
}

// Removed applyBlurEffect; using paintEvent for frosted glass effect.

void SpotlightWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        qApp->quit();
    } else {
        QWidget::keyPressEvent(event);
    }
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

