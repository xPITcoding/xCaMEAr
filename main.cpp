#include "rdrmaindlg.h"

#include <QApplication>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    rdrMainDlg w;
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedKingdom));
    a.setPalette(w.palette());
    w.show();
    return a.exec();
}
