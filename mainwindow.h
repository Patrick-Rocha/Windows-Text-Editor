#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>

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
    void saveShortcut();
    void openFile();

private:
    QVBoxLayout *mainlayout;
    QLabel *namelabel;
    QTextEdit *maintext;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
