#ifndef HUB_GUI_H
#define HUB_GUI_H

#include <QMainWindow>
#include <QDesktopServices>
#include <QMessageBox>
#include "../core/hub.h"
#include "arrera_qt/arrera_theme.h"

#include "arrera_qt/roundedframe.h"
#include "arrera_qt/apushbutton.h"

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

    void on_depots_updated(bool succes);

    void on_application_installed(bool succes);

    void on_application_updated(bool succes);

    void on_application_uninstalled(bool succes);

    void on_BTN_UPDATE_DEPOS_clicked();

    void on_BTN_SOFT_UPDATE_clicked();

    void on_install_application(QString soft);

    void on_uninstall_application(QString soft);

    void on_update_application(QString soft);

private: // Atribut
    Ui::hub_gui *ui;
    Hub hub;
    Arrera_Theme theme;
    bool about_view,page_load,page_update;
private : // Methode private
    void set_view_install_soft(QStringList list_available,QStringList list_installed);
    void set_view_update_soft(QStringList list_to_update);
};
#endif // HUB_GUI_H
