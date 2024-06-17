// TODO: Complete the "replace" feature
// Maybe make tabs like in notepad++
// Font changer?

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "exitdialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QVBoxLayout>
#include <QShortcut>
#include <QKeySequence>
#include <QTextBlock>
#include <QTextDocument>
#include <QFont>
#include <QRegularExpression>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    findDialog = new FindDialog(this);
    exitDialog = new ExitDialog(this);

    // Creating the widgets
    mainlayout = findChild<QVBoxLayout*>("mainlayout");
    maintext = findChild<QTextEdit*>("maintext");
    namelabel = findChild<QLabel*>("namelabel");

    // Stylesheets
    maintext->setStyleSheet("QTextEdit { margin: 0px; padding: 0px; border: none; border-top: 1px solid gray; }");
    namelabel->setStyleSheet("QLabel#namelabel { padding-top: 10px; padding-right: 10px; }");
    changeFont(12);

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
    connect(ui->maintext, &QTextEdit::cursorPositionChanged, this, &MainWindow::updateLineCount);
    connect(ui->maintext, &QTextEdit::textChanged, this, &MainWindow::updateWindowTitle);
    connect(ui->actionFind, &QAction::triggered, this, &MainWindow::find);
    connect(ui->actionZoomIn, &QAction::triggered, this, &MainWindow::zoomIn);
    connect(ui->actionZoomOut, &QAction::triggered, this, &MainWindow::zoomOut);
    connect(ui->actionDefaultZoom, &QAction::triggered, this, &MainWindow::defaultZoom);
    connect(this, &MainWindow::leaveRequested, exitDialog, &ExitDialog::leave);
    connect(findDialog, &FindDialog::findRequested, this, &MainWindow::highlight);
    connect(findDialog, &FindDialog::replaceRequested, this, &MainWindow::replace);
    connect(exitDialog, &ExitDialog::dontSaveRequested, this, &MainWindow::leave);
    connect(exitDialog, &ExitDialog::saveRequested, this, &MainWindow::saveLeave);
    connect(ui->actionUndo, &QAction::triggered, maintext, &QTextEdit::undo);
    connect(ui->actionRedo, &QAction::triggered, maintext, &QTextEdit::redo);
    connect(exitDialog, &ExitDialog::exitConfirm, this, &MainWindow::leave);
    connect(this, &MainWindow::readyToClose, this, &MainWindow::close);
    connect(this, &MainWindow::highlightRequest, findDialog, &FindDialog::find);
    connect(findDialog, &FindDialog::finished, this, &MainWindow::clearHighlights);

    // Setting initial values for global variables
    highlightCount = 1;
    totalHighlight = 0;
    fileSaved = true;
    matchCase = false;
    matchWhole = false;
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

void MainWindow::changeFont(int size) {
    font = maintext->font();
    font.setPointSize(size);
    maintext->setFont(font);
}

void MainWindow::zoomIn() {
    int fontSize = font.pointSize();
    if (fontSize < 45) {
        changeFont(fontSize + 2);
    }
}

void MainWindow::zoomOut() {
    int fontSize = font.pointSize();
    if (fontSize > 7) {
        changeFont(fontSize - 2);
    }
}

void MainWindow::defaultZoom() {
    changeFont(12);
}

// Save button action
void MainWindow::saveFile() {
    qDebug() << "Save button wowwerwer!";
    if (fileName.isEmpty()) {
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
        setWindowTitle(tr("%1 - Patrick's Text Editor").arg(QFileInfo(fileName).fileName()));
        fileSaved = true;
    }
}

// Save-As button action
void MainWindow::saveAsFile() {
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
    fileSaved = true;
    setWindowTitle(tr("%1 - Patrick's Text Editor").arg(QFileInfo(fileName).fileName()));
    file.close();
}
// Find button action
void MainWindow::find() {
    if (!findDialog) {
        findDialog = new FindDialog(this);
    }

    findDialog->show();
}

void MainWindow::saveLeave() {
    saveFile();
    emit leaveRequested();
}

void MainWindow::leave() {
    fileSaved = true;
    emit readyToClose();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (!fileSaved) {
        if (!exitDialog) {
            exitDialog = new ExitDialog(this);
        }

        exitDialog->show();
        event->ignore();
    }
}

void MainWindow::highlight(QString &findtext, bool highlight, bool caseMatch, bool wholeMatch, QString direction) {
    matchCase = caseMatch;
    matchWhole = wholeMatch;

    if (totalHighlight > 0) {
        if (direction == "up") {
            if (highlightCount == 1) {
                highlightCount = totalHighlight;
            } else {
                highlightCount--;
            }
            qDebug() << highlightCount;
            qDebug() << totalHighlight;
        } else if (direction == "down") {
            if (highlightCount >= totalHighlight) {
                highlightCount = 1;
            } else {
                highlightCount++;
            }
            qDebug() << highlightCount;
            qDebug() << totalHighlight;
        }
    }

    qDebug() << "highlighting";
    QTextCursor cursor(maintext->document());

    QTextCharFormat highlightFormatCurrent;
    QColor lightBlueColor(173, 216, 230);
    highlightFormatCurrent.setBackground(lightBlueColor);
    QTextCharFormat highlightFormatOthers;
    highlightFormatOthers.setBackground(Qt::yellow);
    QTextCharFormat highlightFormatTransparent;
    highlightFormatTransparent.setBackground(Qt::transparent);
    //QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.clear();

    int count = 0;

    // Regular expressions for whole word matching
    QString pattern;
    if (wholeMatch) {
        pattern = "\\b" + QRegularExpression::escape(findtext) + "\\b";
    } else {
        pattern = QRegularExpression::escape(findtext);
    }
    QRegularExpression re(pattern);

    // Cycles through the text looking for matches
    if (!findtext.isEmpty()) {
        while (!cursor.isNull() && !cursor.atEnd()) {
            if (caseMatch) {
                cursor = maintext->document()->find(re, cursor, QTextDocument::FindCaseSensitively);
            } else {
                cursor = maintext->document()->find(re, cursor);
            }
            if (!cursor.isNull()) {
                count++;
                if (count == highlightCount) {
                    QTextEdit::ExtraSelection extraBlue;
                    extraBlue.cursor = cursor;
                    extraBlue.format = highlightFormatCurrent;
                    extraSelections.append(extraBlue);
                } else if (highlight) {
                    QTextEdit::ExtraSelection extraYellow;
                    extraYellow.cursor = cursor;
                    extraYellow.format = highlightFormatOthers;
                    extraSelections.append(extraYellow);
                } else {
                    QTextEdit::ExtraSelection extraTransparent;
                    extraTransparent.cursor = cursor;
                    extraTransparent.format = highlightFormatTransparent;
                    extraSelections.append(extraTransparent);
                }
            }
        }
    }

    // Keeps track of the total number of matches
    totalHighlight = count;

    // Performs the highlight
    maintext->setExtraSelections(extraSelections);

    // Scroll to the last occurrence of the blue highlight
    QTextCursor lastBlueCursor = maintext->document()->find(re, QTextCursor::End, QTextDocument::FindBackward);
    while (!lastBlueCursor.isNull()) {
        if (lastBlueCursor.charFormat().background().color() == lightBlueColor) {
            maintext->setTextCursor(lastBlueCursor);
            maintext->ensureCursorVisible();
            break;
        }
        lastBlueCursor = maintext->document()->find(re, lastBlueCursor, QTextDocument::FindBackward);
    }
}

// Removes all of the highlights upon closing the find dialog
void MainWindow::clearHighlights() {
    extraSelections.clear();
    maintext->setExtraSelections(extraSelections);
}

// The replace function within the find dialog
void MainWindow::replace(bool all, QString &replaceText) {
    qDebug("replace");
    qDebug() << replaceText;
    if (!replaceText.isEmpty()) {
        foreach (const QTextEdit::ExtraSelection &sel, extraSelections) {
            QTextCursor cur = sel.cursor;
            QTextCharFormat fmt = sel.format;
            qDebug() << "Selection from" << cur.selectionStart() << "to" << cur.selectionEnd();
            qDebug() << "Background color:" << fmt.background().color().name();

            // Check if the background color is one of the highlighted colours
            QString bgColor = fmt.background().color().name();
            if (bgColor == "#add8e6" || (all && (bgColor == "#000000" || bgColor == "#ffff00"))) {

                // Replace the selected text
                cur.beginEditBlock(); // Start a new edit block for undo/redo
                cur.insertText(replaceText);
                cur.endEditBlock(); // End the edit block

                // Breaks after the first replacement
                if (!all) {
                    break;
                }
            }
        }

        // Sends a signal to cast "highlight" again, updating the highlights
        emit highlightRequest();
    }
}

void MainWindow::updateLineCount() {
    QTextEdit *edit = qobject_cast<QTextEdit *>(sender());
    Q_ASSERT(edit);
    QTextCursor cursor = edit->textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);

    int lines = 1;
    while(cursor.positionInBlock()>0) {
        cursor.movePosition(QTextCursor::Up);
        lines++;
    }
    QTextBlock block = cursor.block().previous();

    while(block.isValid()) {
        lines += block.lineCount();
        block = block.previous();
    }
    namelabel->setText(QString("Line %1      |      Patrick Rocha's Text Editor").arg(lines));
}

void MainWindow::updateWindowTitle() {
    fileSaved = false;
    if (fileName.isEmpty()) {
        setWindowTitle(tr("*No File Open - Patrick's Text Editor"));
    } else {
        setWindowTitle(tr("*%1 - Patrick's Text Editor").arg(QFileInfo(fileName).fileName()));
    }
}
