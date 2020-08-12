#include "xrsettingswdgt.h"
#include "ui_xrsettingswdgt.h"
#include "pathbrowsewdgt.h"
#include "xrcolorsettingsitem.h"

#include <QFile>
#include <QFileInfo>
#include <QComboBox>
#include <QLineEdit>

QMap <QString,settings_node> _settings;

xrSettingsWdgt::xrSettingsWdgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::xrSettingsWdgt)
{
    ui->setupUi(this);
    ui->pSettingsTable->resizeColumnToContents(0);

    _iconMap["general"] = QIcon(QPixmap(":/images/general_settings.png"));
    _iconMap["electrode"] = QIcon(QPixmap(":/images/analysis.png"));
    _iconMap["calcium"] = QIcon(QPixmap(":/images/cell.png"));

    QFileInfo info("rdrSettings.dat");
    if (info.exists())
    {
        QFile f(info.absoluteFilePath());
        if (f.open(QFile::ReadOnly))
        {
            QDataStream d(&f);
            load(d);
            f.close();
        }
        populateTable();
    }
    else {
        standardValues();
    }
    connect(ui->pStandardTB,SIGNAL(clicked()),this,SLOT(standardValues()));
}

xrSettingsWdgt::~xrSettingsWdgt()
{
    QFile f("rdrSettings.dat");
    if (f.open(QFile::WriteOnly))
    {
        QDataStream d(&f);
        save(d);
        f.close();
    }
    delete ui;
}

QStringList xrSettingsWdgt::toString()
{
    QStringList lst;

    for (QMap <QString,settings_node>::iterator it=::_settings.begin();it!=::_settings.end();++it)
    {
        switch (it->_type)
        {
        case MT_FILE:
        case MT_DIR:
        case MT_STRING: lst.append(it.key()+","+it->_stringValue);break;
        case MT_FLOAT: lst.append(it.key()+","+QString("%1").arg(it->_floatValue));break;
        case MT_INT: lst.append(it.key()+","+QString("%1").arg(it->_intValue));break;
        case MT_BOOL: it->_boolValue ?  lst.append(it.key()+","+"true") : lst.append(it.key()+","+"false");break;
        case MT_STRING_LIST:
        {
            QString s=it.key();
            for (QStringList::iterator itt=it->_stringList.begin();itt!=it->_stringList.end();++itt)
                s+=","+(*itt);
        } break;
        case MT_CHOICE: lst.append(it.key()+","+it->_stringList.at(it->_intValue));break;
        case MT_COLOR: lst.append(it.key()+","+it->_colorValue.name());break;
        }
    }

    return lst;
}

void xrSettingsWdgt::standardValues()
{
    ::_settings.clear();
    ::_settings["FPS"]._type=MT_FLOAT;
    ::_settings["FPS"]._floatValue=50.0f;
    ::_settings["FPS"]._rubrik= "calcium";
    ::_settings["minimum interval length [s]"]._type=MT_FLOAT;
    ::_settings["minimum interval length [s]"]._floatValue=0.1f;
    ::_settings["minimum interval length [s]"]._rubrik="calcium";
    ::_settings["FPS(electrode)"]._type=MT_FLOAT;
    ::_settings["FPS(electrode)"]._floatValue=10000.0f;
    ::_settings["FPS(electrode)"]._rubrik= "electrode";
    ::_settings["last imported file"]._type=MT_FILE;
    ::_settings["last imported file"]._stringValue="/home/xpit/reader_settings.txt";
    ::_settings["last imported file"]._rubrik = "general";
    /*::_settings["preview reduction"]._type=MT_INT;::_settings["preview reduction"]._intValue=4;
    ::_settings["preview reduction"]._rubrik = "3d view";
    QStringList sLst; sLst << "fast" << "precise";
    ::_settings["thumbnail calculation mode"]._type=MT_CHOICE;
    ::_settings["thumbnail calculation mode"]._intValue=0;
    ::_settings["thumbnail calculation mode"]._stringList=sLst;
    ::_settings["thumbnail calculation mode"]._rubrik = "3d view";
    ::_settings["light intensity [%]"]._type=MT_FLOAT;::_settings["light intensity [%]"]._floatValue=100;
    ::_settings["light intensity [%]"]._rubrik = "3d view";
    sLst.clear(); sLst << "rendered" << "calculated";
    ::_settings["2D display mode"]._type=MT_CHOICE;
    ::_settings["2D display mode"]._intValue=1;
    ::_settings["2D display mode"]._stringList=sLst;
    ::_settings["2D display mode"]._rubrik = "2d view";
    ::_settings["overlay color"]._type=MT_COLOR;::_settings["overlay color"]._colorValue=QColor(Qt::lightGray);
    ::_settings["overlay color"]._rubrik = "2d view";
    ::_settings["cursor color"]._type=MT_COLOR;::_settings["cursor color"]._colorValue=QColor(Qt::lightGray);
    ::_settings["cursor color"]._rubrik = "2d view";
    ::_settings["background color"]._type=MT_COLOR;::_settings["background color"]._colorValue=QColor(Qt::black);
    ::_settings["background color"]._rubrik = "3d view";
    ::_settings["show axis label"]._type=MT_BOOL;
    ::_settings["show axis label"]._rubrik = "3d view";
    ::_settings["show axis label"]._boolValue = true;
    ::_settings["show x-plane"]._type=MT_BOOL;
    ::_settings["show x-plane"]._rubrik = "3d view";
    ::_settings["show x-plane"]._boolValue = true;
    ::_settings["show y-plane"]._type=MT_BOOL;
    ::_settings["show y-plane"]._rubrik = "3d view";
    ::_settings["show y-plane"]._boolValue = true;
    ::_settings["show z-plane"]._type=MT_BOOL;
    ::_settings["show z-plane"]._rubrik = "3d view";
    ::_settings["show z-plane"]._boolValue = true;
    ::_settings["show grid"]._type=MT_BOOL;
    ::_settings["show grid"]._rubrik = "3d view";
    ::_settings["show grid"]._boolValue = true;
    ::_settings["frame color"]._type=MT_COLOR;
    ::_settings["frame color"]._colorValue=QColor(Qt::lightGray);
    ::_settings["frame color"]._rubrik = "3d view";
    ::_settings["clip plane color"]._type=MT_COLOR;
    ::_settings["clip plane color"]._colorValue=QColor(Qt::lightGray);
    ::_settings["clip plane color"]._rubrik = "3d view";
    ::_settings["show clip plane"]._type=MT_BOOL;
    ::_settings["show clip plane"]._rubrik = "3d view";
    ::_settings["show clip plane"]._boolValue = true;
    ::_settings["show clip plane in 2D"]._type=MT_BOOL;
    ::_settings["show clip plane in 2D"]._rubrik = "2d view";
    ::_settings["show clip plane in 2D"]._boolValue = true;
    ::_settings["clip plane active"]._type=MT_BOOL;
    ::_settings["clip plane active"]._rubrik = "3d view";
    ::_settings["clip plane active"]._boolValue = true;
    ::_settings["show volume data"]._type=MT_BOOL;
    ::_settings["show volume data"]._rubrik = "3d view";
    ::_settings["show volume data"]._boolValue = true;
    sLst.clear(); sLst << "frame" << "plane" << "frame and plane";
    ::_settings["frame mode"]._type=MT_CHOICE;
    ::_settings["frame mode"]._intValue=1;
    ::_settings["frame mode"]._stringList=sLst;
    ::_settings["frame mode"]._rubrik = "3d view";
    ::_settings["show scale bar"]._type=MT_BOOL;
    ::_settings["show scale bar"]._rubrik = "2d view";
    ::_settings["show scale bar"]._boolValue = true;
    ::_settings["show slice indicator"]._type=MT_BOOL;
    ::_settings["show slice indicator"]._rubrik = "2d view";
    ::_settings["show slice indicator"]._boolValue = true;
    ::_settings["show cursor"]._type=MT_BOOL;
    ::_settings["show cursor"]._rubrik = "2d view";
    ::_settings["show cursor"]._boolValue = true;
    ::_settings["identify plane"]._type=MT_BOOL;
    ::_settings["identify plane"]._rubrik = "2d view";
    ::_settings["identify plane"]._boolValue = true;
    ::_settings["high definition renderer"]._type=MT_BOOL;
    ::_settings["high definition renderer"]._rubrik="3d view";
    ::_settings["high definition renderer"]._boolValue=false;
    ::_settings["region growing threshhold"]._type=MT_INT;
    ::_settings["region growing threshhold"]._intValue=4500;
    ::_settings["punch dock"]._type = MT_INT;
    ::_settings["punch dock"]._intValue = 100;
    */

    populateTable();
}

void xrSettingsWdgt::show()
{
    populateTable();
    QWidget::show();
}

QTreeWidgetItem* xrSettingsWdgt::findOrCreateParentItem(const settings_node &node)
{
    QTreeWidgetItem *pItem=nullptr;
    for (int i=0;i<ui->pSettingsTable->topLevelItemCount();++i)
    {
        if (ui->pSettingsTable->topLevelItem(i)->text(0)==node._rubrik)
            pItem = ui->pSettingsTable->topLevelItem(i);
    }
    if (pItem==nullptr)
    {
        pItem=new QTreeWidgetItem(ui->pSettingsTable);
        pItem->setText(0,node._rubrik);
        pItem->setIcon(0,_iconMap[node._rubrik]);
    }
    return pItem;
}

void xrSettingsWdgt::populateTable()
{
    ui->pSettingsTable->clear();
    int row = 0;
    for (QMap <QString,settings_node>::iterator it=::_settings.begin();it!=::_settings.end();++it)
    {
        //ui->pSettingsTable->setRowCount(row+1);
        //ui->pSettingsTable->setVerticalHeaderItem(row,new QTableWidgetItem(it.key()));

        if (it.key().isEmpty()) continue;

        //const char* txt="Hallo";
        QTreeWidgetItem *pRubrikParent = findOrCreateParentItem(it.value());
        if (pRubrikParent) pRubrikParent->setExpanded(true);

        QTreeWidgetItem *pItem = new QTreeWidgetItem(pRubrikParent);
        pItem->setText(0,it.key());
        //pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);

        switch (it->_type)
        {
        case MT_BOOL :
        {
            QComboBox *pCB = new QComboBox();
            QStringList lst;lst << "yes" << "no";
            pCB->setEditable(true);
            pCB->addItems(lst);
            ui->pSettingsTable->setItemWidget(pItem,1,pCB);
            pCB->setCurrentIndex(it->_boolValue ? 0 : 1);
            connect(pCB,SIGNAL(currentTextChanged(const QString&)),this,SLOT(valueChanged()));
        }
        break;
        case MT_INT :
        {
            QLineEdit *pLE = new QLineEdit();
            pLE->setText(QString("%1").arg((int)it->_intValue));
            ui->pSettingsTable->setItemWidget(pItem,1,pLE);
            connect(pLE,SIGNAL(editingFinished()),this,SLOT(valueChanged()));
        }
            break;
        case MT_FLOAT :
        {
            QLineEdit *pLE = new QLineEdit();
            pLE->setText(QString("%1").arg((float)it->_floatValue));
            ui->pSettingsTable->setItemWidget(pItem,1,pLE);
            connect(pLE,SIGNAL(editingFinished()),this,SLOT(valueChanged()));
        }
            break;
        case MT_STRING :
        {
            QLineEdit *pLE = new QLineEdit();
            pLE->setText(it->_stringValue);
            ui->pSettingsTable->setItemWidget(pItem,1,pLE);
            connect(pLE,SIGNAL(editingFinished()),this,SLOT(valueChanged()));
        }
            break;
        case MT_STRING_LIST :
            {
                xrComboBox *pCB = new xrComboBox(it.key());
                pCB->setEditable(true);
                pCB->setInsertPolicy(QComboBox::InsertAlphabetically);
                pCB->addItems(it->_stringList);
                ui->pSettingsTable->setItemWidget(pItem,1,pCB);
                connect(pCB,SIGNAL(activated(const QString&)),this,SLOT(rubrikItemChangedSLOT()));
                //connect(pCB,SIGNAL(editTextChanged(const QString&)),this,SLOT(rubrikItemChangedSLOT()));
            }
            break;
        case MT_CHOICE:
            {
                xrComboBox *pCB = new xrComboBox(it.key());
                pCB->setEditable(false);
                pCB->addItems(it->_stringList);
                ui->pSettingsTable->setItemWidget(pItem,1,pCB);
                pCB->setCurrentIndex(it->_intValue);
                connect(pCB,SIGNAL(activated(const QString&)),this,SLOT(valueChanged()));
                //connect(pCB,SIGNAL(editTextChanged(const QString&)),this,SLOT(rubrikItemChangedSLOT()));
            }
            break;
        case MT_DIR:
            {
                pathbrowsewdgt *pPBWdgt = new pathbrowsewdgt();
                pPBWdgt->setPath(it->_stringValue,true);
                connect(pPBWdgt,SIGNAL(valueChanged()),this,SLOT(valueChanged()));
                ui->pSettingsTable->setItemWidget(pItem,1,pPBWdgt);
            }
            break;
        case MT_FILE:
            {
                pathbrowsewdgt *pPBWdgt = new pathbrowsewdgt();
                pPBWdgt->setPath(it->_stringValue,false);
                connect(pPBWdgt,SIGNAL(valueChanged()),this,SLOT(valueChanged()));
                ui->pSettingsTable->setItemWidget(pItem,1,pPBWdgt);
            }
            break;
        case MT_COLOR:
            {
                xrColorSettingsItem *pCWdgt = new xrColorSettingsItem(it->_colorValue);
                connect(pCWdgt,SIGNAL(valueChanged()),this,SLOT(valueChanged()));
                ui->pSettingsTable->setItemWidget(pItem,1,pCWdgt);
            }
            break;
        }
    }
    ui->pSettingsTable->resizeColumnToContents(0);
}

void xrSettingsWdgt::save(QDataStream &d)
{
    d << (quint16) ::_settings.count();
    for (QMap <QString,settings_node>::iterator it=::_settings.begin();it!=::_settings.end();++it)
    {
        d << it.key();
        d << (int)it->_type;
        d << it->_important;
        d << it->_rubrik;
        switch (it->_type)
        {
        case MT_BOOL : d << it->_boolValue;break;
        case MT_INT : d << it->_intValue;break;
        case MT_FLOAT : d << it->_floatValue;break;
        case MT_DIR:
        case MT_FILE:
        case MT_STRING : d << it->_stringValue;break;
        case MT_STRING_LIST : {
            d << (int)it->_stringList.count();
            for (int i=0;i<it->_stringList.count();++i)
                d << it->_stringList.at(i);
            }
            break;
        case MT_CHOICE : {
            d << (int)it->_stringList.count();
            for (int i=0;i<it->_stringList.count();++i)
                d << it->_stringList.at(i);
            }
            d << (int)it->_intValue;
            break;
        case MT_COLOR:
            d << it->_colorValue.name();
            break;
        }
    }
}

void xrSettingsWdgt::load(QDataStream &d)
{
    quint16 _count;
    d >> _count;
    int _typeDummy;

    QString _key,_strDummy;
    for (int i=0;i<_count;++i)
    {
        d >> _key;
        d >> _typeDummy;
        ::_settings[_key]._type=(member_type)_typeDummy;
        d >> ::_settings[_key]._important;
        d >> ::_settings[_key]._rubrik;
        switch (::_settings[_key]._type)
        {
        case MT_BOOL : d >> ::_settings[_key]._boolValue;break;
        case MT_INT : d >> ::_settings[_key]._intValue;break;
        case MT_FLOAT : d >> ::_settings[_key]._floatValue;break;
        case MT_DIR:
        case MT_FILE:
        case MT_STRING : d >> ::_settings[_key]._stringValue;break;
        case MT_STRING_LIST :
            {
                QString s;
                int c; d >> c;
                for (int i=0;i<c;++i)
                {
                    d >> s;
                    ::_settings[_key]._stringList.append(s);
                }
            }
            break;
        case MT_CHOICE :
            {
                QString s;
                int c; d >> c;
                for (int i=0;i<c;++i)
                {
                    d >> s;
                    ::_settings[_key]._stringList.append(s);
                }
                d >> ::_settings[_key]._intValue;
            }
            break;
        case MT_COLOR:
            {
                QString s;d >> s;::_settings[_key]._colorValue.setNamedColor(s);
            }
            break;
        }        
    }
}

void xrSettingsWdgt::rubrikItemChangedSLOT()
{
   xrComboBox *pCB = dynamic_cast<xrComboBox*>(sender());
    // find key
    if (pCB && _settings.contains(pCB->rubric()))
    {
        _settings[pCB->rubric()]._stringList.clear();
        for (int i=0;i<pCB->count();++i)
            _settings[pCB->rubric()]._stringList.append(pCB->itemText(i));
        emit rubrikItemChanged();
    }
}

void xrSettingsWdgt::valueChanged()
{
    // only for the special ones
    pathbrowsewdgt* pSenderPathBrowseWdgt = dynamic_cast<pathbrowsewdgt*>(sender());
    QComboBox* pSenderComboBox = dynamic_cast<QComboBox*>(sender());
    QLineEdit* pSenderLEdit = dynamic_cast<QLineEdit*>(sender());
    xrColorSettingsItem* pSenderColorEdit = dynamic_cast<xrColorSettingsItem*>(sender());
    if (pSenderColorEdit)
    {
        for (int i=0;i<ui->pSettingsTable->topLevelItemCount();++i)
        {
            for (int j=0;j<ui->pSettingsTable->topLevelItem(i)->childCount();++j)
            {
                xrColorSettingsItem* pLocal = dynamic_cast<xrColorSettingsItem*>(ui->pSettingsTable->itemWidget(ui->pSettingsTable->topLevelItem(i)->child(j),1));
                if (pLocal && pLocal==pSenderColorEdit && ui->pSettingsTable->topLevelItem(i)->child(j))
                {
                    if (_settings.contains(ui->pSettingsTable->topLevelItem(i)->child(j)->text(0)))
                        _settings[ui->pSettingsTable->topLevelItem(i)->child(j)->text(0)]._colorValue = pLocal->color();
                }
            }
        }

    }
    if (pSenderPathBrowseWdgt)
    {
        for (int i=0;i<ui->pSettingsTable->topLevelItemCount();++i)
        {
            for (int j=0;j<ui->pSettingsTable->topLevelItem(i)->childCount();++j)
            {
                pathbrowsewdgt* pLocal = dynamic_cast<pathbrowsewdgt*>(ui->pSettingsTable->itemWidget(ui->pSettingsTable->topLevelItem(i)->child(j),1));
                if (pLocal && pLocal==pSenderPathBrowseWdgt && ui->pSettingsTable->topLevelItem(i)->child(j))
                {
                    if (_settings.contains(ui->pSettingsTable->topLevelItem(i)->child(j)->text(0)))
                        _settings[ui->pSettingsTable->topLevelItem(i)->child(j)->text(0)]._stringValue = pLocal->path();
                }
            }
        }
    }
    if (pSenderLEdit)
    {
        for (int i=0;i<ui->pSettingsTable->topLevelItemCount();++i)
        {
            for (int j=0;j<ui->pSettingsTable->topLevelItem(i)->childCount();++j)
            {
                QLineEdit* pLocal = dynamic_cast<QLineEdit*>(ui->pSettingsTable->itemWidget(ui->pSettingsTable->topLevelItem(i)->child(j),1));
                if (pLocal && pLocal==pSenderLEdit && ui->pSettingsTable->topLevelItem(i)->child(j))
                {
                    if (_settings.contains(ui->pSettingsTable->topLevelItem(i)->child(j)->text(0)))
                    {
                        switch (_settings[ui->pSettingsTable->topLevelItem(i)->child(j)->text(0)]._type)
                        {
                        case MT_INT: _settings[ui->pSettingsTable->topLevelItem(i)->child(j)->text(0)]._intValue = pLocal->text().toInt();break;
                        case MT_FLOAT: _settings[ui->pSettingsTable->topLevelItem(i)->child(j)->text(0)]._floatValue = pLocal->text().toFloat();break;
                        case MT_STRING: _settings[ui->pSettingsTable->topLevelItem(i)->child(j)->text(0)]._stringValue = pLocal->text();break;
                        }
                    }
                }
            }
        }
    }
    if (pSenderComboBox)
    {
        for (int i=0;i<ui->pSettingsTable->topLevelItemCount();++i)
        {
            for (int j=0;j<ui->pSettingsTable->topLevelItem(i)->childCount();++j)
            {
                QComboBox* pLocal = dynamic_cast<QComboBox*>(ui->pSettingsTable->itemWidget(ui->pSettingsTable->topLevelItem(i)->child(j),1));
                if (pLocal && pLocal==pSenderComboBox && ui->pSettingsTable->topLevelItem(i)->child(j))
                {
                    if (_settings.contains(ui->pSettingsTable->topLevelItem(i)->child(j)->text(0)))
                    {
                        switch (_settings[ui->pSettingsTable->topLevelItem(i)->child(j)->text(0)]._type)
                        {
                        case MT_BOOL: _settings[ui->pSettingsTable->topLevelItem(i)->child(j)->text(0)]._boolValue = pLocal->currentText()=="yes";break;
                        case MT_STRING_LIST:
                            {
                            _settings[ui->pSettingsTable->topLevelItem(i)->child(j)->text(0)]._stringList.clear();
                            for (int i=0;i<pLocal->count();++i)
                                _settings[ui->pSettingsTable->topLevelItem(i)->child(j)->text(0)]._stringList.append(pLocal->itemText(i));
                            }
                            break;
                        case MT_CHOICE: _settings[ui->pSettingsTable->topLevelItem(i)->child(j)->text(0)]._intValue = pLocal->currentIndex();break;
                        }
                    }
                }
            }
        }
    }
    //emit KSignal(SETTINGS_CHANGED);
}
