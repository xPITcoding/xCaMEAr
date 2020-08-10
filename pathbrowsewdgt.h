/*
 * Import Widget to define a path for the settings dialog
 * CDullin xPIT 2020
 */

#ifndef PATHBROWSEWDGT_H
#define PATHBROWSEWDGT_H

#include <QWidget>

namespace Ui {
class pathbrowsewdgt;
}

class pathbrowsewdgt : public QWidget
{
    Q_OBJECT

public:
    explicit pathbrowsewdgt(QWidget *parent = nullptr);
    ~pathbrowsewdgt();

    void setPath(const QString& fname,bool isDir=false);
    QString path();

protected slots:
    void browsePath();

signals:
    void valueChanged();

private:
    Ui::pathbrowsewdgt *ui;
    bool _isDir = false;
};

#endif // PATHBROWSEWDGT_H
