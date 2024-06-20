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

public slots:
    void find(bool changingText);

protected:
    void showEvent(QShowEvent *event) override;

signals:
    void findRequested(QString &findText, bool highlight, bool caseMatch, bool wholeMatch, QString direction, bool changingText);
    void replaceRequested(bool all, QString &replaceText);

private slots:
    void on_cancelbutton_clicked();
    void on_replaceButton_clicked();
    void on_replaceAllButton_clicked();
    void findHelp();

private:
    Ui::FindDialog *ui;
    QPlainTextEdit *findtext;
    QPlainTextEdit *replaceText;
    QLabel *findlabel;
    QLabel *replaceLabel;
    QLabel *directionlabel;
    QCheckBox *highlightbox;
    QCheckBox *casebox;
    QCheckBox *wholebox;
    QPushButton *upbutton;
    QPushButton *downbutton;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QString direction;
    QObject *lastSender;
};

#endif // FINDDIALOG_H
