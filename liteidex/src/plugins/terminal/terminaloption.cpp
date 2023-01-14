#include "terminaloption.h"
#include "ui_terminaloption.h"
#include "terminal_global.h"
#include <QFontDatabase>
#include <QApplication>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

TerminalOption::TerminalOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::TermianlOption)
{
    ui->setupUi(m_widget);
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
    m_fontFamily = ui->fontComboBox->currentText();
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
    m_fontFamily = m_liteApp->settings()->value(TERMINAL_FAMILY,TERMINAL_FAMILY_DEFAULT).toString();
    m_fontSize = m_liteApp->settings()->value(TERMINAL_FONTSIZE,12).toInt();
    int fontZoom = m_liteApp->settings()->value(TERMINAL_FONTZOOM,100).toInt();
    bool antialias = m_liteApp->settings()->value(TERMINAL_ANTIALIAS,true).toBool();

    ui->antialiasCheckBox->setChecked(antialias);

    if (!QFontDatabase().hasFamily(m_fontFamily)) {
#if QT_VERSION >= 0x050200
        m_fontFamily = QFontDatabase::systemFont(QFontDatabase::FixedFont).family();
#else
        m_fontFamily = QApplication::font().family();
#endif
    }
    ui->fontComboBox->setEditText(m_fontFamily);
    int index = ui->fontComboBox->findText(m_fontFamily);
    if (index >= 0) {
        ui->fontComboBox->setCurrentIndex(index);
    }

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
    const QString familyName = ui->fontComboBox->currentText();
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

