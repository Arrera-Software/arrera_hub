#ifndef ROUNDEDFRAME_H
#define ROUNDEDFRAME_H

#include <QFrame>
#include <QPainter>

class RoundedFrame : public QFrame
{
    Q_OBJECT
public:
    explicit RoundedFrame(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // ROUNDEDFRAME_H
