#include "imagestackimporterdlg.h"
#include "ui_imagestackimporterdlg.h"
#include "xlfsettingsdlg.h"
#include <QFileDialog>
#include <QProgressBar>

#include "tiffio.h"

ISI_CreatePreviewPixmapThread::ISI_CreatePreviewPixmapThread(ImageStackImporterDlg* p):QThread(p){pMainDlg=p;}

void ISI_CreatePreviewPixmapThread::run()
{
    _abb = false;
    // read TIFF image
    int _frameNr = 0;

    do {
        if (!_fname.isEmpty())
        {
            const char* _fileName = _fname.toLatin1().constData();
            TIFF *tif = TIFFOpen(_fileName,"r");
            if (tif)
            {
                uint32 imageWidth, imageLength;
                uint32 bitsPerPixel;
                unsigned short resUnit;
                uint32 row;
                uint64 offset;
                tdata_t buf;
                float pixInX;

                //TIFFSetDirectory(tif,_frameNr);

                TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&imageWidth);
                TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&imageLength);
                TIFFGetField(tif,TIFFTAG_BITSPERSAMPLE,&bitsPerPixel);
                TIFFGetField(tif,TIFFTAG_XRESOLUTION,&pixInX);
                TIFFGetField(tif,TIFFTAG_RESOLUTIONUNIT,&resUnit);

                quint16* pBuffer=(quint16*)_TIFFmalloc(imageWidth*imageLength*sizeof(quint16));
                offset = (uint64)pBuffer;
                buf = _TIFFmalloc(TIFFScanlineSize(tif));
                for (row = 0; row < imageLength; row++)
                {
                    TIFFReadScanline(tif, buf, row);
                    memcpy((void*)offset,buf,imageWidth*2);
                    offset+=imageWidth*2;
                }
                _TIFFfree(buf);
                TIFFClose(tif);

                float _minGVal = 65535.0f;
                float _maxGVal = -1.0f;
                for (uint32 lx=0;lx<imageWidth;++lx)
                    for (uint32 ly=0;ly<imageLength;++ly)
                    {
                        _minGVal = std::min(_minGVal,(float)pBuffer[int(lx+imageWidth*ly)]);
                        _maxGVal = std::max(_maxGVal,(float)pBuffer[int(lx+imageWidth*ly)]);
                    }

                float _window = (_maxGVal - _minGVal);
                float _level = (_maxGVal + _minGVal)/2.0f;

                QImage qimg;
                qimg = QImage(imageWidth,imageLength,QImage::Format_RGB32);
                float v;
                quint16 v16;
                float b = 255.0f/(_maxGVal - _minGVal);

                for (uint32 lx=0;lx<imageWidth;++lx)
                    for (uint32 ly=0;ly<imageLength;++ly)
                    {
                        v = (float)(pBuffer[(int)(lx+imageWidth*ly)]);
                        v16 = (quint16)std::max(0.0f,(float)std::min(255.0f,(v - _level)/_window * 255.0f + 128.0f));
                        qimg.setPixel(lx,ly,qRgb(v16,v16,v16));
                    }

                emit pMainDlg->setPrevPixmap_SIGNAL(QPixmap::fromImage(qimg,Qt::MonoOnly));
            }
        }
        _frameNr = (_frameNr+1) % _frame;
        if (!_abb) msleep(100);
    }
    while (_frame>1 && !_abb);
}

ImageStackImporterDlg::ImageStackImporterDlg(XLFImageStackTransferStruct *p,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageStackImporterDlg)
{
    pReference = p;
    ui->setupUi(this);
    ui->pProgBar->hide();
    pThread = new ISI_CreatePreviewPixmapThread(this);
    pGS = new QGraphicsScene(this);
    ui->pGV->setScene(pGS);

    connect(ui->pAddFileTB,SIGNAL(clicked()),this,SLOT(addFile()));
    connect(ui->pAddFolderTB,SIGNAL(clicked()),this,SLOT(addFolder()));
    connect(ui->pClearLstTB,SIGNAL(clicked()),this,SLOT(clearList()));
    connect(ui->pFileListWdgt,SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),this,SLOT(selectionChanged(QListWidgetItem*,QListWidgetItem*)));
    connect(this,SIGNAL(setPrevPixmap_SIGNAL(QPixmap)),this,SLOT(setPrevPixmap(QPixmap)));
}

void ImageStackImporterDlg::setPrevPixmap(QPixmap pix)
{
    pix=pix.scaled(ui->pGV->width(),ui->pGV->height(),Qt::KeepAspectRatio);
    if (!pPixmapItem)
    {
        pPixmapItem=new XLFPixmapItem(0);
        pPixmapItem->setPixmap(pix);
        pGS->addItem(pPixmapItem);
    }
    else {
        pPixmapItem->setPixmap(pix);
        //pPixmapItem->update();
        ui->pGV->update();
    }
}

void ImageStackImporterDlg::selectionChanged(QListWidgetItem *pCurrent,QListWidgetItem *previous)
{
    pCurrent = ui->pFileListWdgt->currentItem();
    if (pCurrent)
    {
        pThread->stop();
        while (pThread->isRunning())
        {
            QThread::msleep(100);
        }
        XLFImageInfoStruct *pInfo = (XLFImageInfoStruct *)(pCurrent->data(Qt::UserRole).toLongLong());

        // change currentIndex
        //pThread->setFileNameAndFrame(pInfo->_absoluteFilePath,pInfo->_dim[2]);
        //pThread->start();
    }
}

void ImageStackImporterDlg::reject()
{
    if (pThread && pThread->isRunning())
    {
        pThread->stop();
        while (pThread->isRunning())
        {
            QThread::msleep(100);
        }
    }
    QDialog::reject();
}

void ImageStackImporterDlg::accept()
{
    if (pReference)
    {
        pReference->_frames=0;
        for (int i=0;i<pReference->_fileLst.count();++i)
            pReference->_frames+=pReference->_fileLst.at(i)->_dim[2];
    }
    if (pThread && pThread->isRunning())
    {
        pThread->stop();
        while (pThread->isRunning())
        {
            QThread::msleep(100);
        }
    }
    QDialog::accept();
}

bool ImageStackImporterDlg::addFileToList(const QString& fname)
{
    //emit MSG(QString("try to add file %1").arg(fname));
    bool _NoError = false;
    // try tiff
    // 1.) test multiframe
    const char* _fileName = fname.toUtf8().constData();
    TIFF *tif = TIFFOpen(_fileName,"r");
    int _frames=-1;
    if (tif)
    {
        /*
        _frames=0;
        do
        {
          _frames++;
        } while (TIFFReadDirectory(tif));
        TIFFClose(tif);

        tif = TIFFOpen(_fileName,"r");
        if (_frames>1) TIFFSetDirectory(tif,0);
        */
        uint32 imageWidth, imageLength;
        uint32 bitsPerPixel;
        TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&imageWidth);
        TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&imageLength);
        TIFFGetField(tif,TIFFTAG_BITSPERSAMPLE,&bitsPerPixel);

        XLFImageInfoStruct *pImageInfo = new XLFImageInfoStruct();
        pImageInfo->_dim[0] = imageWidth;
        pImageInfo->_dim[1] = imageLength;
        pImageInfo->_dim[2] = _frames;
        pImageInfo->_type = IT_TIF;
        pImageInfo->_depth = bitsPerPixel / 8+1;
        pImageInfo->_absoluteFilePath = fname;
        pImageInfo->_currentFrame = 0;

        pReference->_fileLst.append(pImageInfo);
        // add to window
        QFileInfo info(fname);
        QListWidgetItem *pItem = new QListWidgetItem(info.fileName());
        pItem->setData(Qt::UserRole,quint64(pImageInfo));
        ui->pFileListWdgt->addItem(pItem);

        _NoError = true;
        TIFFClose(tif);

        if (ui->pFileListWdgt->currentItem()==0) ui->pFileListWdgt->setCurrentItem(pItem);
    }
    else
        emit MSG("Error - image file format not support");
    return _NoError;
}

void ImageStackImporterDlg::addFile()
{
    QString filter = "*.tif *.tiff *.TIF *.TIFF";
    QString fname = QFileDialog::getOpenFileName(0,"add image file",::_settings["last image import folder"]._stringValue,filter);
    if (!fname.isEmpty())
    {
        if (addFileToList(fname))
        {
            QFileInfo info(fname);
            ::_settings["last image import folder"]._stringValue = info.absolutePath();
        }
    }
    else
        emit MSG("no file chosen");
}

void ImageStackImporterDlg::addFolder()
{
    QString folder = QFileDialog::getExistingDirectory(0,"add image file",::_settings["last image import folder"]._stringValue);
    if (!folder.isEmpty())
    {
        ui->pApplyTB->setEnabled(false);

        QString filter = "*.tif, *.tiff, *.TIF, *.TIFF";
        QDir d(folder);
        d.setNameFilters(filter.split(", "));
        QFileInfoList infoLst=d.entryInfoList();
        bool _NoError = false;

        ui->pProgBar->setMaximum(infoLst.count());
        ui->pProgBar->show();
        ui->pStatusLab->setText("analyzing folder please wait");
        ui->pFileListWdgt->setUpdatesEnabled(false);

        for (int i=0;i<infoLst.count();++i)
        {
            _NoError |= addFileToList(infoLst.at(i).absoluteFilePath());
            ui->pProgBar->setValue(i+1);
            qApp->processEvents();
        }
        if (_NoError)
            ::_settings["last image import folder"]._stringValue = folder;

        ui->pStatusLab->setText("finished");
        ui->pProgBar->hide();
        ui->pFileListWdgt->setUpdatesEnabled(true);
        ui->pFileListWdgt->update();

        ui->pApplyTB->setEnabled(true);
    }
    else
        emit MSG("no folder chosen");
}

void ImageStackImporterDlg::clearList()
{
    ui->pFileListWdgt->clear();
    pReference->_fileLst.clear();
}

ImageStackImporterDlg::~ImageStackImporterDlg()
{
    delete ui;
}
