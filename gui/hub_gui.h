#ifndef HUB_GUI_H
#define HUB_GUI_H

#include <QMainWindow>
#include <QDesktopServices>
#include "../core/hub.h"
#include "arrera_qt/arrera_theme.h"

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

private slots:
    void on_BTN_ICONE_clicked();

    void on_IDC_GITHUB_clicked();

    void on_ID_INTERNET_clicked();

private:
    Ui::hub_gui *ui;
    Hub hub;
    Arrera_Theme theme;
    bool about_view;
};
#endif // HUB_GUI_H
