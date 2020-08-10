#ifndef IMAGESTACKIMPORTERDLG_H
#define IMAGESTACKIMPORTERDLG_H

#include <QDialog>
#include <QFileInfoList>
#include <QListWidget>
#include <QThread>
#include <QGraphicsScene>
#include "xlfdlg.h"

namespace Ui {
class ImageStackImporterDlg;
}

class ImageStackImporterDlg;

enum XLFImageType
{
    IT_INVALID = 0x00,
    IT_TIF = 0x01
};

struct XLFImageInfoStruct
{
    QString _absoluteFilePath;
    quint16 _dim[3];
    quint8  _depth;
    float   _pixel_size[3];
    XLFImageType _type;
    int _currentFrame=0;
};

struct XLFImageStackTransferStruct
{
    QList <XLFImageInfoStruct*> _fileLst;
    quint16 _frames;
};

class ISI_CreatePreviewPixmapThread : public QThread
{
public:
    ISI_CreatePreviewPixmapThread(ImageStackImporterDlg* );
    void stop()
    {
        _abb = true;
    }
    virtual void run() override;
    void setFileNameAndFrame(const QString& fname,const int& frame)
    {
        _fname = fname;
        _frame = frame;
    }
protected:
    bool _abb = false;
    QString _fname;
    int _frame;
    ImageStackImporterDlg *pMainDlg;
};

class ImageStackImporterDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ImageStackImporterDlg(XLFImageStackTransferStruct *p,QWidget *parent = nullptr);
    ~ImageStackImporterDlg();

signals:
    void MSG(const QString &);
    void setPrevPixmap_SIGNAL(QPixmap pix);

public slots:
    virtual void accept() override;
    virtual void reject() override;
    void setPrevPixmap(QPixmap pix);

protected slots:

    void addFile();
    void addFolder();
    void clearList();
    void selectionChanged(QListWidgetItem*,QListWidgetItem*);

protected:

    bool addFileToList(const QString& fname);

private:
    Ui::ImageStackImporterDlg *ui;
    XLFImageStackTransferStruct *pReference = NULL;
    ISI_CreatePreviewPixmapThread *pThread = NULL;
    QGraphicsScene *pGS = NULL;
    XLFPixmapItem *pPixmapItem = NULL;
};

#endif // IMAGESTACKIMPORTERDLG_H
