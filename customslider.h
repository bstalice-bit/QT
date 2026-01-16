#ifndef CUSTOMSLIDER_H
#define CUSTOMSLIDER_H

#include <QSlider>
#include <QMouseEvent>
#include <QCoreApplication>

class CustomSlider : public QSlider
{
    Q_OBJECT
public:
    CustomSlider(QWidget *parent = 0) : QSlider(parent) {}

protected:
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;  // 新增

signals:
    void costomSliderClicked();
};

#endif // CUSTOMSLIDER_H
