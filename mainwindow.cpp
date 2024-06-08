#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QVBoxLayout>
#include <QShortcut>
#include <QKeySequence>

bool openedFile = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Creating the widgets
    mainlayout = findChild<QVBoxLayout*>("mainlayout");
    maintext = findChild<QTextEdit*>("maintext");
    namelabel = findChild<QLabel*>("namelabel");

    // Stylesheets
    maintext->setStyleSheet("QTextEdit { margin: 0px; padding: 0px; border: none; border-top: 1px solid gray; }");
    namelabel->setStyleSheet("QLabel#namelabel { padding-top: 10px; padding-right: 10px; }");

    // Set margins to 0
    mainlayout->setContentsMargins(0, 0, 0, 0);
    mainlayout->setSpacing(0);

    // Setting the layout to the central widget
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainlayout);
    setCentralWidget(centralWidget);

    // Moving the name label
    namelabel->setGeometry(0, this->height() - namelabel->height(), this->width(), namelabel->height());

    // Connecting the buttons to their respective functions
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::saveAsFile);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);

    // Keyboard shortcuts
    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(shortcut, &QShortcut::activated, this, &MainWindow::saveShortcut);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Performed upon Ctrl+S
void MainWindow::saveShortcut() {
    qDebug() << "TESTESTESIOJT";
    if (openedFile) {
        saveFile();
    } else {
        saveAsFile();
    }
}

// Open button action
void MainWindow::openFile() {
    qDebug() << "open";
}

// Save button action
void MainWindow::saveFile() {
    qDebug() << "Save button wowwerwer!";
}

// Save-As button action
void MainWindow::saveAsFile() {
    qDebug() << "woof";

    // Opens a save menu and returns the path of the new file
    QString fileName = QFileDialog::getSaveFileName(maintext, tr("Save File As"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (fileName.isEmpty()) {
        openedFile = false;
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot save file: ") + file.errorString());
        openedFile = false;
        return;
    }

    QTextStream out(&file);
    out << maintext->toPlainText();
    file.close();
    openedFile = true;
}
