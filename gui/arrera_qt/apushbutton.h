#ifndef APUSHBUTTON_H
#define APUSHBUTTON_H

#include <QPushButton>
#include <QVariantAnimation>
#include <QColor>
#include <QPainter>
#include <QIcon>
#include <QEvent>

class APushButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor baseColor READ baseColor WRITE setBaseColor)
public:
    explicit APushButton(QWidget *parent = nullptr);

    QColor baseColor() const { return _baseCol; }
    void setBaseColor(const QColor &col) { _baseCol = col; _currentCol = col; update(); }
protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;
private:
    void updateThemeColors();
    QColor _baseCol;
    QColor _hoverCol;
    QColor _currentCol;
    QVariantAnimation* _anim;
};

#endif // APUSHBUTTON_H
