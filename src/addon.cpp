#include "addon.h"
#include <QSettings>
#include <QtXml>

addon::addon(QObject *parent)
    : QObject{parent}
{

}
#include <QDir>
bool copyFile(QString srcPath, QString dstPath, bool coverFileIfExist)
{
    srcPath.replace("/", "\\");
    dstPath.replace("/", "\\");
    if (srcPath == dstPath) {
        return true;
    }

    if (!QFile::exists(srcPath)) {
        return false;
    }

    if (QFile::exists(dstPath)) {
        if (coverFileIfExist) {
            QFile::remove(dstPath);
        }
    }

    if (!QFile::copy(srcPath, dstPath)){
        return false;
    }
    return true;
}
bool copyDir(const QString &source,const QString &destination, bool override)
{
    QDir directory(source);
    if (!directory.exists())
    {
        qWarning()<<"Folder "+source+" dosen\'t exist";
        return false;
    }


    QString srcPath = QDir::toNativeSeparators(source);
    if (!srcPath.endsWith(QDir::separator()))
        srcPath += QDir::separator();
    QString dstPath = QDir::toNativeSeparators(destination);
    if (!dstPath.endsWith(QDir::separator()))
        dstPath += QDir::separator();
    bool error = false;
    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    for (QStringList::size_type i=0; i != fileNames.size(); ++i)
    {
        QString fileName = fileNames.at(i);
        QString srcFilePath = srcPath + fileName;
        QString dstFilePath = dstPath + fileName;
        QFileInfo fileInfo(srcFilePath);
        if (fileInfo.isFile() || fileInfo.isSymLink())
        {
            if (override)
            {
                QFile::setPermissions(dstFilePath, QFile::WriteOwner);
            }
            copyFile(srcFilePath,dstFilePath,1);
        }
        else if (fileInfo.isDir())
        {
            QDir dstDir(dstFilePath);
            dstDir.mkpath(dstFilePath);
            if (!copyDir(srcFilePath, dstFilePath, override))
            {
                error = true;
            }
        }
    }
    return error;
}
QString MainBinDir = "";
void addon::BinDir(QString bindir){
    MainBinDir = bindir;
    qDebug() << MainBinDir;
}
QString searchAddonFile()
{
    QString binDir=MainBinDir;
    QStringList searchList;
    searchList.append(binDir+"/addon");
    searchList.append(binDir+"/addon");
    searchList.append(binDir+"/../etc/addon");
    searchList.append(binDir+"/../QtBigScreen/etc/addon"); // for development with shadow build (Linux)
    searchList.append(binDir+"/../../QtBigScreen/etc/addon"); // for development with shadow build (Windows)
    searchList.append(QDir::rootPath()+"etc/opt/addon");
    searchList.append(QDir::rootPath()+"etc/addon");
    QString dir;
    foreach (dir, searchList)
    {
        QDir file(dir);
        if (file.exists())
        {
            QString fileName=file.path();
            return fileName;
        }
    }

    // not found
    QString dir1;
    foreach (dir1, searchList)
    {
        qWarning("%s not found",qPrintable(dir1));
    }
    qFatal("Cannot find addon %s",qPrintable(dir1));
    return nullptr;
}
QString addonfolder;
QStringList FindFile(const QString& _filePath)
{
    QDir dir(_filePath);
    QStringList folderList = {};
    if (!dir.exists()) {
        return {};
    }

  //取到所有的文件和文件名，但是去掉.和..的文件夹（这是QT默认有的）
    dir.setFilter(QDir::Dirs|QDir::NoDotAndDotDot);

    //文件夹优先
    dir.setSorting(QDir::DirsFirst);

    //转化成一个list
    QFileInfoList list = dir.entryInfoList();
    int i=0;

    //递归算法的核心部分
    do{
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.isDir()){
            folderList.append(fileInfo.filePath());
        }
        i++;
    } while(i < list.size());

    return folderList;
}
bool XCopyFile(QString source, QString destination)
{
    QDir source_dir(source);
    source_dir.mkpath(source);
    QString str="xcopy \""+source+"\" \""+destination+"\" /y /E /i /q &";
    qDebug() << str;
    system(str.toStdString().c_str());
    return false;
}
int TableTr=0;
int LastTr=0;
bool CopyAddons(QString configDir){
    QString newDir = MainBinDir+"/real_docroot/addon_"+QDir(configDir).dirName();
    XCopyFile(configDir,newDir);
    QDir::setCurrent(newDir);
    QString config = newDir+"/config.ini";
    if(!(QFile(config).exists())){
        qWarning() << "Can\'t find "+config;
        return 1;
    }
    QSettings ini = QSettings(config, QSettings::IniFormat);
    //html
    QString html = ini.value("HTML/file","").toString();
    //change index
    bool hascard = 0;
    QString body_title="";
    QString body_pic="";
    QString body_text="";
    if(!(ini.value("ChangeIndex/body_title",0) == 0)){
        hascard = 1;
        body_title=ini.value("ChangeIndex/body_title").toByteArray();
        body_pic=ini.value("ChangeIndex/body_pic").toByteArray();
        body_text=ini.value("ChangeIndex/body_text").toByteArray();
    }
    if(ini.value("Exec/exec") != NULL){
        QString program = ini.value("Exec/exec").toString();
        QProcess * mProcess = new QProcess;
        mProcess->setProcessChannelMode(QProcess::MergedChannels);
        int background = 0;
        if(ini.value("Exec/background") == "1" ||ini.value("Exec/background") == "true"){
            background = 1;
        }
        if(ini.value("Exec/block") == "1" ||ini.value("Exec/block") == "true"){
            if(background == 1){
                mProcess->start(program);
                mProcess->waitForFinished();
            }
            else{
                mProcess->startDetached(program);
                mProcess->waitForFinished();
            }
        }
        else{
            if(background == 1){
                mProcess->start(program);
            }
            else{
                mProcess->startDetached(program);
            }
        }
    }
    bool isNavbar = 0;
    if(ini.value("navbar/name") != NULL){
        QString navbar_name = ini.value("navbar/name").toString();
        isNavbar = 1;
    }
    ini.beginGroup("Setting");
    QStringList SettingskeyList = ini.allKeys();
    ini.endGroup();
    //end of read
    QFile js_file = addonfolder+"\\..\\real_docroot\\loadAddon.js";
    if(!js_file.open(QFile::WriteOnly | QFile::Append)){
         qFatal("Can't open js file "+js_file.fileName().toLocal8Bit());
         return 1;
    }
    QTextStream outs(&js_file);
    outs << "//This file is auto generate by QtBigScreen." << Qt::endl;
    outs << "//Your changes will NOT be saved!!!" << Qt::endl;
    if(hascard == 1){
        qDebug() << body_title;
        QString img = ("/addon_"+QDir(configDir).dirName()+"/"+body_pic);
        if(TableTr % 3 == 0 || TableTr == 0){
            outs << ("var tr_"+std::to_string(TableTr)+" = document.createElement('tr');").c_str() << Qt::endl;
            outs << ("var td_"+std::to_string(TableTr)+" = document.createElement('td');").c_str() << Qt::endl;
            outs << ("var div_card_"+std::to_string(TableTr)+" = document.createElement('div');").c_str() << Qt::endl;
            outs << ("div_card_"+std::to_string(TableTr)+".className = 'card borderless shadow blur';").c_str() << Qt::endl;
            outs << ("div_card_"+std::to_string(TableTr)+".onmouseover = function () { this.className = 'card border-primary shadow-lg blur' };").c_str() << Qt::endl;
            outs << ("div_card_"+std::to_string(TableTr)+".onmouseout = function () { this.className = 'card borderless shadow blur' };").c_str() << Qt::endl;
            outs << ("div_card_"+std::to_string(TableTr)+".onclick = function () { window.location.href = '"+("/addon_"+QDir(configDir).dirName()+"/"+html).toStdString()+"'; };").c_str() << Qt::endl;
            outs << ("var div_card_img_"+std::to_string(TableTr)+" = document.createElement('div');").c_str() << Qt::endl;
            outs << ("div_card_img_"+std::to_string(TableTr)+".className = 'card-img-top borderless rounded-lg zoomImage';").c_str() << Qt::endl;
            outs << ("div_card_img_"+std::to_string(TableTr)+".style = `background-image: url('"+img.toStdString()+"')`;").c_str() << Qt::endl;
            outs << ("var div_card_body_"+std::to_string(TableTr)+" = document.createElement('div');").c_str() << Qt::endl;
            outs << ("div_card_body_"+std::to_string(TableTr)+".className = 'card-body';").c_str() << Qt::endl;
            outs << ("var card_title_"+std::to_string(TableTr)+" = document.createElement('h5');").c_str() << Qt::endl;
            outs << ("card_title_"+std::to_string(TableTr)+".className = 'card-title';").c_str() << Qt::endl;
            outs << ("card_title_"+std::to_string(TableTr)+".style = 'white-space: nowrap;';").c_str() << Qt::endl;
            outs << ("card_title_"+std::to_string(TableTr)+".innerText = '"+body_title.toStdString()+"';").c_str() << Qt::endl;
            outs << ("var card_text_"+std::to_string(TableTr)+" = document.createElement('p');").c_str() << Qt::endl;
            outs << ("card_text_"+std::to_string(TableTr)+".className = 'card-text';").c_str() << Qt::endl;
            outs << ("card_text_"+std::to_string(TableTr)+".style = 'overflow-x:auto;white-space: nowrap;';").c_str() << Qt::endl;
            outs << ("card_text_"+std::to_string(TableTr)+".appendChild(document.createTextNode('"+body_text.toStdString()+"'));").c_str() << Qt::endl;
            outs << ("tr_"+std::to_string(TableTr)+".appendChild(td_"+std::to_string(TableTr)+");").c_str() << Qt::endl;
            outs << ("td_"+std::to_string(TableTr)+".appendChild(div_card_"+std::to_string(TableTr)+");").c_str() << Qt::endl;
            outs << ("div_card_"+std::to_string(TableTr)+".appendChild(div_card_img_"+std::to_string(TableTr)+");").c_str() << Qt::endl;
            outs << ("div_card_"+std::to_string(TableTr)+".appendChild(div_card_body_"+std::to_string(TableTr)+");").c_str() << Qt::endl;
            outs << ("div_card_body_"+std::to_string(TableTr)+".appendChild(card_title_"+std::to_string(TableTr)+");").c_str() << Qt::endl;
            outs << ("div_card_body_"+std::to_string(TableTr)+".appendChild(card_text_"+std::to_string(TableTr)+");").c_str() << Qt::endl;
            outs << ("document.getElementsByTagName('table')[0].appendChild(tr_"+std::to_string(TableTr)+");").c_str() << Qt::endl;
            LastTr = TableTr;
        }
        else{
            outs << ("var tr_"+std::to_string(TableTr)+" = document.createElement('tr');").c_str() << Qt::endl;
            outs << ("var td_"+std::to_string(TableTr)+" = document.createElement('td');").c_str() << Qt::endl;
            outs << ("var div_card_"+std::to_string(TableTr)+" = document.createElement('div');").c_str() << Qt::endl;
            outs << ("div_card_"+std::to_string(TableTr)+".className = 'card borderless shadow blur';").c_str() << Qt::endl;
            outs << ("div_card_"+std::to_string(TableTr)+".onmouseover = function () { this.className = 'card border-primary shadow-lg blur' };").c_str() << Qt::endl;
            outs << ("div_card_"+std::to_string(TableTr)+".onmouseout = function () { this.className = 'card borderless shadow blur' };").c_str() << Qt::endl;
            outs << ("div_card_"+std::to_string(TableTr)+".onclick = function () { window.location.href = '"+("/addon_"+QDir(configDir).dirName()+"/"+html).toStdString()+"'; };").c_str() << Qt::endl;
            outs << ("var div_card_img_"+std::to_string(TableTr)+" = document.createElement('div');").c_str() << Qt::endl;
            outs << ("div_card_img_"+std::to_string(TableTr)+".className = 'card-img-top borderless rounded-lg zoomImage';").c_str() << Qt::endl;
            outs << ("div_card_img_"+std::to_string(TableTr)+".style = `background-image: url('"+img.toStdString()+"')`;").c_str() << Qt::endl;
            outs << ("var div_card_body_"+std::to_string(TableTr)+" = document.createElement('div');").c_str() << Qt::endl;
            outs << ("div_card_body_"+std::to_string(TableTr)+".className = 'card-body';").c_str() << Qt::endl;
            outs << ("var card_title_"+std::to_string(TableTr)+" = document.createElement('h5');").c_str() << Qt::endl;
            outs << ("card_title_"+std::to_string(TableTr)+".className = 'card-title';").c_str() << Qt::endl;
            outs << ("card_title_"+std::to_string(TableTr)+".style = 'white-space: nowrap;';").c_str() << Qt::endl;
            outs << ("card_title_"+std::to_string(TableTr)+".innerText = '"+body_title.toStdString()+"';").c_str() << Qt::endl;
            outs << ("var card_text_"+std::to_string(TableTr)+" = document.createElement('p');").c_str() << Qt::endl;
            outs << ("card_text_"+std::to_string(TableTr)+".className = 'card-text';").c_str() << Qt::endl;
            outs << ("card_text_"+std::to_string(TableTr)+".style = 'overflow-x:auto;white-space: nowrap;';").c_str() << Qt::endl;
            outs << ("card_text_"+std::to_string(TableTr)+".appendChild(document.createTextNode('"+body_text.toStdString()+"'));").c_str() << Qt::endl;
            outs << ("tr_"+std::to_string(LastTr)+".appendChild(td_"+std::to_string(TableTr)+");").c_str() << Qt::endl;
            outs << ("td_"+std::to_string(TableTr)+".appendChild(div_card_"+std::to_string(TableTr)+");").c_str() << Qt::endl;
            outs << ("div_card_"+std::to_string(TableTr)+".appendChild(div_card_img_"+std::to_string(TableTr)+");").c_str() << Qt::endl;
            outs << ("div_card_"+std::to_string(TableTr)+".appendChild(div_card_body_"+std::to_string(TableTr)+");").c_str() << Qt::endl;
            outs << ("div_card_body_"+std::to_string(TableTr)+".appendChild(card_title_"+std::to_string(TableTr)+");").c_str() << Qt::endl;
            outs << ("div_card_body_"+std::to_string(TableTr)+".appendChild(card_text_"+std::to_string(TableTr)+");").c_str() << Qt::endl;
        }
        TableTr++;
    }
    if(isNavbar){

    }
    outs << Qt::flush;
    js_file.close();
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    return 0;
}
bool addon::init()
{
    addonfolder = searchAddonFile();
    QStringList addons = FindFile(addonfolder);
    qDebug() << "Using addon(s):" << addons;
    QString addon;
    foreach(addon,addons){
        CopyAddons(addon);
        qDebug() << "Copy addons "+addon;
    }
    return 0;
}
