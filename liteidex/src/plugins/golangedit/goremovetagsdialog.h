#ifndef GOREMOVETAGSDIALOG_H
#define GOREMOVETAGSDIALOG_H

#include <QDialog>

namespace Ui {
class GoRemoveTagsDialog;
}

class GoRemoveTagsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GoRemoveTagsDialog(QWidget *parent = 0);
    ~GoRemoveTagsDialog();
    void setInfo(const QString &info);
    QString arguments() const;
public slots:
    void updateArguments();
private:
    Ui::GoRemoveTagsDialog *ui;
};

#endif // GOREMOVETAGSDIALOG_H
