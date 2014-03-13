#include "finddocwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

FindDocWidget::FindDocWidget(LiteApi::IApplication *app, QWidget *parent) :
    QWidget(parent), m_liteApp(app)
{
    m_findEdit = new Utils::FilterLineEdit(500);
    m_findEdit->setPlaceholderText(tr("Search"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(1);
    mainLayout->setSpacing(1);

    QHBoxLayout *findLayout = new QHBoxLayout;
    findLayout->setMargin(2);
    findLayout->addWidget(new QLabel(tr("Find")));
    findLayout->addWidget(m_findEdit);

    mainLayout->addLayout(findLayout);

    this->setLayout(mainLayout);
}
