#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ExportDialog(QWidget *parent = 0);
    ~ExportDialog();
    QString fileName() const;
    void setFileName(const QString &text);
    bool isExportAndView() const { return m_bView; }
private slots:
    void on_exportButton_clicked();
    void on_exportAndViewButton_clicked();
private:
    bool m_bView;
    Ui::ExportDialog *ui;
};

#endif // EXPORTDIALOG_H
