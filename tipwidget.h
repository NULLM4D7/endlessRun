#ifndef TIPWIDGET_H
#define TIPWIDGET_H

#include <QPushButton>
#include <QLabel>
#include <QDialog>

class TipWidget : public QDialog
{
public:
    TipWidget(QWidget* parent = nullptr);
    QLabel* m_back;
    QPushButton* m_OKBtn;
};

#endif // TIPWIDGET_H
