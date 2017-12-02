#include "goremovetagsdialog.h"
#include "ui_goremovetagsdialog.h"

GoRemoveTagsDialog::GoRemoveTagsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GoRemoveTagsDialog)
{
    ui->setupUi(this);
    connect(ui->clearAllTagsRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateArguments()));
    connect(ui->clearAllOptionsRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateArguments()));

    connect(ui->removeJsonTagRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateArguments()));
    connect(ui->removeXmlOptionRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateArguments()));
    connect(ui->removeCustomTagRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateArguments()));

    connect(ui->removeJsonOptionRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateArguments()));
    connect(ui->removeXmlOptionRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateArguments()));
    connect(ui->removeCustomOptionRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateArguments()));

    connect(ui->customTaglineEdit,SIGNAL(textChanged(QString)),this,SLOT(updateArguments()));
    connect(ui->jsonOptionLineEdit,SIGNAL(textChanged(QString)),this,SLOT(updateArguments()));
    connect(ui->xmlOptionLineEdit,SIGNAL(textChanged(QString)),this,SLOT(updateArguments()));
    connect(ui->customOptionLineEdit,SIGNAL(textChanged(QString)),this,SLOT(updateArguments()));
}

GoRemoveTagsDialog::~GoRemoveTagsDialog()
{
    delete ui;
}

void GoRemoveTagsDialog::setInfo(const QString &info)
{
    ui->infoLabel->setText(info);
}

QString GoRemoveTagsDialog::arguments() const
{
    return ui->argumentsEdit->toPlainText().trimmed();
}

static QString removeHead(const QString &text, const QString &head)
{
    if (text.startsWith(head)) {
        return text.mid(head.length());
    }
    return text;
}

void GoRemoveTagsDialog::updateArguments()
{
    QString args;
    if (ui->clearAllTagsRadioButton->isChecked()) {
        args = "-clear-tags";
    } else if (ui->clearAllOptionsRadioButton->isChecked()) {
        args = "-clear-options";
    } else if (ui->removeJsonTagRadioButton->isChecked()) {
        args = "-remove-tags json";
    } else if (ui->removeXmlTagRadioButton->isChecked()) {
        args = "-remove-tags xml";
    } else if (ui->removeCustomTagRadioButton->isChecked()) {
        QString tag = ui->customTaglineEdit->text().trimmed();
        if (!tag.isEmpty()) {
            args = "-remove-tags "+tag;
        }
    } else if (ui->removeJsonOptionRadioButton->isChecked()) {
        QString opt = ui->jsonOptionLineEdit->text().trimmed();
        if (!opt.isEmpty()) {
            args = "-remove-options json="+removeHead(opt,"json=");
        }
    } else if (ui->removeXmlOptionRadioButton->isChecked()) {
        QString opt = ui->xmlOptionLineEdit->text().trimmed();
        if (!opt.isEmpty()) {
            args = "-remove-options xml="+removeHead(opt,"xml=");
        }
    } else if(ui->removeCustomOptionRadioButton->isChecked()) {
        QString opt = ui->customOptionLineEdit->text().trimmed();
        if (opt == ui->customOptionLineEdit->placeholderText()) {
            if (ui->customOptionLineEdit->cursorPosition() == 0) {
                opt.clear();
            }
        }
        if (opt.contains("=")) {
            args = "-remove-options "+opt;
        }
    }

    ui->argumentsEdit->setPlainText(args);
}
