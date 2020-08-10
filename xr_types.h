/* provides basic data types and enumerations for xRender
 * CDullin xPIT 2020
 */

#ifndef XR_TYPES_H
#define XR_TYPES_H

#include <QVector>
#include <QtDataVisualization/qcustom3dvolume.h>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>

using namespace QtCharts;

// event types for costum events (mainly for sending them out of a thread
enum EVENT_TYPES
{
    FULL_RESOLUTION_DONE        = QEvent::User,
    PREV_RESOLUTION_DONE        = QEvent::User+1,
    START_CREATING_TEXTURE      = QEvent::User+2,
    PROGRESS_CREATING_TEXTURE   = QEvent::User+3,
    COLOR_LUT_DONE              = QEvent::User+4
};

// render mode (we are using indexed only
enum RENDER_TYPE
{
    RT_RGBA     = 0x00,
    RT_INDEXED  = 0x01
};

struct _3D_VIS_PARAM
{
    Q3DScatter *m_graph = nullptr;                  // 3DScene handles camera, light etc.
    RENDER_TYPE m_renderType = RT_INDEXED;          // render type (we use 8bit indexed with look up table
    int m_decimation = -1;                          // data reduction for preview rendering
    bool m_isPreview = false;                       // flag if preview is currently displayed
    float m_level, m_window;                        // 2D display parameter not in rendered mode,
                                                    // < (m_level-m_window/2) = black, (m_level-m_window/2 .. m_level+m_window/2) from black to white, > (m_level+m_window/2) = white

    QVector<QRgb> m_colorTable;                     // general LUT for both preview and high res (0.254) 255 = transparent for clipping

    bool m_previewNeedsClippingUpdate = false;      // flag for update of clipping display
    QCustom3DVolume *m_prevVolumeItem = nullptr;    // preview volume rendering
    QVector <uchar> *m_reducedIndexed;              // preview texture buffer

    bool m_higResNeedsClippingUpdate = false;       // flag for update of clipping display (highg res)
    QCustom3DVolume *m_volumeItem = nullptr;        // high res volume rendering
    QVector <uchar> *m_Indexed;                     // high res tecture buffer

    int m_sliceIndexX;                              // slice position along X Axis (in preview devided by decimation)
    int m_sliceIndexY;
    int m_sliceIndexZ;
    bool m_slicingX;                                // flag if plane is displayed or not in 3D
    bool m_slicingY;
    bool m_slicingZ;

    bool m_movePlane = false;                       // flag if free cutting plane is displayed
    QCustom3DItem *m_clipPlane = nullptr;           // definition of the cutting plane (obj format as resources)
    QList <QCustom3DItem*> punchList;
};


enum SIG_TYPE
{
    VOBJ_MODIFIED                   = 0x01,         // emited if the original data has been modified -> update of histogram, 2D and 3D needed
    UPDATE_HISTO                    = 0x02,         // update of histogram needed
    RENDER_PARAM_CHANGED            = 0x03,         // render parameter have been modified
    CONNECT_CHILDREN_TO_MAINWINDOW  = 0x04,         // sends mainwindow reference to all child windows
    VOBJ_LOADED                     = 0x05,         // sends pointer to data container
    SET_PROGRESS_STRING             = 0x06,         // set string for displaying progress (*QString)
    SET_PROGRESS_MAXIMUM            = 0x07,         // set maximum for displaying progress (*float)
    SHOW_PROGRESS_BAR               = 0x08,         // show progress bar
    HIDE_PROGRESS_BAR               = 0x09,         // hide progress bar
    DISP_PROGRESS                   = 0x0A,         // displays current progress (*int)
    DISP_2D_CHANGED                 = 0x0B,         // 2D View has been modified
    ACTIVATE_PREVIEW_MODE           = 0x0C,         // peview rendering was activated
    SETTINGS_CHANGED                = 0x0D,         // settings have been modified (all children need to check on their own)
    CLIPPING_CHANGED                = 0x0E,         // clip plane has been repositioned
    SAVE_CUT_PLANE                  = 0x0F          // cut plane needs to be exported
};

// file types for the importer
// currently only VOX is supported
enum SUPPORTED_FILE_TYPES
{
    FT_RAW      = 0x00,
    FT_VOX      = 0x01,
    FT_VFF      = 0x02,
    FT_VOL      = 0x03,
    FT_INVALID  = 0xFF
};

// data types
// currently unsigned 16bit is assumed
enum VOLUME_DATA_TYPE
{
    VDT_INVALID     = 0x00,
    VDT_I8          = 0x01,
    VDT_UI8         = 0x02,
    VDT_I16         = 0x03,
    VDT_UI16        = 0x04,
    VDT_FLOAT       = 0x05,
    VDT_DOUBLE      = 0x06,
    VDT_RGB8        = 0x07
};

// definition of the histogram
struct HISTO_DATA
{
    long _histoBins = 300;
    double _maxBinValue;
    double _minGVal,_maxGVal;
    float *pHistogram = nullptr;
};

// main data container
class VOLUME_DATA
{
public:
    long _dim[4];                           // dimensions (x,y,z and time) currently time is not supported
    long _reducedDim[4];                    // dimensions of the previeww data
    VOLUME_DATA_TYPE _type;                 // data type
    void* pData=nullptr;                    // buffer to the data (16bit)
    void* pReducedData=nullptr;             // buffer to the reduced data (8bit)
    //QCustom3DVolume *pRenderData=nullptr;
    double _minGVal,_maxGVal;               // min max in original data
    long _histoBins = 300;                  // histo bins
    float* pHistogram = nullptr;            // histo data
    SUPPORTED_FILE_TYPES _fileType;         // file type
    float _vs[4];                           // voxel size in mm (and s)
    bool _modified = true;                  // data has been modified flag
    double _maxBinValue = 0;                // highest value in on bin for scaling
    bool _maskValueNotSet = true;           // is masking used
    float _maskValue = 255.0f;              // index value for masking
    VOLUME_DATA()
    {}

    void generateHistoData(const int& _bC=300);
};

#endif // XR_TYPES_H
