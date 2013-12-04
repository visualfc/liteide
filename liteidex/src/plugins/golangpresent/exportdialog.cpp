#include "exportdialog.h"
#include "ui_exportdialog.h"

ExportDialog::ExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog),
    m_bView(false)
{
    ui->setupUi(this);
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

QString ExportDialog::fileName() const
{
    return ui->fileName->text();
}

void ExportDialog::setFileName(const QString &text)
{
    ui->fileName->setText(text);
}

void ExportDialog::on_exportButton_clicked()
{
    m_bView = false;
    this->accept();
}

void ExportDialog::on_exportAndViewButton_clicked()
{
    m_bView = true;
    this->accept();
}
