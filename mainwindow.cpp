#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QVBoxLayout>
#include <QShortcut>
#include <QKeySequence>

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

    // Set the initial title of the window
    setWindowTitle(tr("No File Opened - Patrick's Text Editor"));

    // Moving the name label
    namelabel->setGeometry(0, this->height() - namelabel->height(), this->width(), namelabel->height());

    // Connecting the buttons to their respective functions
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::saveAsFile);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Open button action
void MainWindow::openFile() {
    qDebug() << "open";

    // Open file dialog to select a file
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;All Files (*)"));

    // Check if a file was selected
    if (fileName.isEmpty()) {
        qDebug() << "No file selected";
        return;
    }

    QFile file(fileName);

    // Try to open the file in read-only mode
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::information(this, tr("Unable to open file"), file.errorString());
        return;
    }

    // Read the file contents
    QTextStream in(&file);
    QString fileContents = in.readAll();

    // Set the text in the maintext QTextEdit
    maintext->setPlainText(fileContents);
    setWindowTitle(tr("%1 - Patrick's Text Editor").arg(QFileInfo(fileName).fileName()));

    // Close the file
    file.close();
}

// Save button action
void MainWindow::saveFile() {
    qDebug() << "Save button wowwerwer!";
    if (fileName.isEmpty()) {
        qDebug() << "empty";
        saveAsFile();
    } else {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("unable to open fiel"), file.errorString());
            return;
        }
        QTextStream out(&file);
        out << maintext->toPlainText();
        file.close();
        qDebug() << "Saved to file";
    }
}

// Save-As button action
void MainWindow::saveAsFile() {
    qDebug() << "woof";

    // Opens a save menu and returns the path of the new file
    fileName = QFileDialog::getSaveFileName(maintext, tr("Save File As"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot save file: ") + file.errorString());
        return;
    }

    QTextStream out(&file);
    out << maintext->toPlainText();
    setWindowTitle(tr("%1 - Patrick's Text Editor").arg(QFileInfo(fileName).fileName()));
    file.close();
}
