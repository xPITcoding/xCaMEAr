/* handles all settings, saving etc.
 * the settings object is external and can be used in any parts of the application
 * the settings are organized in a map based on QString keys
 * CDullin xPIT 2020
 */

#ifndef XRSETTINGSWDGT_H
#define XRSETTINGSWDGT_H

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QTreeWidget>
#include <QComboBox>

namespace Ui {
class xrSettingsWdgt;
}

// the different settings types that are supported
enum member_type
{
    MT_STRING,              // _stringValue is used
    MT_FLOAT,               // _floatValue is used
    MT_INT,                 // _intValue is used
    MT_BOOL,                // _boolValue is used
    MT_STRING_LIST,         // _stringList is used
    MT_FILE,                // _stringValue is used
    MT_DIR,                 // _stringValue is used
    MT_CHOICE,              // _stringList in combination with _intValue (current choice) is used
    MT_COLOR                // _colorValue is used
};

struct settings_node
{
public:
    member_type         _type;              // settings type
    bool                _important;         // flag for the application if an update is needed
    QString             _stringValue;       // the values which are used depend on the setting type
    QStringList         _stringList;
    float               _floatValue;
    int                 _intValue;
    bool                _boolValue;
    QColor              _colorValue;
    QString             _rubrik;            // only for sorting the settings in the dialog
    settings_node()
    {
        _important = false;
        _rubrik = "general";
    }
};

// general entry to the settings dialog
class xrSettingsTreeWidgetItem : public QTreeWidgetItem
{
public:
    xrSettingsTreeWidgetItem(QTreeWidgetItem* parent): QTreeWidgetItem(parent){};
    virtual void setData(int column, int role, const QVariant &value) override {
        if (role == Qt::ItemIsEditable && column != 1){
            return;
        }
        QTreeWidgetItem::setData(column, role, value);
    }
};

// to display lists and choices
class xrComboBox: public QComboBox
{
    Q_OBJECT
public:
    xrComboBox(const QString& rub):QComboBox()
    {
        _rubric = rub;
    }
    QString rubric(){return _rubric;}
protected:
    QString _rubric;
};

// settings dialog (mainly a QListWidget
// supports loading and saving of the settings to one file
// sends a KSignal if the settings have been modified
class xrSettingsWdgt : public QWidget
{
    Q_OBJECT

public:
    explicit xrSettingsWdgt(QWidget *parent = nullptr);
    ~xrSettingsWdgt();

    void populateTable();

    static void save(QDataStream &d);
    static void load(QDataStream &d);

    QTreeWidgetItem * findOrCreateParentItem(const settings_node& node);
    QStringList toString();

protected slots:
    void valueChanged();
    void rubrikItemChangedSLOT();

public slots:
    void standardValues();
    virtual void show();

signals:
    //void KSignal(const SIG_TYPE&,void* data=nullptr);
    void importantParameterModified();
    void rubrikItemChanged();

private:
    Ui::xrSettingsWdgt *ui;
    QMap <QString,QIcon> _iconMap;
};

extern QMap <QString,settings_node> _settings;

#endif // XRSETTINGSWDGT_H
