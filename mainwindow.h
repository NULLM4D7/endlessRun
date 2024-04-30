#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QSound>
#include "tipwidget.h"

#define DATAPATH "data.d"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow;

class Enemy : public QLabel
{
public:
    Enemy(MainWindow* mw, QWidget* parent)
        : QLabel(parent)
        , m_mw(mw)
        , m_status(true)
        , m_isOutDelay(true)
    {

    }

    MainWindow* m_mw;

    void doAnim();
    int isTouch();
    bool m_status;      // enemy状态 ture：在bar的上方 false：在bar的下方
    bool m_isOutDelay;  // 延迟是否结束
};

class EndWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    friend class Enemy;
    friend class EndWidget;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setUIVal();
private:
    Ui::MainWindow *ui;
    QPixmap m_actorPic;
    QPixmap m_actorPicF;
    QVector<QLabel*> m_bars;
    QVector<Enemy*> m_enemys;
    bool m_status;      // 角色状态 ture：在bar的上方 false：在bar的下方
    bool m_barFrontI;   // 前面的bar的下标
    bool m_isStop;      // 当前状态是否为停止
    int m_health;       // actor生命值
    int m_score;
    bool m_isLongPressE;  // 是否长按E 长按E时视为无敌
    QTimer* m_pressTimer;   // 长按翻转按钮计时器
    bool m_isShowFrame;   // 是否显示Actor和enemy的边框
    int m_floatTime;  // 浮动动画运行时间
    bool m_isUpFloat;   // 是否向上浮动
    bool m_isFly;   // character是否处于飞行状态
    int m_ms;   // 一周期动画执行时间
    int m_shieldTime;   // 护盾剩余时间
    QLabel* m_shieldLabel;  // 加载护盾图像
    QLabel* m_aEffectLabel;  // 攻击特效
    int m_aEeffTime;    // 特效持续时间
    QSound* m_sound;
    EndWidget* m_ew;
    QPoint m_mousePressPoint;   // 鼠标按下刘海时的位置
    QPoint m_oPoint;    // 鼠标按下刘海时窗口的位置
    TipWidget* m_tipW;  // 游戏提示窗口
    bool m_labIsE;  // lab对象是否存在

    QLabel* getBar();
    Enemy* getEnemy();
    void flipActor();
    // 执行bar动画
    void doBarAnim(QPoint startFP, QPoint endFP);
    // 执行浮动动画
    void doFloatAnim(QLabel* target);
    void writeScore();

    void keyPressEvent(QKeyEvent * event) override;
    void keyReleaseEvent(QKeyEvent * event) override;
    void mousePressEvent(QMouseEvent* me) override;
    void mouseMoveEvent(QMouseEvent* me) override;
    void mouseReleaseEvent(QMouseEvent* me) override;
    void paintEvent(QPaintEvent* pe) override;

signals:
    void animFinish();
    void stopAll();
    void startAll();
};
#endif // MAINWINDOW_H
