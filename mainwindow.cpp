#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "exitdialog.h"
#include "preferencesdialog.h"

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
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    findDialog = new FindDialog(this);
    exitDialog = new ExitDialog(this);
    preferencesDialog = new PreferencesDialog(this);

    // Creating the widgets
    mainlayout = findChild<QVBoxLayout*>("mainlayout");
    maintext = findChild<QTextEdit*>("maintext");
    namelabel = findChild<QLabel*>("namelabel");

    // Stylesheets
    maintext->setStyleSheet("QTextEdit { margin: 0px; padding: 0px; border: none; border-top: 1px solid gray; }");
    namelabel->setStyleSheet("QLabel#namelabel { padding-top: 10px; padding-right: 10px; }");
    changeFont(12);
    fontSize = 12;

    // Set margins to 0
    mainlayout->setContentsMargins(0, 0, 0, 0);
    mainlayout->setSpacing(0);

    // Setting the layout to the central widget
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainlayout);
    setCentralWidget(centralWidget);

    // Set the initial title of the window
    setWindowTitle(tr("New File - Patrick's Text Editor"));

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
    connect(preferencesDialog, &PreferencesDialog::colourRequest, this, &MainWindow::changeColours);
    connect(preferencesDialog, &PreferencesDialog::directoryRequest, this, &MainWindow::changeDirectory);
    connect(this, &MainWindow::directoryRequest, preferencesDialog, &PreferencesDialog::handleDirectoryRequest);
    connect(preferencesDialog, &PreferencesDialog::fontRequest, this, &MainWindow::changeFontFamily);
    connect(this, &MainWindow::notifyFont, preferencesDialog, &PreferencesDialog::handleNotifyFont);
    connect(preferencesDialog, &PreferencesDialog::styleRequest, this, &MainWindow::changeFontStyle);
    connect(preferencesDialog, &PreferencesDialog::weightRequest, this, &MainWindow::changeFontWeight);
    connect(preferencesDialog, &PreferencesDialog::spacingRequest, this, &MainWindow::changeFontSpacing);
    connect(exitDialog, &ExitDialog::cancelNotify, this, &MainWindow::handleCancelNotify);

    // Setting initial values for global variables
    highlightCount = 1;
    totalHighlight = 0;
    fileSaved = true;
    matchCase = false;
    matchWhole = false;
    currentHighlightColour = QColor(173, 216, 230);
    otherHighlightColour = QColor(255, 255, 0);
    fileDirectory = "";
    chosenDirectory = "";
    makingNew = false;
    fileName = "";

    // Handling the font
    fontFamilies = QFontDatabase::families();
    QFont currentFont = maintext->font();
    emit notifyFont(currentFont);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Open button action
void MainWindow::openFile() {
    // Open file dialog to select a file
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), chosenDirectory, tr("Text Files (*.txt);;All Files (*)"));

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

    // Saving the file directory into the global variable
    QFileInfo fileInfo(file);
    fileDirectory = fileInfo.path();
    file.close();
    fileSaved = true;

    emit directoryRequest();
}

void MainWindow::changeFont(int size) {
    font = maintext->font();
    font.setPointSize(size);
    fontSize = size;
    maintext->setFont(font);
}

void MainWindow::zoomIn() {
    int fontSize = font.pointSize();
    if (fontSize < 75) {
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
    fileName = QFileDialog::getSaveFileName(maintext, tr("Save File As"), chosenDirectory, tr("Text Files (*.txt);;All Files (*)"));

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

    // Saving the file directory into the global variable
    QFileInfo fileInfo(file);
    fileDirectory = fileInfo.path();
    qDebug() << fileDirectory;
    file.close();

    emit directoryRequest();
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
    if (!makingNew) {
        emit leaveRequested();
    } else {
        fileName = "";
        fileDirectory = "";
        maintext->setText("");
        exitDialog->close();
        fileSaved = true;
        setWindowTitle(tr("New File - Patrick's Text Editor"));
    }
}

void MainWindow::leave() {
    if (!makingNew) {
        emit readyToClose();
    } else {
        fileName = "";
        fileDirectory = "";
        maintext->setText("");
        exitDialog->close();
        fileSaved = true;
        setWindowTitle(tr("New File - Patrick's Text Editor"));
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (!fileSaved) {
        if (!exitDialog) {
            exitDialog = new ExitDialog(this);
        }
        exitDialog->setWindowModality(Qt::ApplicationModal);
        exitDialog->show();
        event->ignore();
    }
}

void MainWindow::highlight(QString &findtext, bool highlight, bool caseMatch, bool wholeMatch, QString direction, bool changingText) {
    matchCase = caseMatch;
    matchWhole = wholeMatch;

    if (totalHighlight > 0) {
        if (direction == "up") {
            if (highlightCount == 1) {
                highlightCount = totalHighlight;
            } else {
                highlightCount--;
            }
        } else if (direction == "down") {
            if (highlightCount >= totalHighlight) {
                highlightCount = 1;
            } else {
                highlightCount++;
            }
        }
    }

    QTextCursor cursor(maintext->document());

    QTextCharFormat highlightFormatCurrent;
    highlightFormatCurrent.setBackground(currentHighlightColour);
    QTextCharFormat highlightFormatOthers;
    highlightFormatOthers.setBackground(otherHighlightColour);
    QTextCharFormat highlightFormatTransparent;
    highlightFormatTransparent.setBackground(Qt::transparent);
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
    int currentHighlightPosition = -1;

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
                    currentHighlightPosition = cursor.position();
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

    // Scroll to the blue highlight to ensure it's visible
    if (!changingText) {
        if (count > 0 and currentHighlightPosition != -1) {
            QTextCursor cursorToScroll = maintext->textCursor();
            //QTextCursor cursorToScroll(maintext->document());
            cursorToScroll.setPosition(currentHighlightPosition);
            maintext->setTextCursor(cursorToScroll);
            maintext->ensureCursorVisible();
        }
    }
}

// Removes all of the highlights upon closing the find dialog
void MainWindow::clearHighlights() {
    extraSelections.clear();
    maintext->setExtraSelections(extraSelections);
}

// The replace function within the find dialog
void MainWindow::replace(bool all, QString &replaceText) {
    if (!replaceText.isEmpty()) {
        foreach (const QTextEdit::ExtraSelection &sel, extraSelections) {
            QTextCursor cur = sel.cursor;
            QTextCharFormat fmt = sel.format;

            // Check if the background color is one of the highlighted colours
            QString bgColour = fmt.background().color().name();

            if (bgColour == currentHighlightColour.name() || (all && (bgColour == "#000000" || bgColour == otherHighlightColour.name()))) {

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
        emit highlightRequest(false);
    }
}

// Updates the line count on the bottom of the main window
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

// Adds an * to the title to show that the file needs to be saved
void MainWindow::updateWindowTitle() {
    fileSaved = false;
    if (fileName.isEmpty()) {
        setWindowTitle(tr("*New File - Patrick's Text Editor"));
    } else if (!makingNew) {
        setWindowTitle(tr("*%1 - Patrick's Text Editor").arg(QFileInfo(fileName).fileName()));
    }

    // Update the highlights to prevent a certain bug
    if (findDialog->isVisible()) {
        emit highlightRequest(true);
    }

    if (makingNew) {
        setWindowTitle(tr("New File - Patrick's Text Editor"));
        makingNew = false;
        fileSaved = true;
    }
}

// Shows the preferences dialog upon clicking "preferences"
void MainWindow::on_actionPreferences_triggered()
{
    if (!preferencesDialog) {
        preferencesDialog = new PreferencesDialog(this);
    }

    preferencesDialog->show();
}

// Changes the colours of various widgets
void MainWindow::changeColours(int red, int green, int blue, int index) {
    if (index == 0) {
        QTextCursor cursor = maintext->textCursor();
        int oldPosition = cursor.position();
        int oldAnchor = cursor.anchor();

        QTextCharFormat format;
        format.setForeground(QColor(red, green, blue));
        cursor.select(QTextCursor::Document);
        cursor.mergeCharFormat(format);

        // Restore cursor position and clear selection
        cursor.setPosition(oldPosition);
        cursor.setPosition(oldAnchor, QTextCursor::KeepAnchor);
        maintext->setTextCursor(cursor);
    } else if (index == 1) {
        QString backgroundColor = QString("rgb(%1, %2, %3)").arg(red).arg(green).arg(blue);
        QString currentStyleSheet = maintext->styleSheet();

        // Check where the background-color is in the stylesheet
        int index = currentStyleSheet.indexOf("background-color:");
        if (index == -1) {
            currentStyleSheet += QString("QTextEdit { background-color: %1; }").arg(backgroundColor);
        } else {
            int endIndex = currentStyleSheet.indexOf(";", index); // Find end of the property
            QString oldBackgroundColor = currentStyleSheet.mid(index, endIndex - index + 1);
            currentStyleSheet.replace(oldBackgroundColor, QString("background-color: %1;").arg(backgroundColor));
        }
        maintext->setStyleSheet(currentStyleSheet);
    } else if (index == 2) {
        currentHighlightColour = QColor(red, green, blue);
        if (findDialog->isVisible()) {
            emit highlightRequest(false);
        }
    } else if (index == 3) {
        otherHighlightColour = QColor(red, green, blue);
        if (findDialog->isVisible()) {
            emit highlightRequest(false);
        }
    }
}

// Changes the directory used by "save as" and "open"
void MainWindow::changeDirectory(bool current, bool remember, QString directory) {
    if (current) {
        chosenDirectory = fileDirectory;
    } else if (remember) {

    } else {
        chosenDirectory = directory;
    }

    // If the directory isn't real
    QDir dir(chosenDirectory);
    if (!(dir.exists() && dir.isReadable())) {
        chosenDirectory = "";
    }
}

// Changes the font of the main text
void MainWindow::changeFontFamily(QString fontChange) {
    if (fontFamilies.contains(fontChange)) {
        QFont newFont(fontChange, fontSize);
        maintext->setFont(newFont);
    }
}

// Changes the font's weight (bold, thin, etc)
void MainWindow::changeFontWeight(QString weight) {
    QFont font = maintext->font();

    QFont::Weight fontWeight = QFont::Normal;
    if (weight == "Thin") {
        fontWeight = QFont::Thin;
    } else if (weight == "Extra Light") {
        fontWeight = QFont::ExtraLight;
    } else if (weight == "Light") {
        fontWeight = QFont::Light;
    } else if (weight == "Normal") {
        fontWeight = QFont::Normal;
    } else if (weight == "Medium") {
        fontWeight = QFont::Medium;
    } else if (weight == "Demi Bold") {
        fontWeight = QFont::DemiBold;
    } else if (weight == "Bold") {
        fontWeight = QFont::Bold;
    } else if (weight == "Extra Bold") {
        fontWeight = QFont::ExtraBold;
    } else if (weight == "Black") {
        fontWeight = QFont::Black;
    }

    font.setWeight(fontWeight);
    maintext->setFont(font);
}

// Changes the font's style
void MainWindow::changeFontStyle(QString style) {
    QFont font = maintext->font();

    QFont::Style fontStyle = QFont::StyleNormal;
    if (style == "Normal") {
        fontStyle = QFont::StyleNormal;
    } else if (style == "Italic") {
        fontStyle = QFont::StyleItalic;
    } else if (style == "Oblique") {
        fontStyle = QFont::StyleOblique;
    }

    font.setStyle(fontStyle);
    maintext->setFont(font);
}

// Changes the font's spacing
void MainWindow::changeFontSpacing(int spacing) {
    QFont font = maintext->font();
    font.setLetterSpacing(QFont::PercentageSpacing, spacing);
    maintext->setFont(font);
}

// Hitting copy
void MainWindow::on_actionCopy_triggered() {
    maintext->copy();
}

// Hitting paste
void MainWindow::on_actionPaste_triggered() {
    maintext->paste();
}

// Hitting the new button
void MainWindow::on_actionNew_triggered()
{
    if (!fileSaved) {
        makingNew = true;
        if (!exitDialog) {
            exitDialog = new ExitDialog(this);
        }
        exitDialog->setWindowModality(Qt::ApplicationModal);
        exitDialog->show();
    } else {
        fileName = "";
        fileDirectory = "";
        makingNew = false;
        maintext->setText("");
        setWindowTitle(tr("New File - Patrick's Text Editor"));
        fileSaved = true;
    }
}

// Helps avoid a certain bug regarding the "new" button
void MainWindow::handleCancelNotify() {
    makingNew = false;
}
