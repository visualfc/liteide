#ifndef FOLDERDIALOG_H
#define FOLDERDIALOG_H

#include <QDialog>

class QLineEdit;
class QLabel;
class CreateFileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CreateFileDialog(QWidget *parent = 0);
    void setDirectory(const QString &path);
    QString getFileName() const;
    bool isOpenEditor() const { return m_bOpenEditor; }
protected slots:
    void createAndEdit();
protected:
    bool m_bOpenEditor;
    QLabel    *m_dirLabel;
    QLineEdit *m_fileNameEdit;
};

class CreateDirDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CreateDirDialog(QWidget *parent = 0);
    void setDirectory(const QString &path);
    QString getDirName() const;
protected:
    QLabel    *m_dirLabel;
    QLineEdit *m_dirNameEdit;
};

#endif // FOLDERDIALOG_H
