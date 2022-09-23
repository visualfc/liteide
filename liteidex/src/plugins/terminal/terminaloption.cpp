#include "terminaloption.h"
#include "ui_terminaloption.h"
#include "terminal_global.h"
#include <QFontDatabase>

TerminalOption::TerminalOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::TermianlOption)
{
    ui->setupUi(m_widget);

    QFontDatabase db;
    m_familyList = db.families();
    ui->familyComboBox->addItems(m_familyList);
}

TerminalOption::~TerminalOption()
{
    delete m_widget;
    delete ui;
}

QWidget *TerminalOption::widget()
{
    return m_widget;
}

QString TerminalOption::name() const
{
    return "Terminal";
}

QString TerminalOption::mimeType() const
{
    return OPTION_TERMIANL;
}

void TerminalOption::save()
{
    m_fontFamily = ui->familyComboBox->currentText();
    if (ui->sizeComboBox->count()) {
        const QString curSize = ui->sizeComboBox->currentText();
        bool ok = true;
        int size = curSize.toInt(&ok);
        if (ok) {
            m_fontSize = size;
        }
    }

    int fontZoom = ui->fontZoomSpinBox->value();
    if (fontZoom <= 10) {
        fontZoom = 10;
    }
    m_liteApp->settings()->setValue(TERMINAL_FAMILY,m_fontFamily);
    m_liteApp->settings()->setValue(TERMINAL_FONTSIZE,m_fontSize);
    m_liteApp->settings()->setValue(TERMINAL_FONTZOOM,fontZoom);

    bool antialias = ui->antialiasCheckBox->isChecked();
    m_liteApp->settings()->setValue(TERMINAL_ANTIALIAS,antialias);
}

void TerminalOption::load()
{
#if defined(Q_OS_WIN)
    m_fontFamily = m_liteApp->settings()->value(TERMINAL_FAMILY,"Courier").toString();
#elif defined(Q_OS_LINUX)
    m_fontFamily = m_liteApp->settings()->value(TERMINAL_FAMILY,"DejaVu Sans Mono").toString();
#elif defined(Q_OS_MAC)
    m_fontFamily = m_liteApp->settings()->value(TERMINAL_FAMILY,"Menlo").toString();
#else
    m_fontFamily = m_liteApp->settings()->value(EDITOR_FAMILY,"Monospace").toString();
#endif
    m_fontSize = m_liteApp->settings()->value(TERMINAL_FONTSIZE,12).toInt();

    int fontZoom = m_liteApp->settings()->value(TERMINAL_FONTZOOM,100).toInt();

    bool antialias = m_liteApp->settings()->value(TERMINAL_ANTIALIAS,true).toBool();
    ui->antialiasCheckBox->setChecked(antialias);

    const int idx = m_familyList.indexOf(m_fontFamily);
    ui->familyComboBox->setCurrentIndex(idx);

    updatePointSizes();

    ui->fontZoomSpinBox->setValue(fontZoom);
}

void TerminalOption::updatePointSizes()
{
    // Update point sizes
    const int oldSize = m_fontSize;
    if (ui->sizeComboBox->count()) {
        ui->sizeComboBox->clear();
    }
    const QList<int> sizeLst = pointSizesForSelectedFont();
    int idx = 0;
    int i = 0;
    for (; i < sizeLst.count(); ++i) {
        if (idx == 0 && sizeLst.at(i) >= oldSize)
            idx = i;
        ui->sizeComboBox->addItem(QString::number(sizeLst.at(i)));
    }
    if (ui->sizeComboBox->count())
        ui->sizeComboBox->setCurrentIndex(idx);
}

QList<int> TerminalOption::pointSizesForSelectedFont() const
{
    QFontDatabase db;
    const QString familyName = ui->familyComboBox->currentText();
    QList<int> sizeLst = db.pointSizes(familyName);
    if (!sizeLst.isEmpty())
        return sizeLst;

    QStringList styles = db.styles(familyName);
    if (!styles.isEmpty())
        sizeLst = db.pointSizes(familyName, styles.first());
    if (sizeLst.isEmpty())
        sizeLst = QFontDatabase::standardSizes();

    return sizeLst;
}

