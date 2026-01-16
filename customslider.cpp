#include "customslider.h"

void CustomSlider::mousePressEvent(QMouseEvent *ev)
{
    QSlider::mousePressEvent(ev);
    double pos = ev->pos().x() / (double)width();
    setValue(pos * (maximum() - minimum()) + minimum());
    emit costomSliderClicked();
}

void CustomSlider::mouseReleaseEvent(QMouseEvent *ev)
{
    QSlider::mouseReleaseEvent(ev);
    // 可以在这里添加其他逻辑
}
