#ifndef SPOTLIGHTWINDOW_H
#define SPOTLIGHTWINDOW_H

#include <QWidget>

class QLineEdit;
class QListWidget;

class SpotlightWindow : public QWidget {
    Q_OBJECT
public:
    explicit SpotlightWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QLineEdit *searchBar;
    QListWidget *resultsList;
    QStringList allResults;
    void setupUI();
    void filterResults(const QString &text);
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // SPOTLIGHTWINDOW_H
