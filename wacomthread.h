#ifndef WACOMTHREAD_H
#define WACOMTHREAD_H

#include <QThread>
#include <stdio.h>

class WacomThread : public QThread
{
    Q_OBJECT
public:
    explicit WacomThread(QObject *parent = 0);

protected:
    void run();
    
signals:
    void onStateChanged(float x, float y, float pressure);
    
public slots:

private:
    FILE* _file;
    
};

#endif // WACOMTHREAD_H