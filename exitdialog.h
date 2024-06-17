#ifndef EXITDIALOG_H
#define EXITDIALOG_H

#include <QDialog>
#include <QLabel>

namespace Ui {
class ExitDialog;
}

class ExitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExitDialog(QWidget *parent = nullptr);
    ~ExitDialog();

signals:
    void saveRequested();
    void dontSaveRequested();
    void exitConfirm();

public slots:
    void leave();

private slots:
    void on_saveButton_clicked();
    void on_dontSaveButton_clicked();

private:
    Ui::ExitDialog *ui;
    QPushButton *saveButton;
    QPushButton *dontSaveButton;
    QPushButton *cancelButton;
    QLabel *saveLabel;
    QLine *horizontalLine;
};

#endif // EXITDIALOG_H
