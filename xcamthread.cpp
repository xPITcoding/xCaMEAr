#include "xcamthread.h"
#include "xcwdgt.h"
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QCoreApplication>
#include <stdlib.h>
#include <tiffio.h>
#include <math.h>

xCamThread::xCamThread(xCWdgt *parent):QThread ()
{
    pParentWdgt=parent;
    abb=false;
}

xCamThread::~xCamThread()
{}

void xCamThread::saveTIFF(QString fname,void* pBuffer,long dimx,long dimy)
{
    bool noError    = true;
    uint64 offset   = 0;

    TIFF *tif=TIFFOpen(fname.toLocal8Bit().constData(),"w");

    if (tif!=nullptr)
    {
        // write data type
        // write voxel size
        // write data

        uint32 imageWidth       = dimx;
        uint32 imageLength      = dimy;
        uint32 bitsPerPixel     = 8;
        uint32 bytesPerPixel    = ceil((double)bitsPerPixel/8.0);
        uint32 scanLineInBytes  = dimx*bytesPerPixel;

        if (CAMERA_IMAGE_FORMAT==QImage::Format_Grayscale16)
        {
            bitsPerPixel     = 16;
            bytesPerPixel    = ceil((double)bitsPerPixel/8.0);
            scanLineInBytes  = dimx*bytesPerPixel;
        }

        // no idea for the resolution in the moment
        unsigned short resUnit  = 3;    // cm
        float pixInX            = 10.0f;
        float pixInY            = 10.0f;

        tdata_t buf             = (tdata_t)(pBuffer);

        TIFFSetField(tif,TIFFTAG_COMPRESSION,   COMPRESSION_NONE);
        TIFFSetField(tif,TIFFTAG_PHOTOMETRIC,   PHOTOMETRIC_MINISBLACK);
        TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL,1);
        TIFFSetField(tif,TIFFTAG_PLANARCONFIG,  PLANARCONFIG_CONTIG);
        TIFFSetField(tif,TIFFTAG_SOFTWARE,      "reader_xPIT_2020");
        TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,  SAMPLEFORMAT_UINT);
        TIFFSetField(tif,TIFFTAG_IMAGEWIDTH,    imageWidth);
        TIFFSetField(tif,TIFFTAG_IMAGELENGTH,   imageLength);
        TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE, bitsPerPixel);
        TIFFSetField(tif,TIFFTAG_XRESOLUTION,   pixInX);
        TIFFSetField(tif,TIFFTAG_YRESOLUTION,   pixInY);
        TIFFSetField(tif,TIFFTAG_RESOLUTIONUNIT,resUnit);

        for (long y=0;y<dimy;++y)
            TIFFWriteScanline(tif,(tdata_t)((uint64)buf+y*scanLineInBytes),y,scanLineInBytes);

        TIFFClose(tif);
    }
    else noError=false;
}


void xCamThread::run()
{
    while (!abb)
    {
        if (camera)
        {
            try
            {

                // Start the grabbing of c_countOfImagesToGrab images.
                // The camera device is parameterized with a default configuration which
                // sets up free-running continuous acquisition.
                camera->StartGrabbing( 1000);

                // This smart pointer will receive the grab result data.
                CGrabResultPtr ptrGrabResult;
                CPylonImage iimg;
                quint8 *pImageBuffer;
                int dimx,dimy;

                // Camera.StopGrabbing() is called automatically by the RetrieveResult() method
                // when c_countOfImagesToGrab images have been retrieved.
                while ( camera->IsGrabbing() && !abb)
                {
                    // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
                    camera->RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);

                    // Image grabbed successfully?
                    if (ptrGrabResult->GrabSucceeded())
                    {
                        dimx=ptrGrabResult->GetWidth();
                        dimy=ptrGrabResult->GetHeight();
                        iimg.AttachGrabResultBuffer(ptrGrabResult);
                        pImageBuffer = (quint8 *) ptrGrabResult->GetBuffer();
                        // memory handling
                        QImage bild((uchar*)iimg.GetBuffer(),dimx, dimy, CAMERA_IMAGE_FORMAT);

                        // streaming
                        if (isStreaming())
                        {
                            //bild.save(_streamingPath+"/"+_suffix+QString("_%1.bmp").arg(_streamingCount++,6,10,QChar('0')));
                            saveTIFF(_streamingPath+"/"+_suffix+QString("_%1.tif").arg(_streamingCount++,6,10,QChar('0')),pImageBuffer,dimx,dimy);
                        }

                        // should by asynchron!!
                        QCoreApplication::postEvent(pParentWdgt,new CameraThread_Event(new QImage(bild)));
                        //pParentWdgt->emit MSG(QString("av. intensity: %1").arg(_sumBuffer));
                    }
                    else
                    {
                        pParentWdgt->emit MSG(QString("Error: %1 %2").arg(ptrGrabResult->GetErrorCode()).arg((const char*)ptrGrabResult->GetErrorDescription()));
                    }
                }
                camera->StopGrabbing();
            }
            catch (const GenericException &e)
            {
                // Error handling.
                pParentWdgt->emit MSG(QString("Error: An exception occurred. %1").arg(e.GetDescription()));
            }
        }
    }
}
