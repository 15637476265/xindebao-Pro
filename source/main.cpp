#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QStyleFactory>
#include <QTextCodec>



#define USE_QSS 1


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFont font;
    font.setPointSize(12);
    font.setFamily("Alibaba Sans");

//    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//情况2
//    QTextCodec::setCodecForTr(codec);
//    QTextCodec::setCodecForLocale(codec);
//    QTextCodec::setCodecForCStrings(codec);


    //QApplication::setStyle(QStyleFactory::create("macintosh"));
    QApplication::setStyle(QStyleFactory::create("material"));
    qApp->setFont(font);

    #if USE_QSS
        QFile f(":qss/style.qss");
        if (!f.exists())
        {
            printf("Unable to set stylesheet, file not found\n");
        }
        else
        {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            qApp->setStyleSheet(ts.readAll());
        }
    #endif
        MainWindow w;

        //w.show();
        w.showFullScreen();

    return a.exec();
}
