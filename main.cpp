#include "rdrmaindlg.h"

#include <QApplication>
#include <QLocale>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    rdrMainDlg w;
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedKingdom));
    a.setPalette(w.palette());
    a.setStyle("fusion");
    w.show();
    return a.exec();
}
