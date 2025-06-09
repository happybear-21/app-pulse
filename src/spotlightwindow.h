#ifndef SPOTLIGHTWINDOW_H
#define SPOTLIGHTWINDOW_H

#include <QWidget>
#include <QLabel>

class QLineEdit;
class QListWidget;
class QLabel;
class QWidget;

class SpotlightWindow : public QWidget {
    Q_OBJECT
public:
    explicit SpotlightWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QLineEdit *searchBar;
    QListWidget *appList;
    QStringList allAppNames;
    QList<QIcon> allAppIcons;
    QStringList allAppPaths; // Stores full paths to .app bundles
    QWidget *bottomBar;
    QLabel *footerHintsLabel;
    QLabel *clockLabel;
    QTimer *clockTimer;
    void setupUI();
    void loadApplications();
    void filterResults(const QString &text);
    void updateClock();
protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // SPOTLIGHTWINDOW_H
