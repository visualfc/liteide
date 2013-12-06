#include "exportdialog.h"
#include "ui_exportdialog.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

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
