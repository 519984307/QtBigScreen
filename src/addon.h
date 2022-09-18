#ifndef ADDON_H
#define ADDON_H

#include <QObject>
#include <QCoreApplication>

class addon : public QObject
{
    Q_OBJECT
public:
    explicit addon(QObject *parent = nullptr);
public slots:
    bool init();
    void BinDir(QString bindir);
signals:

};

#endif // ADDON_H
