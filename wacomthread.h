#ifndef WACOMTHREAD_H
#define WACOMTHREAD_H

#include <QThread>
#include <stdio.h>

class WacomThread : public QThread
{
    Q_OBJECT
public:
    explicit WacomThread(QObject *parent = 0);
    ~WacomThread();

protected:
    void run();
    
signals:
    void onStateChanged(float x, float y, float pressure);
    void onButtonChanged(int button, bool state);
    
public slots:

private:
    int _file;
    int _quit[2];
    
};

#endif // WACOMTHREAD_H
