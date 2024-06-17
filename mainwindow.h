#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include "finddialog.h"
#include "exitdialog.h"
#include <QEventLoop>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void leaveRequested();
    void readyToClose();
    void highlightRequest();

public slots:
    void highlight(QString &findtext, bool highlight, bool caseMatch, bool wholeMatch, QString direction);
    void saveLeave();
    void leave();
    void replace(bool all, QString &replacetext);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void saveFile();
    void saveAsFile();
    void openFile();
    void updateLineCount();
    void updateWindowTitle();
    void find();
    void zoomIn();
    void zoomOut();
    void defaultZoom();
    void changeFont(int size);
    void clearHighlights();

private:
    FindDialog *findDialog;
    ExitDialog *exitDialog;
    QVBoxLayout *mainlayout;
    QLabel *namelabel;
    QTextEdit *maintext;
    Ui::MainWindow *ui;
    QString fileName;
    QFont font;
    QEventLoop *closeEventLoop;
    int highlightCount;
    int totalHighlight;
    bool fileSaved;
    bool matchCase;
    bool matchWhole;
    QList<QTextEdit::ExtraSelection> extraSelections;
};
#endif // MAINWINDOW_H
