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

    findtext = findChild<QPlainTextEdit*>("findtext");
    findlabel = findChild<QLabel*>("findlabel");
    directionlabel = findChild<QLabel*>("directionlabel");
    highlightbox = findChild<QCheckBox*>("highlightbox");
    casebox = findChild<QCheckBox*>("casebox");
    wholebox = findChild<QCheckBox*>("wholebox");
    upbutton = findChild<QPushButton*>("upbutton");
    downbutton = findChild<QPushButton*>("downbutton");

    findtext->setStyleSheet("QPlainTextEdit {text-align: center;}");

    connect(ui->findtext, &QPlainTextEdit::textChanged, this, &FindDialog::find);
    connect(ui->highlightbox, &QCheckBox::stateChanged, this, &FindDialog::find);
    connect(ui->casebox, &QCheckBox::stateChanged, this, &FindDialog::find);
    connect(ui->wholebox, &QCheckBox::stateChanged, this, &FindDialog::find);
    connect(ui->upbutton, &QPushButton::clicked, this, &FindDialog::find);
    connect(ui->downbutton, &QPushButton::clicked, this, &FindDialog::find);
}

FindDialog::~FindDialog()
{
    delete ui;
}

void FindDialog::find() {
    qDebug("Find");
}

void FindDialog::on_cancelbutton_clicked()
{
    this->close();
}

