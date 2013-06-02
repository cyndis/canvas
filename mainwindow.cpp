#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGraphicsView>
#include <QPixmap>
#include <QKeyEvent>

#define CANVAS_HEIGHT 800

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _pixmap(CANVAS_HEIGHT*1.6,CANVAS_HEIGHT),
    _was_down(false),
    _eraser(false)
{
    ui->setupUi(this);

    QGraphicsView* view = ui->gv;
    view->setRenderHint(QPainter::Antialiasing);
    _scene = new QGraphicsScene(0, 0, CANVAS_HEIGHT * 1.6, CANVAS_HEIGHT, this);
    view->setScene(_scene);
    view->setFixedSize(_scene->width(), _scene->height());
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _pixmap.fill(Qt::white);
    _pixmap_item = _scene->addPixmap(_pixmap);
    _pixmap_item->setPos(0, 0);

    QPen cursor_pen(Qt::red, 3);
    _cursor_item = _scene->addEllipse(-3.5, -3.5, 7, 7, cursor_pen);
    _cursor_type_item = _scene->addSimpleText("Eraser");
    _cursor_type_item->setParentItem(_cursor_item);
    _cursor_type_item->setPos(10,5);
    _cursor_type_item->setVisible(false);

    connect(&_wacom, SIGNAL(onStateChanged(float,float,float)),
            this, SLOT(updateWacom(float,float,float)));
    connect(&_wacom, SIGNAL(onButtonChanged(int,bool)),
            this, SLOT(updateWacomButton(int,bool)));
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
        _cursor_item->setPos(x * _scene->width(), y * _scene->height());
    }

    if (down) {
        QPainter ptr(&_pixmap);
        ptr.setRenderHint(QPainter::Antialiasing);

        QPen line_pen = QPen(_eraser ? Qt::white : Qt::black, 5);
        line_pen.setCapStyle(Qt::RoundCap);
        ptr.setPen(line_pen);

        QPointF pt(x * _scene->width(), y * _scene->height());

        if (_was_down) {
            ptr.drawLine(_prev_point, pt);
        }

        ptr.setPen(QPen(Qt::transparent));
        ptr.setBrush(QBrush(_eraser ? Qt::white : Qt::black));

        ptr.drawEllipse(pt, 2, 2);
        ptr.end();
        _pixmap_item->setPixmap(_pixmap);

        _was_down = true;
        _prev_point = pt;
    } else {
        _was_down = false;
    }
}

void MainWindow::updateWacomButton(int button, bool state)
{
    Q_UNUSED(button);
    setEraserEnabled(state);
}

void MainWindow::clearCanvas()
{
    _pixmap.fill(Qt::white);
    _pixmap_item->setPixmap(_pixmap);
}

void MainWindow::toggleEraser()
{
    setEraserEnabled(!_eraser);
}

void MainWindow::setEraserEnabled(bool enabled)
{
    _eraser = enabled;
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

