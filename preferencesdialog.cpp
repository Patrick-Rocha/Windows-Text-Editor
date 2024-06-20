#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include <QPushButton>
#include <QPlainTextEdit>
#include <QSlider>
#include <QVBoxLayout>
#include <QIntValidator>
#include <QString>
#include <QDebug>

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    restore1Button = findChild<QPushButton*>("restore1Button");
    restore2Button = findChild<QPushButton*>("restore2Button");
    restore3Button = findChild<QPushButton*>("restore3Button");
    restore4Button = findChild<QPushButton*>("restore4Button");

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
    redSliders[0]->setValue(128);
    redTexts[0]->setText("128");
    greenSliders[0]->setValue(128);
    greenTexts[0]->setText("128");
    blueSliders[0]->setValue(128);
    blueTexts[0]->setText("128");
}


void PreferencesDialog::on_restore2Button_clicked()
{
    redSliders[1]->setValue(128);
    redTexts[1]->setText("128");
    greenSliders[1]->setValue(128);
    greenTexts[1]->setText("128");
    blueSliders[1]->setValue(128);
    blueTexts[1]->setText("128");
}


void PreferencesDialog::on_restore3Button_clicked()
{
    redSliders[2]->setValue(128);
    redTexts[2]->setText("128");
    greenSliders[2]->setValue(128);
    greenTexts[2]->setText("128");
    blueSliders[2]->setValue(128);
    blueTexts[2]->setText("128");
}

void PreferencesDialog::on_restore4Button_clicked()
{
    redSliders[3]->setValue(128);
    redTexts[3]->setText("128");
    greenSliders[3]->setValue(128);
    greenTexts[3]->setText("128");
    blueSliders[3]->setValue(128);
    blueTexts[3]->setText("128");
}

void PreferencesDialog::handleColourRequest() {
    qDebug() << "Colour request sent";
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
