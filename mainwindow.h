#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QPointF>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Item {
    QPointF pos;
    int size;
bool caught;
int weight;
    bool moving; // ★ 新增，是否正在拖动
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void updateFrame();

private:
    Ui::MainWindow *ui;

    QTimer *timer;
    QVector<Item> items;

    double hookAngle;    // 钩子角度
    double hookLength;   // 钩子长度
    double hookSpeed;    // 钩子速度
    bool stretching;     // 正在伸长
    bool retracting;     // 正在收回
    int score;           // 当前得分

    void initGame();
    void resetHook();
    void checkCatch();
};

#endif // MAINWINDOW_H
 
