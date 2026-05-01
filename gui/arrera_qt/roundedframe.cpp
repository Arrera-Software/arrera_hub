#include "roundedframe.h"

RoundedFrame::RoundedFrame(QWidget *parent) : QFrame(parent)
{
}

void RoundedFrame::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Utilise la palette actuelle pour dessiner le fond et la bordure
    // afin de respecter le thème sans feuille de style.
    painter.setBrush(palette().color(QPalette::Window));
    painter.setPen(QPen(palette().color(QPalette::WindowText), 2));

    // Dessine le rectangle avec les coins arrondis
    painter.drawRoundedRect(this->rect().adjusted(1, 1, -1, -1), 15.0, 15.0);
}
