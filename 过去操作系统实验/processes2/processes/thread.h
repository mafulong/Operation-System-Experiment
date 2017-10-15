#ifndef THREAD_H
#define THREAD_H

#include <QThread>

#ifndef MAINWINDOW_H
#include "mainwindow.h"
#endif

#ifndef PCB_H
#include "pcb.h"
#endif

class Thread_cl : public QThread
{
    Q_OBJECT
public:
    Thread_cl(class MainWindow* win);
    void stop();
    bool isStopped();
    bool isStart();
    void start();
    void pause();
    void continueRun();
    void setTime(long);

protected:
    void run();

private:
    class MainWindow* win;
    volatile bool stopped;
    volatile bool started;
    volatile bool paused;
    PCB* lastPCB=NULL;
    long sleepTime=1000;

};

#endif // THREAD_H
