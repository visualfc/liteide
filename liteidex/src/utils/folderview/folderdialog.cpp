#include "folderdialog.h"
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGridLayout>

CreateFileDialog::CreateFileDialog(QWidget *parent) :
    QDialog(parent), m_bOpenEditor(false)
{
    this->setWindowTitle(tr("Create File"));
    QGridLayout *grid = new QGridLayout;
    QLabel *label1 = new QLabel(tr("Directory:"));
    m_dirLabel = new QLabel();
    QLabel *label2 = new QLabel(tr("File Name:"));
    m_fileNameEdit = new QLineEdit();
    grid->addWidget(label1,0,0);
    grid->addWidget(m_dirLabel,0,1);
    grid->addWidget(label2,1,0);
    grid->addWidget(m_fileNameEdit,1,1);

    QHBoxLayout *hbox = new QHBoxLayout;
    QPushButton *btn1 = new QPushButton(tr("Create"));
    QPushButton *btn2 = new QPushButton(tr("Create and Edit"));
    QPushButton *btn3 = new QPushButton(tr("Cancel"));
    hbox->addStretch();
    hbox->addWidget(btn1);
    hbox->addWidget(btn2);
    hbox->addWidget(btn3);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addLayout(grid);
    vbox->addStretch(0);
    vbox->addLayout(hbox);
    this->setLayout(vbox);

    connect(btn1,SIGNAL(clicked()),this,SLOT(accept()));
    connect(btn2,SIGNAL(clicked()),this,SLOT(createAndEdit()));
    connect(btn3,SIGNAL(clicked()),this,SLOT(reject()));
}

void CreateFileDialog::setDirectory(const QString &path)
{
    m_dirLabel->setText(path);
}

QString CreateFileDialog::getFileName() const
{
    return m_fileNameEdit->text();
}

void CreateFileDialog::createAndEdit()
{
    m_bOpenEditor = true;
    this->accept();
}

CreateDirDialog::CreateDirDialog(QWidget *parent) :
    QDialog(parent)
{
    this->setWindowTitle(tr("Create Folder"));
    QGridLayout *grid = new QGridLayout;
    QLabel *label1 = new QLabel(tr("Directory:"));
    m_dirLabel = new QLabel();
    QLabel *label2 = new QLabel(tr("Dir Name:"));
    m_dirNameEdit = new QLineEdit();
    grid->addWidget(label1,0,0);
    grid->addWidget(m_dirLabel,0,1);
    grid->addWidget(label2,1,0);
    grid->addWidget(m_dirNameEdit,1,1);

    QHBoxLayout *hbox = new QHBoxLayout;
    QPushButton *btn1 = new QPushButton(tr("Create"));
    QPushButton *btn2 = new QPushButton(tr("Cancel"));
    hbox->addStretch();
    hbox->addWidget(btn1);
    hbox->addWidget(btn2);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addLayout(grid);
    vbox->addStretch(0);
    vbox->addLayout(hbox);
    this->setLayout(vbox);

    connect(btn1,SIGNAL(clicked()),this,SLOT(accept()));
    connect(btn2,SIGNAL(clicked()),this,SLOT(reject()));
}

void CreateDirDialog::setDirectory(const QString &path)
{
    m_dirLabel->setText(path);
}

QString CreateDirDialog::getDirName() const
{
    return m_dirNameEdit->text();
}
