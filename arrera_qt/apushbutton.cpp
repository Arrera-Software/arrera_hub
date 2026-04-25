#include "apushbutton.h"

APushButton::APushButton(QWidget *parent)
    : QPushButton(parent) {

    setMinimumHeight(40);
    setCursor(Qt::PointingHandCursor);
    setIconSize(QSize(18, 18));

    updateThemeColors();

    _anim = new QVariantAnimation(this);
    _anim->setDuration(150);
    connect(_anim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
        _currentCol = v.value<QColor>();
        update();
    });
}

void APushButton::updateThemeColors() {
    _baseCol = palette().color(QPalette::Highlight);

    _hoverCol = _baseCol.lighter(115);

    _currentCol = underMouse() ? _hoverCol : _baseCol;
}

void APushButton::changeEvent(QEvent *event) {
    QPushButton::changeEvent(event);

    if (event->type() == QEvent::PaletteChange) {
        updateThemeColors();
        update();
    }
}

void APushButton::enterEvent(QEnterEvent *event) {
    _anim->setStartValue(_currentCol);
    _anim->setEndValue(_hoverCol);
    _anim->start();
    QPushButton::enterEvent(event);
}

void APushButton::leaveEvent(QEvent *event) {
    _anim->setStartValue(_currentCol);
    _anim->setEndValue(_baseCol);
    _anim->start();
    QPushButton::leaveEvent(event);
}

void APushButton::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(_currentCol);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), rect().height() / 2.0, rect().height() / 2.0);

    QColor textColor = palette().color(QPalette::HighlightedText);
    painter.setPen(textColor);

    QFont font = this->font();
    font.setWeight(QFont::Medium);
    painter.setFont(font);

    QIcon btnIcon = icon();
    QString btnText = text();
    QSize iconSizeVal = iconSize();
    QFontMetrics fm = painter.fontMetrics();
    int spacing = 8;
    int textWidth = fm.horizontalAdvance(btnText);

    int totalContentWidth = 0;
    if (!btnIcon.isNull() && !btnText.isEmpty()) totalContentWidth = iconSizeVal.width() + spacing + textWidth;
    else if (!btnIcon.isNull()) totalContentWidth = iconSizeVal.width();
    else if (!btnText.isEmpty()) totalContentWidth = textWidth;

    int xStart = (width() - totalContentWidth) / 2;

    if (!btnIcon.isNull()) {
        int iconY = (height() - iconSizeVal.height()) / 2;

        btnIcon.paint(&painter, xStart, iconY, iconSizeVal.width(), iconSizeVal.height());
        xStart += iconSizeVal.width() + spacing;
    }

    if (!btnText.isEmpty()) {
        QRect textRect = rect();
        textRect.setLeft(xStart);
        painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, btnText);
    }
}
