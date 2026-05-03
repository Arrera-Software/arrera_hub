#include "hub_gui.h"
#include "ui_hub_gui.h"

hub_gui::hub_gui(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::hub_gui),
    hub("https://raw.githubusercontent.com/Arrera-Software/distribution/refs/heads/main/index.json",this),
    theme(this)
{
    ui->setupUi(this);
    theme.loadThemeFromJson(":/theme/asset/theme/theme_default.json");
    about_view = false;

    connect(&hub, &Hub::depotsUpdated, this, &hub_gui::on_depots_updated);

    ui->arrera_hub->setCurrentWidget(ui->load_page);
    ui->BTN_UPDATE_DEPOS->setVisible(false);
    page_load = true;

    hub.update_depots();
}

hub_gui::~hub_gui()
{
    delete ui;
}

void hub_gui::on_BTN_ICONE_clicked()
{
    if (!page_load){
        if (!about_view) {
            ui->arrera_hub->setCurrentWidget(ui->about);
            about_view = true;
        }
        else {
            ui->arrera_hub->setCurrentWidget(ui->main);
            about_view = false;
        }
    }
}


void hub_gui::on_IDC_GITHUB_clicked()
{
    QUrl url("https://github.com/Arrera-Software/arrera_hub");
    QDesktopServices::openUrl(url);
}

void hub_gui::on_ID_INTERNET_clicked()
{
    QUrl url("https://www.arrera-software.fr/");
    QDesktopServices::openUrl(url);
}

void hub_gui::on_depots_updated(bool succes){
    ui->arrera_hub->setCurrentWidget(ui->main);
    ui->BTN_UPDATE_DEPOS->setVisible(true);
    if (succes){
        QMessageBox::information(this,"Arrera Hub","Le dépôt Arrera Hub a bien été mis à jour");
    }else{
        QMessageBox::information(this,"Arrera Hub","Le dépôt Arrera Hub n'a pas été mis à jour");
    }
    page_load = false;

}
void hub_gui::on_BTN_UPDATE_DEPOS_clicked()
{
    ui->arrera_hub->setCurrentWidget(ui->load_page);
    ui->BTN_UPDATE_DEPOS->setVisible(false);
    page_load = true;
    hub.update_depots();
}

