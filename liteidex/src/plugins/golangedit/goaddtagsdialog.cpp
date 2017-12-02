#include "goaddtagsdialog.h"
#include "ui_goaddtagsdialog.h"

GoAddTagsDialog::GoAddTagsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GoAddTagsDialog)
{
    ui->setupUi(this);
    connect(ui->jsonGroupBox,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));
    connect(ui->jsonOptionsCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));
    connect(ui->jsonOptionsLineEdit,SIGNAL(textChanged(QString)),this,SLOT(updateaAguments()));

    connect(ui->xmlGroupBox,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));
    connect(ui->xmlOptionsCheckBox,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));
    connect(ui->xmlOptionsLineEdit,SIGNAL(textChanged(QString)),this,SLOT(updateaAguments()));

    connect(ui->customGroupBox,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));
    connect(ui->customTagNameLineEdit1,SIGNAL(textChanged(QString)),this,SLOT(updateaAguments()));
    connect(ui->customTagOptionLineEdit1,SIGNAL(textChanged(QString)),this,SLOT(updateaAguments()));
    connect(ui->customTagNameLineEdit2,SIGNAL(textChanged(QString)),this,SLOT(updateaAguments()));
    connect(ui->customTagOptionlineEdit2,SIGNAL(textChanged(QString)),this,SLOT(updateaAguments()));

    connect(ui->snakeCaseRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));
    connect(ui->camelCaseRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));
    connect(ui->lispCaseRadioButton,SIGNAL(toggled(bool)),this,SLOT(updateaAguments()));

    ui->snakeCaseRadioButton->setChecked(true);
}

GoAddTagsDialog::~GoAddTagsDialog()
{
    delete ui;
}

void GoAddTagsDialog::setInfo(const QString &info)
{
    ui->infoLabel->setText(info);
}

QString GoAddTagsDialog::arguments() const
{
    return ui->argumentsEdit->toPlainText().trimmed();
}

static QString parserTag(const QString &tag)
{
   int pos = tag.indexOf(":");
   if (pos == -1) {
       return tag;
   }
   return tag.left(pos);
}
void GoAddTagsDialog::updateaAguments()
{
    QStringList tagList;
    QStringList optList;
    QString tranform;
    if (ui->jsonGroupBox->isChecked()) {
        tagList << "json";
        if (ui->jsonOptionsCheckBox->isChecked()) {
            QStringList opt = ui->jsonOptionsLineEdit->text().trimmed().split(",",QString::SkipEmptyParts);
            foreach (QString o, opt) {
                optList << "json="+o;
            }
        }
    }
    if (ui->xmlGroupBox->isChecked()) {
        tagList << "xml";
        if (ui->xmlOptionsCheckBox->isChecked()) {
            QStringList opt = ui->xmlOptionsLineEdit->text().trimmed().split(",",QString::SkipEmptyParts);
            foreach (QString o, opt) {
                optList << "xml="+o;
            }
        }
    }
    if (ui->customGroupBox->isChecked()) {
        QString tag1 = ui->customTagNameLineEdit1->text().trimmed();
        QStringList opt1 = ui->customTagOptionLineEdit1->text().trimmed().split(",",QString::SkipEmptyParts);
        QString tag2 = ui->customTagNameLineEdit2->text().trimmed();
        QStringList opt2 = ui->customTagOptionlineEdit2->text().trimmed().split(",",QString::SkipEmptyParts);
        if (!tag1.isEmpty()) {
            tagList << tag1;
            foreach (QString o, opt1) {
                optList << parserTag(tag1)+"="+o;
            }
        }
        if (!tag2.isEmpty()) {
            tagList << tag2;
            foreach (QString o, opt2) {
                optList << parserTag(tag2)+"="+o;
            }
        }
    }
    QString info;
    if (ui->snakeCaseRadioButton->isChecked()) {
        tranform = "snakecase";
        info = "BaseDomain -> base_domain";
    } else if (ui->camelCaseRadioButton->isChecked()) {
        tranform = "camelcase";
        info = "BaseDomain -> baseDomain";
    } else if (ui->lispCaseRadioButton->isChecked()) {
        tranform = "lispcase";
        info = "BaseDomain -> base-domain";
    }
    if (ui->transformInfo->text() != info) {
        ui->transformInfo->setText(info);
    }

    QString command;
    if (!tagList.isEmpty()) {
        command += "-add-tags "+tagList.join(",");
        if (!optList.isEmpty()) {
            command += " -add-options "+optList.join(",");
        }
        command += " -transform "+tranform;
    }
    ui->argumentsEdit->setPlainText(command);
}
