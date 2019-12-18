#include "OpenConfig.h"


OpenConfig::OpenConfig(QObject *parent) : QObject(parent)
{

}

//config.get<int>("Motor.baudRate")

QString OpenConfig::getConfig(QString path)
{
#ifdef Q_OS_LINUX
    ::mkdir("configs/",S_IRWXU);
#endif

}

string OpenConfig::getCurrentDir(string filename)
{
    boost::filesystem::path old_cpath = boost::filesystem::current_path(); //取得当前程序所在文件夹
    return (old_cpath/filename).string();
}

QString OpenConfig::getFile(const QString &filename)
{
    char* buff;
    int len = 1000;
    char buf[80]="0";
    char *p =::getcwd(buf,sizeof(buf));
    int err = linux_readFile(filename.toStdString().data(),&len,&buff);
    //cout<<err<<len<<filename.toStdString().data()<<endl;


    cout<<buf<<endl;
    return "sdsd";
}

int OpenConfig::linux_readFile(const char *filename, int *len, char **data)
{

   return 0;
}



