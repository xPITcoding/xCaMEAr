#include "xr_types.h"

// we need to calculate the grey value histogram of the data set
// should be done multi-threaded in the future
// I also forsee 4D data sets
void VOLUME_DATA::generateHistoData(const int& _bC)
{
    // lame version
    _histoBins = _bC;
    if (pHistogram!=NULL) free(pHistogram);
    pHistogram = (float*)malloc(_bC*sizeof(float));

    if (pData)
    {
        long planeSize = _dim[0]*_dim[1];
        long cubeSize = planeSize*_dim[2];

        unsigned short* pBuffer=(unsigned short*)(pData);

        _minGVal = pBuffer[0];
        _maxGVal = pBuffer[0];

        long offset = 0;

        for (long lt = 0;lt < _dim[3];++lt)
            for (long lz =0;lz < _dim[2];++lz)
                for (long ly=0;ly < _dim[1];++ly)
                    for (long lx=0;lx<_dim[0];++lx)
                    {
                        _minGVal = std::min(_minGVal,(double)pBuffer[offset]);
                        _maxGVal = std::max(_maxGVal,(double)pBuffer[offset]);
                        offset++;
                    }

        memset(pHistogram,0,_bC*sizeof (float));

        long p;
        offset = 0;
        for (long lt = 0;lt < _dim[3];++lt)
            for (long lz =0;lz < _dim[2];++lz)
                for (long ly=0;ly < _dim[1];++ly)
                    for (long lx=0;lx<_dim[0];++lx)
                    {
                        p = (long)(((float)pBuffer[offset]-_minGVal)/(_maxGVal-_minGVal)*float(_bC));
                        pHistogram[p]++;
                        offset++;
                    }
    }
}
