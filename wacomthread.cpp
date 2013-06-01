#include "wacomthread.h"
#include <linux/input.h>
#include <QDebug>

WacomThread::WacomThread(QObject *parent) :
    QThread(parent), _file(0)
{
    _file = fopen("/dev/input/event22", "r");
    if (!_file)
        qFatal("Failed to open event file");
}

#define TYPE_ABS 3

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
        if (ev.type != TYPE_ABS)
            continue;

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
    }
}
