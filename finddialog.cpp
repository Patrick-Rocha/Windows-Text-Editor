#include "finddialog.h"
#include "ui_finddialog.h"

#include <QPlainTextEdit>
#include <QCheckBox>
#include <QLabel>
#include <QDebug>
#include <QPushButton>

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Find Text"));

    replaceButton = findChild<QPushButton*>("replaceButton");
    replaceAllButton = findChild<QPushButton*>("replaceAllButton");
    findtext = findChild<QPlainTextEdit*>("findtext");
    findlabel = findChild<QLabel*>("findlabel");
    directionlabel = findChild<QLabel*>("directionlabel");
    replaceLabel = findChild<QLabel*>("replaceLabel");
    highlightbox = findChild<QCheckBox*>("highlightbox");
    casebox = findChild<QCheckBox*>("casebox");
    wholebox = findChild<QCheckBox*>("wholebox");
    upbutton = findChild<QPushButton*>("upbutton");
    downbutton = findChild<QPushButton*>("downbutton");

    findtext->setStyleSheet("QPlainTextEdit {text-align: center;}");

    connect(ui->findtext, &QPlainTextEdit::textChanged, this, &FindDialog::findHelp);
    connect(ui->highlightbox, &QCheckBox::stateChanged, this, &FindDialog::findHelp);
    connect(ui->casebox, &QCheckBox::stateChanged, this, &FindDialog::findHelp);
    connect(ui->wholebox, &QCheckBox::stateChanged, this, &FindDialog::findHelp);
    connect(ui->upbutton, &QPushButton::clicked, this, &FindDialog::findHelp);
    connect(ui->downbutton, &QPushButton::clicked, this, &FindDialog::findHelp);
}

FindDialog::~FindDialog()
{
    delete ui;
}


void FindDialog::showEvent(QShowEvent *event)
{
    // Call the base class showEvent to handle default behavior
    QDialog::showEvent(event);

    // Call find() when the dialog is shown
    find(false);
}

void FindDialog::find(bool changingText) {
    lastSender = sender();

    if (lastSender == ui->upbutton) {
        direction = "up";
    } else if (lastSender == ui->downbutton) {
        direction = "down";
    } else {
        direction = "neutral";
    }

    QString searchText = ui->findtext->toPlainText();
    bool highlight = ui->highlightbox->isChecked();
    bool caseMatch = ui->casebox->isChecked();
    bool wholeMatch = ui->wholebox->isChecked();

    emit findRequested(searchText, highlight, caseMatch, wholeMatch, direction, changingText);
}

void FindDialog::findHelp() {
    find(false);
}

void FindDialog::on_cancelbutton_clicked()
{
    this->close();
}


void FindDialog::on_replaceButton_clicked()
{
    QString replacingText = ui->replaceText->toPlainText();
    emit replaceRequested(false, replacingText);
}


void FindDialog::on_replaceAllButton_clicked()
{
    QString replacingText = ui->replaceText->toPlainText();
    emit replaceRequested(true, replacingText);
}

