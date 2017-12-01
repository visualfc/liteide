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

private:
    Ui::GoRemoveTagsDialog *ui;
};

#endif // GOREMOVETAGSDIALOG_H
