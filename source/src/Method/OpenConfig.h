#ifndef OPENCONFIG_H
#define OPENCONFIG_H

#include <QObject>
#include <QFile>
#include "utils.h"
#ifdef Q_OS_LINUX
    #include <sys/stat.h>
    #include <errno.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <fcntl.h>
    #include <stdlib.h>
#endif

#include <stdio.h>

#include <mutex>
#include <memory>


#include "boost/filesystem/path.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/filesystem/operations.hpp"
#include <boost/filesystem.hpp>

#include <boost/typeof/typeof.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/asio.hpp>

#include <boost/system/system_error.hpp>


using namespace boost::property_tree;

using namespace std ;

class OpenConfig : public QObject
{
    Q_OBJECT
public:
    explicit OpenConfig(QObject *parent = nullptr);
    std::recursive_mutex _mtx_ptr;


    template<typename T>
    static T get(const std::string &filename,const std::string &index){
        T ret ;
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(filename,pt);
        try {
            ret =pt.get<T>(index);
        } catch (const boost::exception &e) {
            cout<<"Error Read!"<<endl;

        }catch(const boost::system::system_error &e){
            std::cout <<"system:"<< e.what() << std::endl ;
        }
        return ret;
    }
    template<typename T>
    static int write(string filename,string index,T value){
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(filename,pt);

        int ret = 0;

        try {
            pt.put<T>(index,value);

        } catch (...)  {
            ret = -1;
            printf("Config write failed!\n");
        }

        std::ostringstream jsonOutputStream;
        boost::property_tree::json_parser::write_json(jsonOutputStream, pt,false);

        std::ofstream tmpReport(filename);
        if(!tmpReport){
            ret = -2;
            std::cout << filename <<"文件不能打开!"<<std::endl;
        }else{
            tmpReport<<jsonOutputStream.str();
        }
        tmpReport.close();

        return ret;
    }

    static QString getConfig(QString path);
    static string getCurrentDir(string filename);

    static  QString getFile( const QString &filename);
    static int linux_readFile(const char *filename, int *len, char **data);

signals:

public slots:
};

#endif // OPENCONFIG_H
