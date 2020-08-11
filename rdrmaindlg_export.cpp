#include "rdrmaindlg.h"
#include "ui_rdrmaindlg.h"
#include "effecttoolbutton.h"
#include "iostream"
#include "rdr_calc.h"
#include "xrsettingswdgt.h"
#include "catransient.h"
#include "meatransient.h"

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

void rdrMainDlg::on_pExport_clicked()
{
    if(!data._valuesCa.empty() && !data._valuesMEA.empty())
    {
    QString dir = QDir::currentPath();
    QString _date = QDate::currentDate().toString("dd_MM_yyyy");
    QString defaultName = "result_" + _date + ".csv";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"),dir+"/"+ defaultName, tr("Files (*.csv)"));

    QFile f(fileName);
    if(f.open(QFile::WriteOnly))
    {
        QTextStream sy(&f);

        sy.setRealNumberNotation(QTextStream::FixedNotation);
        sy.setRealNumberPrecision(4);

        QTableWidgetItem *pItem;
        sy<< "Calculated values" << Qt::endl;
        sy<< "Peak" <<','
          <<"Transient time" <<','
               <<"Time to peak" <<','
                    <<"Rise" <<','
                        <<"Rise R^2" <<','
                            <<"Decay" <<','
                                <<"Decay R^2" <<','
                                    <<"Ascending slope" <<','
                                          <<"Descending slope" <<','
                                                <<"Time at half maximum" <<','
                                                    <<"Rise time" <<','
                                                        <<"Decay time" <<','
                                                          <<"RT50" <<','
                                                    <<"intensity lmin20" <<','
                                                        <<"intensity lmid50" <<','
                                                            <<"intensity lmax90" <<','
                                                                <<"intensity rmax90" <<','
                                                                    <<"intensity rmid50" <<','
                                                                        <<"intensity rmin20" <<','
                                                                           <<"int lmin" <<','
                                                                               <<"int maxabs" <<','
                                                                                    <<"int rmin" <<','
                                                                                       <<"time lmin20" <<','
                                                                                           <<"time lmid50" <<','
                                                                                               <<"time lmax90" <<','
                                                                                                   <<"time rmax90" <<','
                                                                                                       <<"time rmid50" <<','
                                                                                                           <<"time rmin20" <<','
                                                                                                              <<"time lmin" <<','
                                                                                                                  <<"time maxabs" <<','
                                                                                                                       <<"time rmin" <<','

                                                                                                    << Qt::endl;
        for(int r=0; r<ui->pTable->rowCount()-1; r++)
        {
           for (int c=0;c<ui->pTable->columnCount();++c)
           {
               pItem = ui->pTable->item(r, c);
               if (pItem) sy << pItem->text() << ',';
               else sy << " ,";
           }

               sy << data._valuesCa.at(data.transList.at(r)->lmin20).y() <<','
                  << data._valuesCa.at(data.transList.at(r)->lmid50).y() <<','
                  << data._valuesCa.at(data.transList.at(r)->lmax90).y() <<','
                  << data._valuesCa.at(data.transList.at(r)->rmax90).y() <<','
                  << data._valuesCa.at(data.transList.at(r)->rmid50).y() <<','
                  << data._valuesCa.at(data.transList.at(r)->rmin20).y() <<','
                  << data._valuesCa.at(data.transList.at(r)->lmin).y() <<','
                  << data._valuesCa.at(data.transList.at(r)->_maxabs).y() <<','
                  << data._valuesCa.at(data.transList.at(r)->rmin).y() <<','
                  << data._valuesCa.at(data.transList.at(r)->lmin20).x() <<','
                  << data._valuesCa.at(data.transList.at(r)->lmid50).x() <<','
                  << data._valuesCa.at(data.transList.at(r)->lmax90).x() <<','
                  << data._valuesCa.at(data.transList.at(r)->rmax90).x() <<','
                  << data._valuesCa.at(data.transList.at(r)->rmid50).x() <<','
                  << data._valuesCa.at(data.transList.at(r)->rmin20).x() <<','
                  << data._valuesCa.at(data.transList.at(r)->lmin).x() <<','
                  << data._valuesCa.at(data.transList.at(r)->_maxabs).x() <<','
                  << data._valuesCa.at(data.transList.at(r)->rmin).x() <<','<<Qt::endl;


        }
        sy << "Peak frequency" <<',' <<"Peak irregularity" <<Qt::endl;
        sy <<data.peakFreq <<','   <<data.peakIrregularity   <<Qt::endl;
        sy <<"Action Potential" <<',' <<"Start depolarisation" <<',' <<"End depolarisation" <<','<<"Time to depolarisation" <<',' <<Qt::endl;
        for(int e=0; e<data.transMEA.count(); e++)
        {
            sy <<e <<',' <<data.transMEA.at(e)->lMin20 <<',' <<data.transMEA.at(e)->rMin <<',' <<data.transMEA.at(e)->timeToDepol <<',' <<Qt::endl;

        }

        sy << "Ca time" <<',' <<"Ca intensity" <<',' <<"MEA time" << ',' <<"MEA intensity" <<',' << Qt::endl;
        for(long i=0; i<data._valuesMEA.count(); i++)
        {
            if (i<data._valuesCa.count())
                sy <<data._valuesCa.at(i).x() << ',' << data._valuesCa.at(i).y()<<',' <<data._valuesMEA.at(i).x() <<',' <<data._valuesMEA.at(i).y() << Qt::endl;
            else
                sy << " , ," <<data._valuesMEA.at(i).x() <<',' <<data._valuesMEA.at(i).y() << Qt::endl;
        }

    }
    f.close();
    }


}


