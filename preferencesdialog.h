#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QKeyEvent>
#include <QMimeData>
#include <QSlider>
#include <QLineEdit>
#include <QRadioButton>
#include <QListWidget>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog();

public slots:
    void handleDirectoryRequest();
    void handleFontRequest(QString fontChange);
    void handleNotifyFont(QFont currentFont);
    void handleWeightRequest(QListWidgetItem *weight);
    void handleStyleRequest(QListWidgetItem *style);
    void handleSpacingRequest(QString spacing);

signals:
    void colourRequest(int red, int green, int blue, int index);
    void directoryRequest(bool current, bool remember, QString directory);
    void fontRequest(QString fontChange);
    void weightRequest(QString weight);
    void styleRequest(QString style);
    void spacingRequest(int spacing);

private:
    Ui::PreferencesDialog *ui;

    QPushButton *restore1Button;
    QPushButton *restore2Button;
    QPushButton *restore3Button;
    QPushButton *restore4Button;

    QSlider *redSliders[4];
    QSlider *greenSliders[4];
    QSlider *blueSliders[4];
    QLineEdit *redTexts[4];
    QLineEdit *greenTexts[4];
    QLineEdit *blueTexts[4];

    QRadioButton *currentRadio;
    QRadioButton *rememberRadio;
    QRadioButton *chooseRadio;
    QPushButton *chooseButton;
    QLineEdit *chooseText;

    QLineEdit *fontText;
    QListWidget *fontList;
    QListWidget *weightList;
    QListWidget *styleList;
    QLineEdit *spacingText;
    QPushButton *fontRestoreButton;

private slots:
    void validateInput();
    void updateSliderFromText();
    void updateTextFromSlider(int value);
    void on_restore1Button_clicked();
    void on_restore2Button_clicked();
    void on_restore3Button_clicked();
    void on_restore4Button_clicked();
    void handleColourRequest();
    void on_chooseButton_clicked();
    void editableChooseText(bool checked);
    void filterFontList(QString text);
    void updateFontText(QListWidgetItem *item);
    void restoreDefault();
};

#endif // PREFERENCESDIALOG_H
