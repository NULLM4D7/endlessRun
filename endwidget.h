#ifndef ENDWIDGET_H
#define ENDWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class MainWindow;

class EndWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EndWidget(QWidget *parent = nullptr);

private:
    QPushButton* m_aginBtn;
    QPushButton* m_exitBtn;
    QLabel* m_back;
    MainWindow* m_mw;
signals:
};

#endif // ENDWIDGET_H
