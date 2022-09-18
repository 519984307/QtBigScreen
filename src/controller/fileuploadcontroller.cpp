/**
  @file
  @author Stefan Frings
*/

#include "fileuploadcontroller.h"
#include <QCoreApplication>
#include <QDir>
#include <QTextCodec>
#include <fstream>
#include <iostream>
#include <string.h>
using namespace std;
int checkNumber(int num){
    int weishu = 0;
    for (int i = 1, num1 = num; num / i; i = (i * 10)){
        num1 = num1 / 10;
        weishu++;
    }
    return weishu;
}
void changeJSFile()
{
    QString binDir=QCoreApplication::applicationDirPath();
    QString fileName("time.js");
    QString dir;


    QStringList searchList;
    searchList.append(binDir);
    searchList.append(binDir+"/etc/source/js");
    searchList.append(binDir+"/../etc/source/js");
    searchList.append(binDir+"/../QtBigScreen/etc/source/js");
    searchList.append(binDir+"\\..\\..\\QtBigScreen\\etc\\source\\js");
    searchList.append(QDir::rootPath()+"etc/source/js");
    searchList.append(QDir::rootPath()+"source/js");

    foreach (dir, searchList)
    {
        QFile file(dir+"/"+fileName);
        if (file.exists())
        {
            std::ofstream ofs;
            string t;
            fileName=QDir(file.fileName()).canonicalPath();
            qDebug("Using js file %s",qPrintable(fileName));
            ifstream ifs;
            ifs.open("./temptime.txt", ios::in);
            if (!ifs.is_open())
            {
                cout << "Can't open temp text" << endl;
                goto endf;
            }
            char c;
            while ((c=ifs.get())!=EOF)
            {
                t = t + c;
            }
            ifs.close();
            int t1 = 1;
            string tt;
            string t2;
            string t3;
            ofs.open(fileName.toStdString(), ios::out | ios::trunc);
            ofs << "//This file is auto generate by QtBigScreen." << endl;
            ofs << "//Your changes will NOT be saved!!!" << endl;
            ofs << "var classtime = \"";
            while (1) {
                tt = to_string(t1);
                int a = t.find(tt+"(");
                int b = t.find(")"+tt);
                if (b != -1 && a != -1) {
                    t2 = t.substr(a + 1 + checkNumber(t1), b - a - 1 - checkNumber(t1));
                    t3 = t2.substr(0, t2.find("+"));
                    ofs << t3.substr(0, t3.find(":")) << ":" << t3.substr(t3.find(":") + 1) << "," << t2.substr(t2.find("+") + 1) << ".";
                }
                else {
                    ofs << "\";" << endl;
                    ofs.close();
                    break;
                }
                t1++;
            }
        }
        endf:;
    }
}
FileUploadController::FileUploadController()
{}
QString filepathbyteArrayToUnicode(const QByteArray &array, int &CodeSet)
{
    CodeSet = 0;
    QTextCodec::ConverterState state;
    QString text = QTextCodec::codecForName("UTF-8")->toUnicode(array.constData(), array.size(), &state);
    if (state.invalidChars > 0)
    {
        CodeSet = 1;
        text = QTextCodec::codecForName("GBK")->toUnicode(array);
    }
    return text;
}
void FileUploadController::service(HttpRequest& request, HttpResponse& response)
{

    if (request.getParameter("action")=="show")
    {
        response.setHeader("Content-Type", "text/txt");
        QTemporaryFile* file=request.getUploadedFile("file1");
        if (file)
        {
            while (!file->atEnd() && !file->error())
            {
                QByteArray buffer=file->read(65536);
                int a = 0;
                QString te = filepathbyteArrayToUnicode(buffer,a);
                std::string t = te.toStdString();
                std::ofstream ofs;
                ofs.open("./temptime.txt",std::ios::out | std::ios::trunc);
                ofs << t;
                ofs.close();
                changeJSFile();
                response.redirect("/");
            }
        }
        else
        {
            response.write("upload failed");
        }
    }

    else
    {
        response.redirect("/index.html");
    }
}
void FileUploadController::time(){
    changeJSFile();
}
