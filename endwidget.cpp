#include "endwidget.h"
#include <QDebug>
#include "mainwindow.h"
#include <QFont>

EndWidget::EndWidget(QWidget *parent)
    : QWidget{parent}
{
    setFixedSize(200, 200);
    setWindowFlags(Qt::FramelessWindowHint);

    m_back = new QLabel(this);
    m_back->resize(width(), height());
    m_back->setStyleSheet("background-color:rgb(255, 255, 255); border: 2px solid rgb(120, 80, 80);");

    m_aginBtn = new QPushButton(this);
    m_exitBtn = new QPushButton(this);
    QFont font("Segoe Print");
    m_aginBtn->setFont(font);
    m_exitBtn->setFont(font);
    m_aginBtn->resize(75, 50);
    m_exitBtn->resize(75, 50);
    m_aginBtn->setText("agin");
    m_exitBtn->setText("exit");
    m_aginBtn->move((width() - m_aginBtn->width()) / 2, height() / 4 - m_aginBtn->height() / 2);
    m_exitBtn->move((width() - m_exitBtn->width()) / 2, height() * 3 / 4 - m_exitBtn->height() / 2);

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
    m_aginBtn->SETSS;
    m_exitBtn->SETSS;

    m_mw = static_cast<MainWindow*>(parent);

    connect(m_aginBtn, &QPushButton::clicked, this, [=]()
    {
        m_mw->m_status = true;
        // m_mw->m_barFrontI = 0;
        m_mw->m_isStop = false;
        m_mw->m_health = 3;
        m_mw->m_score = 0;
        m_mw->m_isLongPressE = false;
        // m_mw->m_isShowFrame = false;
        m_mw->m_floatTime = 0;
        m_mw->m_isUpFloat = true;
        m_mw->m_isFly = false;
        m_mw->m_ms = 2000;
        m_mw->m_shieldTime = 0;
        m_mw->m_aEeffTime = 0;

        m_mw->setUIVal();

        m_mw->m_bars[0]->move(0, m_mw->height() / 2);
        m_mw->m_bars[1]->move(m_mw->width(), m_mw->height() / 2);

        for (int i = 0; i < m_mw->m_enemys.size(); i++)
        {
            m_mw->m_enemys[i]->deleteLater();
        }
        m_mw->m_enemys.clear();
        m_mw->m_enemys.append(m_mw->getEnemy());
        m_mw->m_enemys[0]->move(width(), m_mw->m_bars[0]->y() - 80);
        m_mw->m_enemys[0]->doAnim();

        m_mw->m_shieldLabel->hide();

        emit m_mw->startAll();
        close();
    });

    connect(m_exitBtn, &QPushButton::clicked, this, [=]()
    {
        exit(0);
    });
}
