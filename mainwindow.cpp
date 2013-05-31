#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGraphicsView>
#include <QPixmap>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _pixmap(800*1.6,800),
    _was_down(false),
    _eraser(false)
{
    ui->setupUi(this);

    QGraphicsView* view = ui->gv;
    view->setRenderHint(QPainter::Antialiasing);
    _scene = new QGraphicsScene(0, 0, 800 * 1.6, 800, this);
    view->setScene(_scene);
    view->setFixedSize(800 * 1.6, 800);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _pixmap.fill(Qt::white);
    _pixmap_item = _scene->addPixmap(_pixmap);
    _pixmap_item->setPos(0, 0);

    QPen cursor_pen(Qt::red, 3);
    _cursor_item = _scene->addEllipse(-1, -1, 7, 7, cursor_pen);
    _cursor_type_item = _scene->addSimpleText("Eraser");
    _cursor_type_item->setParentItem(_cursor_item);
    _cursor_type_item->setPos(10,5);
    _cursor_type_item->setVisible(false);

    connect(&_wacom, SIGNAL(onStateChanged(float,float,float)),
            this, SLOT(updateWacom(float,float,float)));
    _wacom.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateWacom(float x, float y, float p)
{
    bool down = (p > 0.16);
    if (x > 0.0 && y > 0.0) {
        _cursor_item->setPos(x * 800.0 * 1.6, y * 800.0);
    }

    if (down) {
        QPainter ptr(&_pixmap);
        ptr.setRenderHint(QPainter::Antialiasing);
        ptr.setPen(QPen(_eraser ? Qt::white : Qt::black, 5));

        QPointF pt(x * 800.0 * 1.6 + 3.5, y * 800.0 + 3.5);

        if (_was_down) {
            ptr.drawLine(_prev_point, pt);
        }

        ptr.drawPoint(x * 800.0 * 1.6 + 3.5, y * 800.0 + 3.5);
        ptr.end();
        _pixmap_item->setPixmap(_pixmap);

        _was_down = true;
        _prev_point = pt;
    } else {
        _was_down = false;
    }
}

void MainWindow::clearCanvas()
{
    _pixmap.fill(Qt::white);
    _pixmap_item->setPixmap(_pixmap);
}

void MainWindow::toggleEraser()
{
    _eraser = !_eraser;
    if (_eraser) {
        ui->actionEraser->setText("Pen");
        _cursor_type_item->setVisible(true);
    } else {
        ui->actionEraser->setText("Eraser");
        _cursor_type_item->setVisible(false);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if (ev->modifiers() & Qt::ControlModifier && ev->key() == Qt::Key_X)
        clearCanvas();
    else if (ev->key() == Qt::Key_Space)
        toggleEraser();
    else
        QMainWindow::keyPressEvent(ev);
}

