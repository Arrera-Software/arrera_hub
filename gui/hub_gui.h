#ifndef HUB_GUI_H
#define HUB_GUI_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class hub_gui;
}
QT_END_NAMESPACE

class hub_gui : public QMainWindow
{
    Q_OBJECT

public:
    hub_gui(QWidget *parent = nullptr);
    ~hub_gui();

private:
    Ui::hub_gui *ui;
};
#endif // HUB_GUI_H
