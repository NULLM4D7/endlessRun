#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPropertyAnimation>
#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>
#include <QTextCodec>
#include <QPainter>
#include <QPen>
#include "endwidget.h"
#include <QFile>

#define ACTORY this->height() / 2 - 95
#define ACTORYF this->height() / 2 + 20
#define ENEMYY m_mw->m_bars[0]->y() - 80
#define ENEMYYF m_mw->m_bars[0]->y() + 30
#define REFLOATANIM() m_floatTime = 0; m_isUpFloat = true
#define C(cnStr) QTextCodec::codecForName("GBK")->toUnicode(cnStr)
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_status(true)
    , m_barFrontI(0)
    , m_isStop(false)
    , m_health(3)
    , m_score(0)
    , m_isLongPressE(false)
    , m_isShowFrame(false)
    , m_floatTime(0)
    , m_isUpFloat(true)
    , m_isFly(false)
    , m_ms(2000)
    , m_shieldTime(0)
    , m_aEeffTime(0)
    , m_mousePressPoint(-1, -1)
    , m_labIsE(false)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/res/pic/xiaoBird.png"));
    setWindowTitle(C("endlessRun"));
    ui->healthLabel->setText(QString("HP:%1").arg(m_health));
    ui->score->setText(QString("score:%1").arg(m_score));
    this->setFixedSize(width(), height());
    ui->bar->resize(width(), ui->bar->height());
    setWindowFlags(Qt::FramelessWindowHint);
    ui->bar->setStyleSheet("border: 1px solid rgb(120, 80, 80);");
    m_tipW = nullptr;

#define SETCOLOR setStyleSheet("color: rgb(120, 80, 80);")
#define SETSS setStyleSheet(\
        "   QPushButton:!checked {"\
        "   color: rgb(120, 80, 80);"\
        "   background-color: rgb(255, 255, 255);"\
        "   border: 2px solid rgb(120, 80, 80);"\
        "   border-radius: 5px;"\
        "   }"\
        "   QPushButton:hover {"\
        "   background-color: rgb(255, 245, 235);"\
        "   }"\
        "   QPushButton:pressed {"\
        "   background-color: rgb(255, 235, 225);"\
        "   }"\
        )

    ui->score->SETCOLOR;
    ui->healthLabel->SETCOLOR;
    ui->ctrlBtn->SETSS;
    ui->attackBtn->SETSS;
    ui->flipBtn->SETSS;
    ui->isShowFrameBtn->SETSS;
    ui->exitBtn->SETSS;
    ui->minBtn->SETSS;
    ui->tipsBtn->SETSS;
    ui->topBtn->SETSS;

    // 退出按钮
    connect(ui->exitBtn, &QPushButton::clicked, this, [=]()
    { 
        writeScore();
        exit(0);
    });
    // 最小化按钮
    connect(ui->minBtn, &QPushButton::clicked, this, [=]()
    {
        if (!m_isStop) emit ui->ctrlBtn->clicked();
        this->showMinimized();
    });
    // 游戏提示窗口
    connect(ui->tipsBtn, &QPushButton::clicked, this, [=]()
    {
// 生成游戏提示窗口
#define CREATETIPW() \
        if (!m_isStop) ui->ctrlBtn->click();\
        if (m_tipW == nullptr)\
        {\
            ui->ctrlBtn->setEnabled(false);\
            ui->isShowFrameBtn->setEnabled(false);\
            m_tipW = new TipWidget(this);\
            m_tipW->move((width() - m_tipW->width()) / 2, (height() - m_tipW->height()) / 2);\
            connect(m_tipW->m_OKBtn, &QPushButton::clicked, this, [=]()\
            {\
                ui->ctrlBtn->setEnabled(true);\
                ui->isShowFrameBtn->setEnabled(true);\
                m_tipW->close();\
                m_tipW->deleteLater();\
                m_tipW = nullptr;\
            });\
            m_tipW->exec();\
        }

        CREATETIPW();
    });
    // 获取最高分
    connect(ui->topBtn, &QPushButton::clicked, this, [=]()
    {
        if (m_labIsE) return;
        m_labIsE = true;
        if (!m_isStop) emit ui->ctrlBtn->click();
        QLabel* lab = new QLabel(this);
        lab->resize(200, 200);
        lab->setStyleSheet("background-color:rgb(255, 255, 255); border: 2px solid rgb(120, 80, 80); color: rgb(120, 80, 80)");
        lab->move((width() - lab->width()) / 2, (height() - lab->height()) / 2);
        QFile f(DATAPATH);
        f.open(QFile::ReadOnly);
        QByteArray bArr = f.read(64);
        bool isNum;
        double topScore = bArr.toDouble(&isNum);
        if (!isNum)
        {
            lab->setText(C("历史记录：0"));
        }
        else
        {
            lab->setText(C("历史记录：\n%1").arg(topScore));
        }
        lab->setAlignment(Qt::AlignCenter);
        f.close();
        QPushButton* closeBtn = new QPushButton(lab);
        closeBtn->resize(50, 25);
        closeBtn->setText(C("关闭"));
        closeBtn->move((lab->width() - closeBtn->width()) / 2, lab->height() - closeBtn->height() - 10);
        closeBtn->SETSS;
        connect(closeBtn, &QPushButton::clicked, this, [=]()
        {
            m_labIsE = false;
            lab->close();
            lab->deleteLater();
        });
        lab->show();
    });

    // 初始化音频
    m_sound = new QSound(":/res/mp3/music.wav", this);
    m_sound->play();
    m_sound->setLoops(-1);
    connect(ui->attackBtn, &QPushButton::clicked, this, [=]()
    {
        if (!m_isLongPressE)
        {
            // 播放攻击音效
            QSound::play(":/res/mp3/attack.wav");
        }
    });

    // 初始化角色图像
    m_actorPic = QPixmap(":/res/pic/xiaoBird.png");
    m_actorPic = m_actorPic.scaled(100, 100);
    m_actorPicF = QPixmap(":/res/pic/xiaoBirdF.png");
    m_actorPicF = m_actorPicF.scaled(100, 100);

    ui->character->resize(100, 100);
    ui->character->setPixmap(m_actorPic);
    ui->character->move(100, ACTORY);
    ui->character->show();

    // 初始化护盾信息
    m_shieldLabel = new QLabel(ui->character);
    m_shieldLabel->resize(100, 100);
    m_shieldLabel->setPixmap(QPixmap(":/res/pic/shield.png"));
    m_shieldLabel->hide();

    // 初始化攻击特效信息
    m_aEffectLabel = new QLabel(this);
    m_aEffectLabel->resize(100, 100);
    m_aEffectLabel->setPixmap(QPixmap(":/res/pic/downA.png"));
    m_aEffectLabel->hide();

    // 初始化bar对象信息
    for (int i = 0; i < 2; i++) m_bars.append(getBar());
    m_bars[0]->move(0, height() / 2);
    m_bars[1]->move(width(), height() / 2);
    doBarAnim(m_bars[0]->pos(), QPoint(-width(), m_bars[0]->y()));

    // 初始化enemy信息
    m_enemys.append(getEnemy());
    m_enemys[0]->move(width(), m_bars[0]->y() - 80);
    m_enemys[0]->doAnim();

    // chara执行浮动动画
    doFloatAnim(ui->character);

    // 控制bar动画循环
    connect(this, &MainWindow::animFinish, this, [=]()
    {
        m_bars[!m_barFrontI] = getBar();
        doBarAnim(m_bars[m_barFrontI]->pos(), QPoint(-width(), m_bars[m_barFrontI]->y()));
    });

    // 控制Actor翻转
    connect(ui->flipBtn, &QPushButton::pressed, this, [=]()
    {
        m_pressTimer = new QTimer(this);
        m_pressTimer->start(400);
        connect(m_pressTimer, &QTimer::timeout, this, [=]()
        {
            REFLOATANIM();
            if (m_status)
            {
                ui->character->move(ui->character->x(), ACTORY - 80);
            }
            else
            {
                ui->character->move(ui->character->x(), ACTORYF + 80);
            }
            m_isLongPressE = true;
            if (m_pressTimer)
            {
                m_pressTimer->stop();
                m_pressTimer->deleteLater();
                m_pressTimer = nullptr;
            }
        });
    });
    connect(ui->flipBtn, &QPushButton::released, this, [=]()
    {
        if (m_pressTimer)
        {
            m_pressTimer->stop();
            m_pressTimer->deleteLater();
            m_pressTimer = nullptr;
        }
        m_isLongPressE = false;

        REFLOATANIM();
        flipActor();
    });

    // 控制游戏暂停与继续
    connect(ui->ctrlBtn, &QPushButton::clicked, this, [=]()
    {
        if (m_isStop)
        {
            ui->ctrlBtn->setText(C("暂停(ESC)"));
            emit MainWindow::startAll();
        }
        else
        {
            ui->ctrlBtn->setText(C("继续(ESC)"));
            emit MainWindow::stopAll();
        }
        m_isStop = !m_isStop;
    });

    // 是否显示判定框
    connect(ui->isShowFrameBtn, &QPushButton::clicked, this, [=]()
    {
        // if (m_isStop) return;
        m_isShowFrame = !m_isShowFrame;
        if (m_isShowFrame)
        {
            ui->isShowFrameBtn->setText(C("隐藏判定框(S)"));
            for (Enemy* enemy : m_enemys)
            {
                enemy->setStyleSheet("border: 2px solid red;");
            }
        }
        else
        {
            ui->isShowFrameBtn->setText(C("显示判定框(S)"));
            for (Enemy* enemy : m_enemys)
            {
                enemy->setStyleSheet("border: 0px;");
            }
        }
        update();
    });
    this->show();
    CREATETIPW();
}

// 获取bar对象
QLabel* MainWindow::getBar()
{
    QLabel* bar = new QLabel(this);
    int num = rand() % 3;
    bar->setStyleSheet(QString("background-image: url(:/res/pic/mat%1.jpg);").arg(num + 1));
    bar->resize(this->width(), 25);
    bar->show();
    return bar;
}

// 获取enemy对象
Enemy* MainWindow::getEnemy()
{
    Enemy* enemy(new Enemy(this, this));
    const int size = 75;
    enemy->resize(size, size);
    int num = rand() % 4;
    enemy->setPixmap(QPixmap(QString(":/res/pic/dls%1.jpg").arg(num)).scaled(size, size));
    enemy->show();
    return enemy;
}

// 传入前面的bar的起始点 结束点 动画运行时间
void MainWindow::doBarAnim(QPoint startFP, QPoint endFP)
{
    m_bars[m_barFrontI]->move(startFP);
    m_bars[!m_barFrontI]->move(startFP + QPoint(width(), 0));
    QTimer* timer(new QTimer(this));
    const int f = 20;  // 每f毫秒刷新一次
    timer->start(f);
    connect(timer, &QTimer::timeout, this, [=]()
    {
        if (m_isStop) return;
        float vx = float(startFP.x() - endFP.x()) / (float)m_ms;
        float vy = float(startFP.y() - endFP.y()) / (float)m_ms;
        m_bars[m_barFrontI]->move(m_bars[m_barFrontI]->x() - vx * f, m_bars[m_barFrontI]->y() - vy * f);
        m_bars[!m_barFrontI]->move(m_bars[!m_barFrontI]->x() - vx * f, m_bars[!m_barFrontI]->y() - vy * f);
        if (m_bars[!m_barFrontI]->x() <= 0)
        {
            timer->stop();
            timer->deleteLater();
            m_barFrontI = !m_barFrontI;
            emit animFinish();
        }
    });

    connect(this, &MainWindow::stopAll, this, [=]()
    {
        ui->flipBtn->setEnabled(false);
        ui->attackBtn->setEnabled(false);
        // ui->isShowFrameBtn->setEnabled(false);
    });

    connect(this, &MainWindow::startAll, this, [=]()
    {
        ui->flipBtn->setEnabled(true);
        ui->attackBtn->setEnabled(true);
        // ui->isShowFrameBtn->setEnabled(true);
    });
}

// 翻转Actor
void MainWindow::flipActor()
{

    QSound::play(":/res/mp3/flip.wav");
    // 翻倒
    if (m_status)
    {
        ui->character->move(100, ACTORYF);
        ui->character->setPixmap(m_actorPicF);
    }
    // 翻正
    else
    {
        ui->character->move(100, ACTORY);
        ui->character->setPixmap(m_actorPic);
    }
    ui->character->show();
    m_status = !m_status;
}

// 执行敌方单位动画
void Enemy::doAnim()
{
    if (m_mw->m_isShowFrame)
    {
        setStyleSheet("border: 2px solid red;");
    }
    else
    {
        setStyleSheet("border: 0px;");
    }
    m_status = rand() % 2;
    if (m_status)
    {
        move(m_mw->width(), ENEMYY);
    }
    else
    {
        move(m_mw->width(), ENEMYYF);
    }

    QTimer* timer(new QTimer(m_mw));
    const int f = 20;  // 每f毫秒刷新一次
    timer->start(f);

    connect(timer, &QTimer::timeout, this, [=]()
    {
// 刷新enemy对象
#define NEWENEMY() \
        QTimer* timer = new QTimer(this); \
            timer->start(((rand() % 5) + 1) * 1000); \
            m_isOutDelay = false; \
            connect(timer, &QTimer::timeout, this, [=](){ \
                m_isOutDelay = true; \
                timer->stop(); \
                timer->deleteLater();}); \
            setPixmap(QPixmap(QString(":/res/pic/dls%1.jpg").arg(rand() % 4)).scaled(75, 75)); \
            m_status = rand() % 2; \
            if (m_status) move(m_mw->width(), m_mw->m_bars[0]->y() - 80); \
            else move(m_mw->width(), m_mw->m_bars[0]->y() + 30); \

        if (m_mw->m_isStop || !m_isOutDelay) return;
        float vx = float(m_mw->width() + width()) / (float)m_mw->m_ms;
        move(x() - vx * f, y());

        // enemy攻击到actor
        if (isTouch() == 1 && !m_mw->m_isLongPressE)
        {
            QSound::play(":/res/mp3/hurt.wav");
            // actor有护盾
            if (m_mw->m_shieldTime != 0)
            {
                m_mw->m_score += 100;
                m_mw->ui->score->setText(QString("score:%1").arg(m_mw->m_score));
                NEWENEMY();
                return;
            }
            // actor无护盾
            m_mw->m_health--;
            m_mw->ui->healthLabel->setText(QString("HP:%1").arg(m_mw->m_health));

            // 生成1秒护盾
            int t = 1000;
            m_mw->m_shieldTime += t;
            m_mw->m_shieldLabel->show();
            QTimer* tr(new QTimer(this));
            tr->start(t);
            connect(tr, &QTimer::timeout, this, [=]()
            {
                if (m_mw->m_isStop) return;
                m_mw->m_shieldTime -= t;
                if (m_mw->m_shieldTime <= 0)
                {
                    m_mw->m_shieldLabel->hide();
                }
                tr->stop();
                tr->deleteLater();
            });

            NEWENEMY();
            if (m_mw->m_health <= 0)
            {
                emit m_mw->ui->ctrlBtn->click();
                // 确保在选择退出和继续时无法使用所有按钮
                m_mw->ui->ctrlBtn->setEnabled(false);
                m_mw->ui->isShowFrameBtn->setEnabled(false);
                // 游戏结束
                m_mw->writeScore();

                m_mw->m_ew = new EndWidget(m_mw);
                m_mw->m_ew->show();
                m_mw->m_ew->move((m_mw->width() - m_mw->m_ew->width()) / 2, (m_mw->height() - m_mw->m_ew->height()) / 2);
            }
        }

        // enemy对象出界
        if (x() <= -width())
        {
            NEWENEMY();
        }
    });

    // 攻击
    connect(m_mw->ui->attackBtn, &QPushButton::clicked, this, [=]()
    {
        if (!m_mw->m_isLongPressE)
        {
            // 特效持续100ms
            int time = 100;
            QTimer* tr = new QTimer(this);
            tr->start(time);
            m_mw->m_aEeffTime += time;
            if (m_mw->m_status)
            {
                m_mw->m_aEffectLabel->setPixmap(QPixmap(":/res/pic/downA.png"));
                m_mw->m_aEffectLabel->move(m_mw->ui->character->x() + 5, m_mw->m_bars[0]->y() + m_mw->m_bars[0]->height());
            }
            else
            {
                m_mw->m_aEffectLabel->setPixmap(QPixmap(":/res/pic/upA.png"));
                m_mw->m_aEffectLabel->move(m_mw->ui->character->x() + 5, m_mw->m_bars[0]->y() - m_mw->m_aEffectLabel->height());
            }
            m_mw->m_aEffectLabel->show();
            connect(tr, &QTimer::timeout, this, [=]()
            {
                m_mw->m_aEeffTime -= time;
                if (m_mw->m_aEeffTime <= 0)
                {
                    m_mw->m_aEffectLabel->hide();
                }
                tr->stop();
                tr->deleteLater();
            });
        }

        // 攻击到敌方单位
        if (isTouch() == 2 && !m_mw->m_isLongPressE)
        {
            m_mw->m_score += 100;
            m_mw->ui->score->setText(QString("score:%1").arg(m_mw->m_score));
            NEWENEMY();

            // 加速1秒
#define T 1000
            m_mw->m_ms *= 0.5;
            QTimer* t1 = new QTimer(this);
            t1->start(T);
            m_mw->m_shieldTime += T;
            m_mw->m_shieldLabel->show();

            connect(t1, &QTimer::timeout, this, [=]()
            {
                if (m_mw->m_isStop) return;
                m_mw->m_shieldTime -= T;
                if (m_mw->m_shieldTime == 0)
                {
                    m_mw->m_shieldLabel->hide();
                }
                m_mw->m_ms *= 2;
                t1->stop();
                t1->deleteLater();
            });

// arg:触发新增敌方单位的分数
#define ADDENEMY(score) \
            if (m_mw->m_score == score){ \
                QTimer* t(new QTimer(this)); \
                t->start(1000); \
                connect(t, &QTimer::timeout, this, [=](){ \
                    m_mw->m_enemys.append(m_mw->getEnemy()); \
                    m_mw->m_enemys[m_mw->m_enemys.size() - 1]->doAnim(); \
                    t->stop(); \
                    t->deleteLater(); \
                }); \
            }

            // 分数达到500后 追加一个enemy
            ADDENEMY(500);

            // 分数达到1000后 追加一个enemy
            ADDENEMY(1000);

            // 分数达到3000后 追加一个enemy
            ADDENEMY(3000);

            // 分数达到5000后 追加一个enemy
            ADDENEMY(5000);

            // 分数达到10000后 追加一个enemy
            ADDENEMY(10000);
#undef ADDENEMY
            // 1000分后加速
            if (m_mw->m_score == 1000) m_mw->m_ms *= 0.75;
            // 5000分后加速
            if (m_mw->m_score == 5000) m_mw->m_ms *= 0.75;
            // 10000分后加速
            if (m_mw->m_score == 10000) m_mw->m_ms *= 0.75;
        }
    });
}

// return 0: have no touch
// return 1: enemy attack actor
// return 2: actor can attack enemy
int Enemy::isTouch()
{
#define ACTORX m_mw->ui->character->pos().x()
    if (m_mw->m_status == m_status)
    {
        if ((x() < ACTORX + 90) && (x() + width() > ACTORX + 12))
        {
            return 1;
        }
    }
    else if(m_mw->m_status != m_status)
    {
        if ((x() < ACTORX + 90) && (x() + width() > ACTORX + 12))
        {
            return 2;
        }
    }
#undef ACTORX
    return 0;
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
    switch(event->key())
    {
    case Qt::Key_E:
        if (m_isStop) return;
        // 长按E键
        if(event->isAutoRepeat())
        {
            m_isLongPressE = true;
            if (m_isFly) return;
            REFLOATANIM();
            if (m_status)
            {
                ui->character->move(ui->character->x(), ACTORY - 80);
            }
            else
            {
                ui->character->move(ui->character->x(), ACTORYF + 80);
            }
            m_isFly = true;
        }
        // 短按E键
        else
        {
            m_isLongPressE = false;
        }
        break;
    case Qt::Key_Escape:
        emit ui->ctrlBtn->click();
        break;
    case Qt::Key_S:
        emit ui->isShowFrameBtn->click();
        break;
    default:
        break;
    }
    return QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent * event)
{
    if (m_isStop) return;
    switch(event->key())
    {
    case Qt::Key_E:
        // E键松开（未进入自动重复输入状态）
        if (!event->isAutoRepeat())
        {
            REFLOATANIM();
            flipActor();
            m_isLongPressE = false;
            m_isFly = false;
        }
        break;
    default:
        break;
    }
    return QMainWindow::keyReleaseEvent(event);
}

// 按下鼠标左键视为Actor进行攻击
void MainWindow::mousePressEvent(QMouseEvent* me)
{
    if (me->button() == Qt::LeftButton)
    {
        // 记录鼠标和窗口的位置
        if (me->y() < ui->bar->height())
        {
            m_mousePressPoint = me->globalPos();
            m_oPoint = pos();
        }
        // 攻击
        else
        {
            if (m_isStop) return;
            emit ui->attackBtn->click();
        }
    }
    return QMainWindow::mousePressEvent(me);
}
void MainWindow::mouseMoveEvent(QMouseEvent* me)
{
    if (m_mousePressPoint != QPoint(-1, -1))
    {
        move(m_oPoint + me->globalPos() - m_mousePressPoint);
    }
    return QMainWindow::mouseMoveEvent(me);
}
void MainWindow::mouseReleaseEvent(QMouseEvent* me)
{
    if (m_mousePressPoint != QPoint(-1, -1))
    {
        m_mousePressPoint = QPoint(-1, -1);
    }
    return QMainWindow::mouseReleaseEvent(me);
}

// 执行浮动动画
void MainWindow::doFloatAnim(QLabel* target)
{
    QTimer* timer = new QTimer(this);
    const int f = 20;
    const float size = 1;
    timer->start(f);

    connect(timer, &QTimer::timeout, this, [=]()
    {
        if (m_isStop) return;
        m_floatTime += f;
        if (m_status)
        {
            if (m_isUpFloat)
            {
                target->move(target->x(), target->y() - size);
            }
            else
            {
                target->move(target->x(), target->y() + size);
            }
        }
        else
        {
            if (m_isUpFloat)
            {
                target->move(target->x(), target->y() + size);
            }
            else
            {
                target->move(target->x(), target->y() - size);
            }
        }
        // 切换浮动模式
        if (m_floatTime == f * 25)
        {
            m_floatTime = 0;
            m_isUpFloat = !m_isUpFloat;
        }
    });
}

// 绘制判定框
void MainWindow::paintEvent(QPaintEvent* pe)
{
    QPainter ptClr(this);
    ptClr.setBrush(QBrush(qRgb(255, 255, 255)));
    ptClr.drawRect(rect());
    if (m_isShowFrame)
    {
        QPen pen;
        pen.setColor(qRgb(255, 0, 0));
        pen.setWidth(3);
        QPainter pt(this);
        pt.setPen(pen);
        const int actorX = ui->character->x();
        const int actorY = 200;
        pt.drawLine(QPoint(actorX + 90, actorY),
                    QPoint(actorX + 90, 2 * m_bars[0]->y() + m_bars[0]->height() - actorY));
        pt.drawLine(QPoint(actorX + 12, actorY),
                    QPoint(actorX + 12, 2 * m_bars[0]->y() + m_bars[0]->height() - actorY));
    }
    return QMainWindow::paintEvent(pe);
}

void MainWindow::setUIVal()
{
    ui->healthLabel->setText(QString("HP:%1").arg(m_health));
    ui->score->setText(QString("score:%1").arg(m_score));
    ui->ctrlBtn->setText(C("暂停(ESC)"));
    ui->character->move(100, ACTORY);
    ui->character->setPixmap(m_actorPic);
    ui->ctrlBtn->setEnabled(true);
    ui->isShowFrameBtn->setEnabled(true);
}

void MainWindow::writeScore()
{
    QFile file(DATAPATH);
    file.open(QFile::ReadWrite);
    QByteArray byteArr = file.read(64);
    bool isNum;
    double num = byteArr.toDouble(&isNum);
    if (isNum)
    {
        if (num < m_score)
        {
            file.close();
            file.open(QFile::WriteOnly);
            file.write(QString::number(m_score).toStdString().c_str());
        }
    }
    else
    {
        file.close();
        file.open(QFile::WriteOnly);
        file.write(QString::number(m_score).toStdString().c_str());
    }
    file.close();
}

MainWindow::~MainWindow()
{
    delete ui;
}
