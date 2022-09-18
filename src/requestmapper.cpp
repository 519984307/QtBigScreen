#include <QCoreApplication>
#include <iostream>
#include "global.h"
#include "requestmapper.h"
#include "filelogger.h"
#include "Toast.h"
#include "staticfilecontroller.h"
#include "controller/fileuploadcontroller.h"
#include <Windows.h>
#include <QDate>
#include <string.h>
#include <QDir>
#include <QProcess>
using namespace std;
RequestMapper::RequestMapper(QObject* parent)
    :HttpRequestHandler(parent)
{
    qDebug("RequestMapper: created");
    FileUploadController().time();
}
RequestMapper::~RequestMapper()
{
    qDebug("RequestMapper: deleted");
}

void RequestMapper::service(HttpRequest& request, HttpResponse& response)
{
    QByteArray path=request.getPath();
    qDebug("RequestMapper: path=%s",path.data());

    // For the following pathes, each request gets its own new instance of the related controller.
    if (path.startsWith("/run"))
    {
        qDebug() << "run path with:" << path.mid(4);
        system(path.mid(4));
    }
    else if (path.startsWith("/pptctrl"))
    {
        if (path.mid(9) == "down"){
            keybd_event(VK_RIGHT, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
            keybd_event(VK_RIGHT, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
        }
        else if (path.mid(9) == "up") {
            keybd_event(VK_LEFT, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
            keybd_event(VK_LEFT, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
        }
        else if (path.mid(9) == "in"){
            keybd_event(VK_F5, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
            keybd_event(VK_F5, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
        }
        else if (path.mid(9) == "out"){
            keybd_event(VK_ESCAPE, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
            keybd_event(VK_ESCAPE, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
        }
        else if (path.mid(9) == "blk"){
            keybd_event('B', NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
            keybd_event('B', NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
        }
        else if (path.mid(9) == "all"){
            keybd_event('G', NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
            keybd_event('G', NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
        }
        else if(path.mid(9).startsWith("kbd")){
            if(path.mid(12) == "bk"){
                keybd_event(VK_BACK, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_BACK, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == "et"){
                keybd_event(VK_RETURN, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_RETURN, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == "sf"){
                keybd_event(VK_SHIFT, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_SHIFT, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12).startsWith("nu")){
                int a = path.mid(14).toInt()+48;
                keybd_event(a, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(a, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == ","){
                keybd_event(VK_OEM_COMMA, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_OEM_COMMA, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == "."){
                keybd_event(VK_OEM_PERIOD, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_OEM_PERIOD, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == "-"){
                keybd_event(VK_OEM_MINUS, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_OEM_MINUS, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == "~"){
                keybd_event(VK_OEM_3, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_OEM_3, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == "="){
                keybd_event(VK_OEM_PLUS, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_OEM_PLUS, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == "'"){
                keybd_event(VK_OEM_7, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_OEM_7, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == " "){
                keybd_event(VK_OEM_7, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_OEM_7, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == "["){
                keybd_event(VK_OEM_4, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_OEM_4, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == "]"){
                keybd_event(VK_OEM_6, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_OEM_6, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == "xi"){
                keybd_event(VK_OEM_5, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_OEM_5, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == "xg"){
                keybd_event(VK_OEM_2, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_OEM_2, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == "up"){
                keybd_event(VK_UP, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_UP, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == "lf"){
                keybd_event(VK_LEFT, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_LEFT, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == "dn"){
                keybd_event(VK_DOWN, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_DOWN, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == "ri"){
                keybd_event(VK_RIGHT, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_RIGHT, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12) == "ko"){
                keybd_event(VK_SPACE, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_SPACE, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12).startsWith("ctrl")){
                keybd_event(VK_CONTROL, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_CONTROL, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12).startsWith(";")){
                keybd_event(VK_OEM_1, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(VK_OEM_1, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else if(path.mid(12).startsWith("fk")){
                int f = path.mid(14).toInt()+111;
                keybd_event(f, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(f, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
            else{
                int b = path.mid(12).toStdString()[0];
                b = b-32;
                keybd_event(b, NULL, KEYEVENTF_EXTENDEDKEY | 0, NULL);
                keybd_event(b, NULL, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, NULL);
            }
        }

    }
    else if (path.startsWith("/action_time"))
    {
        FileUploadController().service(request, response);
    }
    else if (path.startsWith("/stop"))
    {
        if(path.mid(5) == QByteArray::number(QDate::currentDate().day())){
            int a = GetCurrentProcessId();
            system("TASKKILL /F /PID "+QByteArray::number(a)+" /T");
        }
    }
    else if(path == ("/vnc")){
        #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            QMultiMapIterator<QByteArray,QByteArray> i(request.getHeaderMap());
        #else
            QMapIterator<QByteArray,QByteArray> i(request.getHeaderMap());
        #endif
        QByteArray q;
        while (i.hasNext())
        {
            i.next();
            if(i.key() == "host"){
                q = i.value();
                break;
            }
        }
        response.redirect("http://"+q.split(':')[0]+":9006/");
    }
    // All other pathes are mapped to the static file controller.
    // In this case, a single instance is used for multiple requests.
    else
    {
        staticFileController->service(request, response);
    }

    qDebug("RequestMapper: finished request");

    // Clear the log buffer
    if (logger)
    {
       logger->clear();
    }
}
