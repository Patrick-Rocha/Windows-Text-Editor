#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include "finddialog.h"
#include <QFont>

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

private:
    FindDialog *finddialog;
    QVBoxLayout *mainlayout;
    QLabel *namelabel;
    QTextEdit *maintext;
    Ui::MainWindow *ui;
    QString fileName;
    QFont font;
};
#endif // MAINWINDOW_H
