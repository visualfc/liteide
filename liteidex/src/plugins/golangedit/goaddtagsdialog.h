#ifndef GOADDTAGSDIALOG_H
#define GOADDTAGSDIALOG_H

#include <QDialog>

namespace Ui {
class GoAddTagsDialog;
}

class GoAddTagsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GoAddTagsDialog(QWidget *parent = 0);
    ~GoAddTagsDialog();
private:
    Ui::GoAddTagsDialog *ui;
};

#endif // GOADDTAGSDIALOG_H
