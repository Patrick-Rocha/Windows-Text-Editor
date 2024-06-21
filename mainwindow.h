#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include "finddialog.h"
#include "exitdialog.h"
#include "preferencesdialog.h"
#include <QEventLoop>
#include <QCloseEvent>
#include <QFile>

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
    void highlightRequest(bool changingText);
    void directoryRequest();
    void notifyFont(QFont currentFont);

public slots:
    void highlight(QString &findtext, bool highlight, bool caseMatch, bool wholeMatch, QString direction, bool changingText);
    void saveLeave();
    void leave();
    void replace(bool all, QString &replacetext);
    void changeColours(int red, int green, int blue, int index);
    void changeDirectory(bool current, bool remember, QString directory);
    void changeFontFamily(QString fontChange);
    void changeFontWeight(QString weight);
    void changeFontStyle(QString style);
    void changeFontSpacing(int spacing);
    void handleCancelNotify();

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
    void on_actionPreferences_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionNew_triggered();

private:
    FindDialog *findDialog;
    ExitDialog *exitDialog;
    PreferencesDialog *preferencesDialog;
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
    QColor currentHighlightColour;
    QColor otherHighlightColour;
    QString chosenDirectory;
    QString fileDirectory;
    QStringList fontFamilies;
    int fontSize;
    bool makingNew;
};
#endif // MAINWINDOW_H
