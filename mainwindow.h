#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsItem>
#include "wacomthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateWacom(float x, float y, float p);
    void clearCanvas();
    void toggleEraser();

protected:
    void keyPressEvent(QKeyEvent *);
    
private:
    Ui::MainWindow *ui;

    QGraphicsScene *_scene;
    QGraphicsEllipseItem *_cursor_item;
    QGraphicsSimpleTextItem *_cursor_type_item;

    WacomThread _wacom;

    QPixmap _pixmap;
    QGraphicsPixmapItem *_pixmap_item;

    bool _was_down;
    QPointF _prev_point;

    bool _eraser;
};

#endif // MAINWINDOW_H
