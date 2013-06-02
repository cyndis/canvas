#include "wacomthread.h"
#include <linux/input.h>
#include <fcntl.h>
#include <QDebug>
#include <QDir>
#include <stdlib.h>

bool isWacomDevice(QString path) {
    int fd = open(path.toUtf8(), O_RDONLY);
    if (fd == -1) {
        return false;
    }

    char buffer[32];
    ioctl(fd, EVIOCGNAME(32), buffer);
    QString name(buffer);
    if (name.contains("Wacom") && name.contains("Pen"))
        return true;
    else
        return false;
}

QString getWacomDevicePath() {
    QDir dir("/dev/input");
    QStringList devs = dir.entryList(QStringList() << "event*",
                                     QDir::Dirs | QDir::System);
    for (const QString &file : devs) {
        QString path = QString("/dev/input/") + file;
        if (isWacomDevice(path))
            return path;
    }

    return QString();
}

WacomThread::WacomThread(QObject *parent) :
    QThread(parent), _file(0)
{
    QString path = getWacomDevicePath();
    if (path.isNull()) {
        qFatal("Failed to find Wacom event file."
               "Make sure you have permissions to /dev/input/event* files.");
        exit(1);
    }
    _file = fopen(path.toUtf8(), "r");
    if (!_file)
        qFatal("Failed to open event file");
}

#define TYPE_ABS 3
#define TYPE_BTN 1

#define CODE_BTN1 331

#define CODE_X 0
#define CODE_Y 1
#define CODE_PRESSURE 24

void WacomThread::run()
{
    int x=0, y=0, pressure=0;
    int evs=0;
    struct input_event ev;
    for (;;) {
        fread(&ev, sizeof(struct input_event), 1, _file);

        if (ev.type == TYPE_ABS) {
            switch (ev.code) {
            case CODE_X: x = ev.value; evs += 1; break;
            case CODE_Y: y = ev.value; evs += 1; break;
            case CODE_PRESSURE: pressure = ev.value; break;
            default: continue;
            }

            if (evs == 2) {
                evs = 0;
                float mx = (float)x / (14720 * 1.0);
                float my = (float)y / (9200 * 1.0);
                float mp = (float)pressure / 1023;
                emit onStateChanged(mx, my, mp);
            }
        } else if (ev.type == TYPE_BTN) {
            switch (ev.code) {
            case CODE_BTN1: emit onButtonChanged(0, ev.value == 1); break;
            }
        }
    }
}
