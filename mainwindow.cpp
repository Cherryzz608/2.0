#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QKeyEvent>
#include <QtMath>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    hookAngle(0),
    hookLength(50),
    hookSpeed(0),
    stretching(false),
    retracting(false),
    score(0)
{
    ui->setupUi(this);
    setFixedSize(800, 600);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    timer->start(16); // 大约60帧

    initGame();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initGame()
{
    items.clear();
    for (int i = 0; i < 10; ++i) {
        Item item;
        item.pos = QPointF(QRandomGenerator::global()->bounded(50, 750),
                           QRandomGenerator::global()->bounded(300, 550));
        item.size = (QRandomGenerator::global()->bounded(1, 4)) * 10; // 10、20、30
        item.caught = false;
        item.moving = false;
        items.append(item);
    }
}

void MainWindow::resetHook()
{
    hookLength = 50;
    hookSpeed = 0;
    stretching = false;
    retracting = false;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // 背景
    painter.fillRect(rect(), Qt::cyan);

    // 分数
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 18));
    painter.drawText(10, 30, QString("Score: %1").arg(score));

    // 钩子
    QPointF start(400, 0);
    QPointF end(400 + hookLength * qSin(qDegreesToRadians(hookAngle)),
                hookLength * qCos(qDegreesToRadians(hookAngle)));

    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(start, end);

    painter.setBrush(Qt::gray);
    painter.drawEllipse(end, 5, 5);

    // 金矿
    for (const Item &item : items) {
        if (!item.caught) {
            painter.setBrush(Qt::yellow);
            painter.drawEllipse(item.pos, item.size, item.size);
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        if (!stretching && !retracting) {
            stretching = true;
            hookSpeed = 5;
        }
    }
}

void MainWindow::updateFrame()
{
    if (!stretching && !retracting) {
        static bool rotateRight = true;
        if (rotateRight)
            hookAngle += 1;
        else
            hookAngle -= 1;

        if (hookAngle > 75) rotateRight = false;
        if (hookAngle < -75) rotateRight = true;
    } else {
        hookLength += hookSpeed;

        if (stretching && hookLength > 600) {
            stretching = false;
            retracting = true;
            hookSpeed = -5;
        }

        if (retracting && hookLength <= 50) {
            // ★ 钩子回到顶部
            for (Item &item : items) {
                if (item.moving) {
                    item.caught = true;
                    item.moving = false;
                    score += item.size * 10;
                }
            }
            resetHook();
        }

        if (stretching) {
            checkCatch();
        }
    }

    // ★ 让被抓的金矿跟着钩子动
    QPointF hookPos(400 + hookLength * qSin(qDegreesToRadians(hookAngle)),
                    hookLength * qCos(qDegreesToRadians(hookAngle)));
    for (Item &item : items) {
        if (item.moving) {
            item.pos = hookPos;
        }
    }

    update();
}

void MainWindow::checkCatch()
{
    QPointF hookPos(400 + hookLength * qSin(qDegreesToRadians(hookAngle)),
                    hookLength * qCos(qDegreesToRadians(hookAngle)));

    for (Item &item : items) {
        if (!item.caught) {
            double dist = qSqrt(qPow(hookPos.x() - item.pos.x(), 2) + qPow(hookPos.y() - item.pos.y(), 2));
            if (dist <= item.size + 5) {
                item.moving = true; // ★ 被挂住，跟着钩子走
                retracting = true;
                stretching = false;
               hookSpeed = -qMax(1.0, 6.0 / item.weight);                break;
            }
        }
    }
}
