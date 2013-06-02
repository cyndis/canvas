#include "wacomthread.h"
#include <linux/input.h>
#include <fcntl.h>
#include <QDebug>
#include <QDir>
#include <stdlib.h>
#include <unistd.h>

bool isWacomDevice(QString path) {
    int fd = open(path.toUtf8(), O_RDONLY);
    if (fd == -1) {
        return false;
    }

    char buffer[32];
    ioctl(fd, EVIOCGNAME(32), buffer);
    close(fd);

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
    QThread(parent), _file(0), _quit{0,0}
{
    QString path = getWacomDevicePath();
    if (path.isNull()) {
        qFatal("Failed to find Wacom event file."
               "Make sure you have permissions to /dev/input/event* files.");
        exit(1);
    }
    _file = open(path.toUtf8(), O_RDONLY);
    if (!_file)
        qFatal("Failed to open event file");
    pipe(_quit);
}

WacomThread::~WacomThread()
{

    write(_quit[1], ".", 1);
    QThread::wait();
    if (_file)
        close(_file);
    if (_quit[0])
        close(_quit[0]);
    if (_quit[1])
        close(_quit[1]);
}

#define TYPE_ABS 3
#define TYPE_BTN 1

#define CODE_BTN1 331
#define CODE_BTN2 332
#define CODE_DETECTED 320

#define CODE_X 0
#define CODE_Y 1
#define CODE_PRESSURE 24

void WacomThread::run()
{
    int x=0, y=0, pressure=0;
    int evs=0;
    struct input_event ev;
    fd_set read_fds;

    int max_fd = (_file > _quit[0]) ? _file : _quit[0];

    for (;;) {
        FD_ZERO(&read_fds);
        FD_SET(_file, &read_fds);
        FD_SET(_quit[0], &read_fds);
        if (select(max_fd+1, &read_fds, NULL, NULL, NULL) < 0)
            break;
        if (FD_ISSET(_quit[0], &read_fds))
            break;
        read(_file, &ev, sizeof(struct input_event));

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
            case CODE_BTN1: emit onButtonChanged(1, ev.value == 1); break;
            case CODE_BTN2: emit onButtonChanged(2, ev.value == 1); break;
            }
        }
    }
}
