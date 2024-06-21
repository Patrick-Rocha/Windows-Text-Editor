#include "exitdialog.h"
#include "ui_exitdialog.h"
#include <QPushButton>
#include <QDebug>

ExitDialog::ExitDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ExitDialog)
{
    ui->setupUi(this);

    saveButton = findChild<QPushButton*>("saveButton");
    dontSaveButton = findChild<QPushButton*>("dontSaveButton");
    cancelButton = findChild<QPushButton*>("cancelButton");
    saveLabel = findChild<QLabel*>("saveLabel");

    // Sets the cancel button to exit this window
    connect(ui->cancelButton, &QPushButton::clicked, this, &ExitDialog::cancelNotify);
    connect(cancelButton, &QPushButton::clicked, this, &ExitDialog::close);
    connect(this, &ExitDialog::exitConfirm, this, &ExitDialog::close);

    setWindowTitle(tr("Save File?"));
}

ExitDialog::~ExitDialog()
{
    delete ui;
}

void ExitDialog::on_saveButton_clicked() {
    emit saveRequested();
}

void ExitDialog::on_dontSaveButton_clicked() {
    emit dontSaveRequested();
}

void ExitDialog::leave() {
    emit exitConfirm();
}
