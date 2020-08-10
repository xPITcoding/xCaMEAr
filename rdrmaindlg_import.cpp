#include "rdrmaindlg.h"
#include "ui_rdrmaindlg.h"
#include "effecttoolbutton.h"
#include "iostream"
#include "rdr_calc.h"
#include "xrsettingswdgt.h"
#include "catransient.h"

#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QTextStream>
#include <QTimer>
#include <QDate>
#include <QGraphicsSceneMouseEvent>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QCategoryAxis>
#include <QMovie>
#include <QTimer>

#include "tiffio.h"
#include "math.h"
using namespace std;

QImage rdrMainDlg::loadTIFFImage(const QString& fname)
{
    QImage img;
    data._frameRate= ::_settings["FPS"]._floatValue;
    if (::_settings.contains("pixel size"))
        data._pixelSize= ::_settings["pixel size"]._floatValue;
    else
        data._pixelSize=1.0f;
    const char* _fileName = fname.toUtf8().constData();
    TIFF *tif = TIFFOpen(_fileName,"r");
    if (tif)
    {
        uint32 imageWidth, imageLength;
        uint32 bitsPerPixel;
        unsigned short resUnit;
        uint32 row;
        uint64 offset;
        tdata_t buf = nullptr;
        float pixInX;

        TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&imageWidth);
        TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&imageLength);
        TIFFGetField(tif,TIFFTAG_BITSPERSAMPLE,&bitsPerPixel);
        TIFFGetField(tif,TIFFTAG_XRESOLUTION,&pixInX);
        TIFFGetField(tif,TIFFTAG_RESOLUTIONUNIT,&resUnit);
/*
            switch (resUnit)
            {
            case 2 : // inch
                _data._pix_size = 25.4f / pixInX;
                break;
            case 3 : // cm
                _data._pix_size = 10.0f / pixInX;
                break;
            default:
                _data._pix_size = -1.0f;
                break;
            }
*/
        long imgDim = imageWidth/4*4;
        int bytes = 1;

        uchar* pBuffer=(uchar*)_TIFFmalloc(imgDim*imageLength*bytes);
        if (buf==nullptr) buf = _TIFFmalloc(TIFFScanlineSize(tif));
        offset = (uint64)pBuffer;
        for (row = 0; row < imageLength; row++)
        {
            TIFFReadScanline(tif, buf, row);
            memcpy((void*)offset,buf,imgDim*bytes);
            offset+=imgDim*bytes;
        }
        _TIFFfree(buf);

        bytes == 1 ? img = QImage(pBuffer,imgDim,imageLength,QImage::Format_Grayscale8) : QImage(pBuffer,imgDim,imageLength,QImage::Format_Grayscale16);

        TIFFClose(tif);
    }
    _sceneActive = true;
    return img;
}

void rdrMainDlg::on_pCalciumTB_clicked()
{
    //import calcium
    QString path = QFileDialog::getExistingDirectory(0,"pick calcium source directory",::_settings["last imported file"]._stringValue,QFileDialog::ShowDirsOnly);
    if (!path.isEmpty())
    {
        ::_settings["last imported file"]._stringValue=path;
        ui->pCalciumLE->setText(path);

    }

}



void rdrMainDlg::on_pBackgroundTB_clicked()
{
    //import background
    QString path = QFileDialog::getExistingDirectory(0,"pick backgound source directory",::_settings["last imported file"]._stringValue,QFileDialog::ShowDirsOnly);
    if (!path.isEmpty())
    {
        ::_settings["last imported file"]._stringValue=path;
        ui->pBackgroundLE->setText(path);

    }

}

void rdrMainDlg::on_pElectrodesTB_clicked()
{
    //import electrode data
    QString fname = QFileDialog::getOpenFileName(0,"pick electrode data",::_settings["last imported file"]._stringValue,QString("*.csv *.odt"));
    if (!fname.isEmpty())
    {
        ::_settings["last imported file"]._stringValue=fname;
        QFileInfo info(fname);
        ui->pElectrodesLE->setText(info.absoluteFilePath());
    }

}

void rdrMainDlg::on_pDigitalTB_clicked()
{
    //import starding point data data
    QString path = QFileDialog::getOpenFileName(0,"pick digital input data",::_settings["last imported file"]._stringValue, QString("*.csv *.odt"));
    if (!path.isEmpty())
    {
        ::_settings["last imported file"]._stringValue=path;
        ui->pDigitalLE->setText(path);

    }

}

void rdrMainDlg::flatFielding()
{
    //provided that the images in the two stacks are the same format and size
    if(!data._backgroundLst.isEmpty() && !data._frameLst.isEmpty())
    {
        ui->pProgressLbl->setText("calculating mean image");
        // create a mean image of the background
        long W = data._frameLst.at(0)->width();
        long H = data._frameLst.at(0)->height();
        long _size=W*H;
        float *pAvBuffer=(float*)malloc(W*H*sizeof (float));
        float pixelAv=0.0f;
        ui->pProgressBar->setRange(0,_size);
        for (long i=0;i<_size; i++)
        {
            ui->pProgressBar->setValue(i);
            for(int img=0; img<data._backgroundInFloat.size();img++)
                pixelAv+=data._backgroundInFloat[img][i];
            pixelAv/=(float)data._backgroundLst.size();
            pAvBuffer[i]=pixelAv;
        }
        ui->pProgressLbl->setText("background correction");
         ui->pProgressBar->setRange(0,data._frameLst.count()-1);
        //divide every image in the calcium stack by the mean background image
        for(int img = 0; img<data._frameLst.size(); img++)
        {
            ui->pProgressBar->setValue(img);
            for(long i=0; i<_size; i++)
                data._framesInFloat[img][i]/=pAvBuffer[i];
        }

    }
    else
    {
        ui->pErrorTxtB->append("Flat fielding failed. One or more stacks weren't loaded");
        return;
    }
}


void rdrMainDlg::on_pImport_clicked()

{
    QFile f("/home/xpit/reader_settings.txt");
    if (f.open(QFile::WriteOnly))
    {
        QTextStream t(&f);
        t << ui->pCalciumLE->text() << Qt::endl;
        t << ui->pBackgroundLE->text() << Qt::endl;
        t << ui->pElectrodesLE->text() << Qt::endl;
        t << ui->pDigitalLE->text() << Qt::endl;
        f.close();
    }
    //store calcium data
    data._frameLst.clear();
    data._framesInFloat.clear();
    if(!ui->pCalciumLE->text().isNull())
    {
        QDir d(ui->pCalciumLE->text());
        d.setFilter(QDir::Files);
        d.setNameFilters(QStringList() << "*.tiff" << "*.tif" << "*.TIF" << "*.TIFF");
        QFileInfoList fileLst = d.entryInfoList();

        ui->pProgressBar->setRange(0,fileLst.count()-1);
        ui->pProgressLbl->setText("loading calcium data");
        long i=0;
        float miv,mav;
        QImage *img=nullptr;
        for (QFileInfoList::iterator it=fileLst.begin();it!=fileLst.end();++it)
        {
            ui->pProgressBar->setValue(i++);
            qApp->processEvents();
            //emit message(it->absoluteFilePath());
            img = new QImage(loadTIFFImage(it->absoluteFilePath()));
            data._frameLst.append(img);
            data._framesInFloat.append(convertImageToFloatBuffer(img));
            /*
            if (i>1)
            {
                data._minVal = std::min(miv,data._minVal);
                data._maxVal = std::max(mav,data._maxVal);
            }
            else
            {
                data._minVal = miv;
                data._maxVal = mav;
            }
            */

        }
    }

    //store backgroud data
    data._backgroundLst.clear();
    data._backgroundInFloat.clear();
    QDir d(ui->pBackgroundLE->text());

    if(!ui->pBackgroundLE->text().isEmpty() && d.exists())
    {
        d.setFilter(QDir::Files);
        d.setNameFilters(QStringList() << "*.tiff" << "*.tif" << "*.TIF" << "*.TIFF");
        QFileInfoList fileLst = d.entryInfoList();

        ui->pProgressBar->setRange(0,fileLst.count()-1);
        ui->pProgressLbl->setText("loading backgound");
        long i=0;
        QImage *pImg;

        for (QFileInfoList::iterator it=fileLst.begin();it!=fileLst.end();++it)
        {
            ui->pProgressBar->setValue(i++);
            qApp->processEvents();
            //emit message(it->absoluteFilePath());
            pImg = new QImage(loadTIFFImage(it->absoluteFilePath()));
            data._backgroundLst.append(pImg);
            data._backgroundInFloat.append(convertImageToFloatBuffer(pImg));
        }

        //do background correction
        if(!data._backgroundLst.isEmpty())
        flatFielding();

    }
    else{
        ui->pErrorTxtB->append("Background correction wasn't performed");
    }


    // update display ...

    if (data._frameLst.count()>0)
    {
        ui->pSlider->setRange(0, data._frameLst.size()-1);
        displaySlice(0);
    }

    //load electrode data
    if(!ui->pElectrodesLE->text().isEmpty())
    {
        ui->pProgressBar->setRange(0,100);
        ui->pProgressLbl->setText("loading electrode map");
        QFile f(ui->pElectrodesLE->text());

        if(f.open(QFile::ReadOnly))
        {
            ui->pProgressBar->setValue(20);
            QTextStream str(&f);
            str.readLine();
            QString line;
            data._electrodeMap.clear();
            QString _eID;
            QStringList _keyLUT,_valueLst;
            while(!str.atEnd())
            {
                qApp->processEvents();
                line = str.readLine();
                if(line.contains("TimeStamp"))
                {
                    _keyLUT = line.split(',');
                    line=str.readLine();
                    while(!line.isEmpty())
                    {
                        qApp->processEvents();
                        _valueLst=line.split(',');
                        for (int i=0;i<_valueLst.count();++i)
                            data._electrodeMap[_keyLUT[i]].append(_valueLst[i].toFloat());
                        line=str.readLine();
                    }

                }
                ui->pProgressBar->setValue(60);
            }

            f.close();
        }

        QStringList _keyLst;
        ui->pElectrode->clear();
        for (QMap <QString, QList<float>>::iterator it=data._electrodeMap.begin();it!=data._electrodeMap.end();++it)
            _keyLst.append(it.key());
        _keyLst.removeLast();
        ui->pElectrode->addItems(_keyLst);
        ui->pProgressBar->setValue(100);
    }

    //load digital output
    if(!ui->pDigitalLE->text().isEmpty())
    {
      ui->pProgressLbl->setText("loading starting points");
      QFile f2(ui->pDigitalLE->text());
        if(f2.open(QFile::ReadOnly))
        {
            ui->pProgressBar->setValue(20);
            qApp->processEvents();
            QTextStream str2(&f2);
            QString line2;
            data._startFound = false;
            data._strtPnt.clear();
            while(!str2.atEnd())
            {
                qApp->processEvents();
                line2= str2.readLine();
                if(line2.contains("TimeStamp"))
                {
                    qApp->processEvents();
                    line2=str2.readLine();
                    while(!line2.isEmpty())
                    {
                        qApp->processEvents();
                        if (line2.right(1).toInt() == 1 && data._startFound == false)
                        {
                            data._startFound=true;
                            line2=line2.section(',',0,0);
                            data._strtPnt.append(line2.toFloat());
                        }
                        else if(line2.right(1).toInt() ==0 &&data._startFound ==true)
                        {
                            data._startFound = false;
                        }
                     line2=str2.readLine();
                    }

                }
                ui->pProgressBar->setValue(60);
            }

         }
        f2.close();
        for(int i=0; i<data._strtPnt.size(); i++)
        {
            ui->pStartingPoint->addItem(QString("%1").arg(data._strtPnt.at(i)/1000.0f,0,'f',3));
        }
        ui->icelabel->close();
        ui->pProgressBar->setValue(100);
        qApp->processEvents();
        ui->pProgressLbl->setText("Complete");
        ui->tabWidget->setCurrentIndex(2);
    }
}
