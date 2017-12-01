#include "goaddtagsdialog.h"
#include "ui_goaddtagsdialog.h"

GoAddTagsDialog::GoAddTagsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GoAddTagsDialog)
{
    ui->setupUi(this);
}

GoAddTagsDialog::~GoAddTagsDialog()
{
    delete ui;
}
