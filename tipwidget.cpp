#include "tipwidget.h"
#include <QTextCodec>

#define C(cnStr) QTextCodec::codecForName("GBK")->toUnicode(cnStr)
TipWidget::TipWidget(QWidget *parent)
    : QDialog{parent}
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    resize(685, 350);
    m_back = new QLabel(this);
    m_back->resize(width(), height());
    m_back->setStyleSheet(
        "background-color:rgb(255, 255, 255);"
        "border: 2px solid rgb(120, 80, 80);"
        "color: rgb(120, 80, 80);");

    m_back->setText(C(
    "游戏规则\n"
    "【操作对象】：靠近屏幕左边的鸟\n"
    "【敌方单元】：从屏幕右边出现的德丽傻\n"
    "（什么？你问俺为什么是德丽傻？因为找不到合适的素材，只好委屈一下德丽傻了（doge））\n"
    "游戏过程中会有【敌方单元】靠近【操作对象】\n"
    "当【敌方单元】与无护盾的【操作对象】接触时，损失1点HP值并获得护盾1秒，当HP值为0时结束游戏\n"
    "【操作对象】拥有护盾期间与【敌方单元】接触时不会损失HP值，将直接获得100点积分\n"
    "点击e键，【操作对象】将进行翻转，从而避免与【敌方单元】接触\n"
    "长按e键0.4秒将会让【操作对象】处于置空状态\n"
    "置空状态下【操作对象】完全不会损失HP值，但同时也无法对【敌方单元】进行攻击\n"
    "在游戏窗口内点击鼠标左键，【操作对象】将释放攻击\n"
    "该攻击只对【操作对象】对面的一定区域有效\n"
    "当攻击命中【敌方单元】时，【操作对象】将获得护盾和移动速度加成各1秒，并获得100点积分\n"
    "按下esc键可暂停游戏，再次点击esc键即可继续游戏\n"
    "按下s键可显示各对象的判断框，再次点击s键可隐藏判定框\n"
    "随着积分的增加，【敌方单元】的数量和移动速度都将增加\n"));
    m_back->setAlignment(Qt::AlignVCenter);
    m_OKBtn = new QPushButton(this);
    m_OKBtn->resize(50, 25);

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
    m_OKBtn->SETSS;
    m_OKBtn->setText(C("关闭"));
    m_OKBtn->move((width() - m_OKBtn->width()) / 2, height() - m_OKBtn->height() - 10);
}
