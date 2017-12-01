#include "goremovetagsdialog.h"
#include "ui_goremovetagsdialog.h"

GoRemoveTagsDialog::GoRemoveTagsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GoRemoveTagsDialog)
{
    ui->setupUi(this);
}

GoRemoveTagsDialog::~GoRemoveTagsDialog()
{
    delete ui;
}
