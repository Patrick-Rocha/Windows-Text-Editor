#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include <QPushButton>
#include <QPlainTextEdit>
#include <QSlider>
#include <QVBoxLayout>
#include <QIntValidator>
#include <QString>
#include <QDebug>
#include <QRadioButton>
#include <QFileDialog>
#include <QListWidget>
#include <QFontDatabase>

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Preferences"));

    // Creating some widgets
    restore1Button = findChild<QPushButton*>("restore1Button");
    restore2Button = findChild<QPushButton*>("restore2Button");
    restore3Button = findChild<QPushButton*>("restore3Button");
    restore4Button = findChild<QPushButton*>("restore4Button");
    currentRadio = findChild<QRadioButton*>("currentRadio");
    rememberRadio = findChild<QRadioButton*>("rememebrRadio");
    chooseRadio = findChild<QRadioButton*>("chooseRadio");
    chooseButton = findChild<QPushButton*>("chooseButton");
    chooseText = findChild<QLineEdit*>("chooseText");
    fontText = findChild<QLineEdit*>("fontText");
    fontList = findChild<QListWidget*>("fontList");
    weightList = findChild<QListWidget*>("weightList");
    styleList = findChild<QListWidget*>("styleList");
    spacingText = findChild<QLineEdit*>("spacingText");
    fontRestoreButton = findChild<QPushButton*>("fontRestoreButton");

    // Set up validators and max lengths
    QIntValidator *validator = new QIntValidator(0, 9999, this);

    spacingText->setValidator(validator);
    spacingText->setMaxLength(4);

    // Establishing some connections
    connect(ui->currentRadio, &QRadioButton::clicked, this, &PreferencesDialog::handleDirectoryRequest);
    connect(ui->chooseRadio, &QRadioButton::clicked, this, &PreferencesDialog::handleDirectoryRequest);
    connect(ui->chooseText, &QLineEdit::textChanged, this, &PreferencesDialog::handleDirectoryRequest);
    connect(ui->chooseRadio, &QRadioButton::toggled, this, &PreferencesDialog::editableChooseText);
    connect(ui->fontText, &QLineEdit::textChanged, this, &PreferencesDialog::filterFontList);
    connect(ui->fontList, &QListWidget::itemClicked, this, &PreferencesDialog::updateFontText);
    connect(ui->fontList, &QListWidget::currentItemChanged, this, &PreferencesDialog::updateFontText);
    connect(ui->fontText, &QLineEdit::textChanged, this, &PreferencesDialog::handleFontRequest);
    connect(ui->weightList, &QListWidget::itemClicked, this, &PreferencesDialog::handleWeightRequest);
    connect(ui->weightList, &QListWidget::currentItemChanged, this, &PreferencesDialog::handleWeightRequest);
    connect(ui->styleList, &QListWidget::itemClicked, this, &PreferencesDialog::handleStyleRequest);
    connect(ui->styleList, &QListWidget::currentItemChanged, this, &PreferencesDialog::handleStyleRequest);
    connect(ui->spacingText, &QLineEdit::textChanged, this, &PreferencesDialog::handleSpacingRequest);
    connect(ui->fontRestoreButton, &QPushButton::pressed, this, &PreferencesDialog::restoreDefault);

    PreferencesDialog::editableChooseText(false);

    // Get the list of available fonts
    QStringList fontFamilies = QFontDatabase::families();
    foreach (const QString &family, fontFamilies) {
        fontList->addItem(family);
    }

    restoreDefault();

    // Initialize sliders and text fields
    for (int i = 0; i < 4; ++i) {
        redSliders[i] = findChild<QSlider*>(QString("redSlider%1").arg(i + 1));
        greenSliders[i] = findChild<QSlider*>(QString("greenSlider%1").arg(i + 1));
        blueSliders[i] = findChild<QSlider*>(QString("blueSlider%1").arg(i + 1));

        redTexts[i] = findChild<QLineEdit*>(QString("red%1Text").arg(i + 1));
        greenTexts[i] = findChild<QLineEdit*>(QString("green%1Text").arg(i + 1));
        blueTexts[i] = findChild<QLineEdit*>(QString("blue%1Text").arg(i + 1));

        // Set up validators and max lengths
        QIntValidator *validator = new QIntValidator(0, 255, this);

        redTexts[i]->setValidator(validator);
        redTexts[i]->setMaxLength(3);
        connect(redTexts[i], &QLineEdit::textChanged, this, &PreferencesDialog::updateSliderFromText);
        connect(redSliders[i], &QSlider::valueChanged, this, &PreferencesDialog::updateTextFromSlider);
        connect(redTexts[i], &QLineEdit::textChanged, this, &PreferencesDialog::validateInput);

        greenTexts[i]->setValidator(validator);
        greenTexts[i]->setMaxLength(3);
        connect(greenTexts[i], &QLineEdit::textChanged, this, &PreferencesDialog::updateSliderFromText);
        connect(greenSliders[i], &QSlider::valueChanged, this, &PreferencesDialog::updateTextFromSlider);
        connect(greenTexts[i], &QLineEdit::textChanged, this, &PreferencesDialog::validateInput);

        blueTexts[i]->setValidator(validator);
        blueTexts[i]->setMaxLength(3);
        connect(blueTexts[i], &QLineEdit::textChanged, this, &PreferencesDialog::updateSliderFromText);
        connect(blueSliders[i], &QSlider::valueChanged, this, &PreferencesDialog::updateTextFromSlider);
        connect(blueTexts[i], &QLineEdit::textChanged, this, &PreferencesDialog::validateInput);

        // Connecting the sliders to the signals
        connect(redSliders[i], &QSlider::valueChanged, this, &PreferencesDialog::handleColourRequest);
        connect(greenSliders[i], &QSlider::valueChanged, this, &PreferencesDialog::handleColourRequest);
        connect(blueSliders[i], &QSlider::valueChanged, this, &PreferencesDialog::handleColourRequest);
    }

    // Sets the default values
    on_restore1Button_clicked();
    on_restore2Button_clicked();
    on_restore3Button_clicked();
    on_restore4Button_clicked();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::validateInput() {
    QLineEdit *senderLineEdit = qobject_cast<QLineEdit*>(sender());
    if (!senderLineEdit) return;

    QString text = senderLineEdit->text();
    int value = text.toInt();

    if (value < 0) value = 0;
    if (value > 255) value = 255;

    QString validatedText = QString::number(value);
    if (validatedText != text) {
        bool signalsBlocked = senderLineEdit->blockSignals(true);
        senderLineEdit->setText(validatedText);
        senderLineEdit->blockSignals(signalsBlocked);
    }
}

void PreferencesDialog::updateSliderFromText() {
    QLineEdit *senderLineEdit = qobject_cast<QLineEdit*>(sender());
    if (!senderLineEdit) return;

    QSlider *slider = nullptr;

    // Find corresponding slider based on senderLineEdit
    for (int i = 0; i < 4; ++i) {
        if (senderLineEdit == redTexts[i]) {
            slider = redSliders[i];
            break;
        } else if (senderLineEdit == greenTexts[i]) {
            slider = greenSliders[i];
            break;
        } else if (senderLineEdit == blueTexts[i]) {
            slider = blueSliders[i];
            break;
        }
    }

    if (slider) {
        int value = senderLineEdit->text().toInt();
        slider->setValue(value);
    }
}

void PreferencesDialog::updateTextFromSlider(int value) {
    QSlider *senderSlider = qobject_cast<QSlider*>(sender());
    if (!senderSlider) return;

    QLineEdit *lineEdit = nullptr;

    // Determine index based on sender slider
    for (int i = 0; i < 4; ++i) {
        if (senderSlider == redSliders[i]) {
            lineEdit = redTexts[i];
            break;
        } else if (senderSlider == greenSliders[i]) {
            lineEdit = greenTexts[i];
            break;
        } else if (senderSlider == blueSliders[i]) {
            lineEdit = blueTexts[i];
            break;
        }
    }

    if (lineEdit) {
        lineEdit->setText(QString::number(value));
    }
}

void PreferencesDialog::on_restore1Button_clicked()
{
    redSliders[0]->setValue(0);
    redTexts[0]->setText("0");
    greenSliders[0]->setValue(0);
    greenTexts[0]->setText("0");
    blueSliders[0]->setValue(0);
    blueTexts[0]->setText("0");
}


void PreferencesDialog::on_restore2Button_clicked()
{
    redSliders[1]->setValue(255);
    redTexts[1]->setText("255");
    greenSliders[1]->setValue(255);
    greenTexts[1]->setText("255");
    blueSliders[1]->setValue(255);
    blueTexts[1]->setText("255");
}


void PreferencesDialog::on_restore3Button_clicked()
{
    redSliders[2]->setValue(173);
    redTexts[2]->setText("173");
    greenSliders[2]->setValue(216);
    greenTexts[2]->setText("216");
    blueSliders[2]->setValue(230);
    blueTexts[2]->setText("230");
}

void PreferencesDialog::on_restore4Button_clicked()
{
    redSliders[3]->setValue(255);
    redTexts[3]->setText("255");
    greenSliders[3]->setValue(255);
    greenTexts[3]->setText("255");
    blueSliders[3]->setValue(0);
    blueTexts[3]->setText("0");
}

void PreferencesDialog::handleColourRequest() {
    QSlider *senderSlider = qobject_cast<QSlider*>(sender());
    if (!senderSlider) return;

    int index = -1;
    for (int i = 0; i < 4; ++i) {
        if (senderSlider == redSliders[i] || senderSlider == greenSliders[i] || senderSlider == blueSliders[i]) {
            index = i;
            break;
        }
    }

    int red = redSliders[index]->value();
    int green = greenSliders[index]->value();
    int blue = blueSliders[index]->value();

    // Emit signal with RGB values and index
    emit colourRequest(red, green, blue, index);
}

// Lets the user choose a path and puts it onto chooseText
void PreferencesDialog::on_chooseButton_clicked()
{
    QString directoryPath = QFileDialog::getExistingDirectory(this, tr("Select Directory"), QDir::homePath());
    if (!directoryPath.isEmpty()) {
        ui->chooseText->setText(directoryPath);
    }
}

// Sends a signal which contains everything that needs to be known about the directory
void PreferencesDialog::handleDirectoryRequest() {
    QString directory = "";
    bool current = false;
    bool remember = false;
    if (ui->currentRadio->isChecked()) {
        current = true;
    } else {
        directory = ui->chooseText->text();
    }
    emit directoryRequest(current, remember, directory);
}

void PreferencesDialog::editableChooseText(bool checked) {
    ui->chooseText->setReadOnly(!checked);
}

void PreferencesDialog::filterFontList(QString text) {
    QList<QListWidgetItem *> items = fontList->findItems(text, Qt::MatchStartsWith);
    if (!items.isEmpty()) {
        fontList->scrollToItem(items.first());
        fontList->setCurrentItem(items.first());
    }
}

void PreferencesDialog::updateFontText(QListWidgetItem *item) {
    if (item) {
        fontText->setText(item->text());
    }
}

void PreferencesDialog::handleFontRequest(QString fontChange) {
    emit fontRequest(fontChange);
}

void PreferencesDialog::handleNotifyFont(QFont currentFont) {
    fontText->setText(currentFont.family());
}

void PreferencesDialog::handleWeightRequest(QListWidgetItem *weight) {
    emit weightRequest(weight->text());
}

void PreferencesDialog::handleStyleRequest(QListWidgetItem *style) {
    emit styleRequest(style->text());
}

void PreferencesDialog::handleSpacingRequest(QString spacing) {
    emit spacingRequest(spacing.toInt());
}

// Restores the default options for the font section
void PreferencesDialog::restoreDefault() {
    // Sets the font family
    QList<QListWidgetItem*> items = ui->fontList->findItems("Segoe UI", Qt::MatchExactly);
    if (!items.isEmpty()) {
        fontList->setCurrentItem(items.first());
    }

    // Sets the font weight
    items = ui->weightList->findItems("Normal", Qt::MatchExactly);
    weightList->setCurrentItem(items.first());

    // Sets the font style
    items = ui->styleList->findItems("Normal", Qt::MatchExactly);
    styleList->setCurrentItem(items.first());

    // Sets the spacing
    spacingText->setText("100");
}
