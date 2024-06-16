#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>

namespace Ui {
class FindDialog;
}

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);
    ~FindDialog();

private slots:
    void find();

    void on_cancelbutton_clicked();

private:
    Ui::FindDialog *ui;
    QPlainTextEdit *findtext;
    QLabel *findlabel;
    QLabel *directionlabel;
    QCheckBox *highlightbox;
    QCheckBox *casebox;
    QCheckBox *wholebox;
    QPushButton *upbutton;
    QPushButton *downbutton;
};

#endif // FINDDIALOG_H
