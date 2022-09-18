#include <QApplication>
#include <QDir>
#include "global.h"
#include "httplistener.h"
#include "requestmapper.h"
#include "icon.h"
#include "Toast.h"
#include "addon.h"
#include <QProcess>
#include <Windows.h>
#include <QFile>
using namespace stefanfrings;
QProcess myPro;
/** Search the configuration file */
QString dir1;
QString searchConfigFile()
{
    QString binDir=QCoreApplication::applicationDirPath();
    QString fileName("Settings.ini");
    QStringList searchList;
    searchList.append(binDir);
    searchList.append(binDir+"/etc");
    searchList.append(binDir+"/../etc");
    searchList.append(binDir+"/../QtBigScreen/etc"); // for development with shadow build (Linux)
    searchList.append(binDir+"/../../QtBigScreen/etc"); // for development with shadow build (Windows)
    searchList.append(QDir::rootPath()+"etc/opt");
    searchList.append(QDir::rootPath()+"etc");

    foreach (dir1, searchList)
    {
        QFile file(dir1+"/"+fileName);
        if (file.exists())
        {
            fileName=QDir(file.fileName()).canonicalPath();
            qDebug("Using config file %s",qPrintable(fileName));
            return fileName;
        }
    }

    // not found
    foreach (QString dir, searchList)
    {
        qWarning("%s/%s not found",qPrintable(dir),qPrintable(fileName));
    }
    qFatal("Cannot find config file %s",qPrintable(fileName));
    return nullptr;
}
QString fileName("jsmpeg-vnc.exe");
void GetVNC()
{
    QString binDir=QCoreApplication::applicationDirPath();
    QString dir;


    QStringList searchList;
    searchList.append(binDir);
    searchList.append(binDir+"/etc/vnc");
    searchList.append("./etc/vnc");
    searchList.append(binDir+"/../etc/vnc");
    searchList.append(binDir+"/../QtBigScreen/etc/vnc");
    searchList.append(binDir+"\\..\\..\\QtBigScreen\\etc\\vnc");
    searchList.append(QDir::rootPath()+"etc/vnc");
    searchList.append(QDir::rootPath()+"/vnc");

    foreach (dir, searchList)
    {
        QFile file(dir+"/"+fileName);
        if (file.exists())
        {
            std::string t;
            int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
            int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
            QString s = (std::to_string(nScreenWidth/3*2) + "x" + std::to_string(nScreenHeight/3*2)).c_str();
            QString st = (std::to_string(nScreenWidth)+","+std::to_string(nScreenHeight)).c_str();
            QString program = "./"+fileName+" -b 1000 -p 9006 -f 30 -s "+s+" -c 0,0,"+st+" \"desktop\"";
            qDebug("Using vnc %s",qPrintable(program));
            QDir::setCurrent(dir);
            myPro.start(program);
            QDir::setCurrent(QApplication::applicationDirPath());
            break;
        }
    }
}

bool xCopyFile(QString source, QString destination)
{
    QDir source_dir(source);
    source_dir.mkpath(source);
    QString str="xcopy \""+source+"\" \""+destination+"\" /y /E /i /q &";
    qDebug() << str;
    system(str.toStdString().c_str());
    return false;
}
bool initRealDocroot(){
    QDir Real_docroot = dir1+"\\real_docroot";
    if(Real_docroot.exists()){
        Real_docroot.removeRecursively();
    }
    Real_docroot.mkdir(dir1+"\\real_docroot");
    xCopyFile(dir1+"\\source",dir1+"\\real_docroot");
    return 0;
}
/**
  Entry point of the program.
*/
int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    QApplication::setQuitOnLastWindowClosed(false);
    app.setApplicationName("Server");
    // Find the configuration file
    QString configFileName=searchConfigFile();

    // Configure logging into a file
    QSettings* logSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    logSettings->beginGroup("logging");
    FileLogger* logger=new FileLogger(logSettings,10000,&app);
    logger->installMsgHandler();

    // Configure template loader and cache
    QSettings* templateSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    templateSettings->beginGroup("templates");
    templateCache=new TemplateCache(templateSettings,&app);

    // Configure session store
    QSettings* sessionSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    sessionSettings->beginGroup("sessions");
    sessionStore=new HttpSessionStore(sessionSettings,&app);

    icon i;
    i.showicon();

//    GetVNC();
    initRealDocroot();
    addon ad;
    ad.BinDir(dir1);
    ad.init();

    // Configure static file controller
    QSettings* fileSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    fileSettings->beginGroup("docroot");
    staticFileController=new StaticFileController(fileSettings,&app);

    // Configure and start the TCP listener
    QSettings* listenerSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    listenerSettings->beginGroup("listener");
    new HttpListener(listenerSettings,new RequestMapper(&app),&app);
    qWarning("Application has started");
    app.exec();
    qWarning("Application has stopped");
}

