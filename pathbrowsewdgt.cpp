#include "pathbrowsewdgt.h"
#include "ui_pathbrowsewdgt.h"

// the only purpose of this widget is to provide a lineedit and a toolbutton to define files in the settings dialog

#include <QFileDialog>

pathbrowsewdgt::pathbrowsewdgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pathbrowsewdgt)
{
    ui->setupUi(this);

    connect(ui->pPathBrowseTB,SIGNAL(clicked()),this,SLOT(browsePath()));
    connect(ui->pPathLEdit,SIGNAL(editingFinished()),this,SIGNAL(valueChanged()));
}

void pathbrowsewdgt::browsePath()
{
    QString s;
    _isDir ? s=QFileDialog::getExistingDirectory(0,"chose directory ...",ui->pPathLEdit->text()) : s=QFileDialog::getOpenFileName(0,"chose file ...",ui->pPathLEdit->text());
    if (!s.isEmpty())
    {
        ui->pPathLEdit->setText(s);
        emit valueChanged();
    }
}

void pathbrowsewdgt::setPath(const QString &fname, bool iD)
{
    ui->pPathLEdit->setText(fname);
    _isDir = iD;
}

QString pathbrowsewdgt::path()
{
    return ui->pPathLEdit->text();
}

pathbrowsewdgt::~pathbrowsewdgt()
{
    delete ui;
}
