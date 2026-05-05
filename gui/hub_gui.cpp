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

    connect(&hub,&Hub::connection_status,this,[this](bool out){
        is_online = out;
    });
    connect(&hub, &Hub::depotsUpdated, this, &hub_gui::on_depots_updated);
    connect(&hub,&Hub::app_installed,this,&hub_gui::on_application_installed);
    connect(&hub,&Hub::app_uninstall,this,&hub_gui::on_application_uninstalled);
    connect(&hub,&Hub::app_update,this,&hub_gui::on_application_updated);


    hub.check_connection_status();

    ui->arrera_hub->setCurrentWidget(ui->load_page);
    ui->L_PAGE_LOAD->setText("Mise à jour des dépôts");
    ui->BTN_UPDATE_DEPOS->setVisible(false);
    page_load = true;
    page_update = false;

    hub.update_depots();

    ui->BTN_SOFT_UPDATE->setText("Mise a jour");
    ui->BTN_SOFT_UPDATE->setIcon(QIcon(":/gui/asset/icone/gui/update_soft.png"));
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
    if (is_online){
        ui->arrera_hub->setCurrentWidget(ui->main);
        if (succes){
            QStringList availabe,update_soft;
            QStringList list_soft_installed = hub.get_soft_installed();
            for (QString soft : hub.get_soft_available()){
                if (hub.get_url_img(soft) != ""){
                    availabe.append(soft.toLower());
                }
            }

            for (QString soft : hub.get_soft_with_update()){
                update_soft.append(soft.toLower());
            }

            set_view_install_soft(availabe,list_soft_installed);
            set_view_update_soft(update_soft);

            QMessageBox::information(this,"Arrera Hub","Le dépôt Arrera Hub a bien été mis à jour");
        }else{
            QMessageBox::information(this,"Arrera Hub","Le dépôt Arrera Hub n'a pas été mis à jour");
        }
        page_load = false;
        ui->SOFT_STACKED->setCurrentWidget(ui->install_soft);

        ui->SOFT_STACKED->setCurrentWidget(ui->install_soft);
        ui->BTN_SOFT_UPDATE->setText("Mise a jour");
        ui->BTN_SOFT_UPDATE->setIcon(QIcon(":/gui/asset/icone/gui/update_soft.png"));
        page_update = false;
    }else {
        ui->arrera_hub->setCurrentWidget(ui->no_network);
    }
    ui->BTN_UPDATE_DEPOS->setVisible(true);
}

void hub_gui::on_application_installed(bool succes)
{
    ui->arrera_hub->setCurrentWidget(ui->main);
    if (succes){
        QMessageBox::information(this,"Arrera Hub","Application installée");
    }else {
        QMessageBox::information(this,"Arrera Hub","L'application n'a pas pu être installée");
    }
    page_load = false;
    on_BTN_UPDATE_DEPOS_clicked();
}

void hub_gui::on_application_updated(bool succes)
{
    ui->arrera_hub->setCurrentWidget(ui->main);
    if (succes){
        QMessageBox::information(this,"Arrera Hub","L'application a bien été mise à jour");
    }else {
        QMessageBox::information(this,"Arrera Hub","L'application n'a pas pu être mise à jour");
    }
    page_load = false;
    on_BTN_UPDATE_DEPOS_clicked();
}

void hub_gui::on_application_uninstalled(bool succes)
{
    ui->arrera_hub->setCurrentWidget(ui->main);
    if (succes){
        QMessageBox::information(this,"Arrera Hub","Application désinstallée");
    }else {
        QMessageBox::information(this,"Arrera Hub","L'application n'a pas pu être désinstallée");
    }
    page_load = false;
    on_BTN_UPDATE_DEPOS_clicked();
}

void hub_gui::on_BTN_UPDATE_DEPOS_clicked()
{
    hub.check_connection_status();
    ui->arrera_hub->setCurrentWidget(ui->load_page);
    ui->L_PAGE_LOAD->setText("Mise à jour des dépôts");
    ui->BTN_UPDATE_DEPOS->setVisible(false);
    page_load = true;
    hub.update_depots();
}


void hub_gui::on_BTN_SOFT_UPDATE_clicked()
{
    if (!page_update){
        ui->SOFT_STACKED->setCurrentWidget(ui->update_soft);
        ui->BTN_SOFT_UPDATE->setText("Accueil");
        ui->BTN_SOFT_UPDATE->setIcon(QIcon(":/gui/asset/icone/gui/home.png"));
        page_update = true;
    }else {
        ui->SOFT_STACKED->setCurrentWidget(ui->install_soft);
        ui->BTN_SOFT_UPDATE->setText("Mise a jour");
        ui->BTN_SOFT_UPDATE->setIcon(QIcon(":/gui/asset/icone/gui/update_soft.png"));
        page_update = false;
    }
}

void hub_gui::on_install_application(QString soft)
{
    page_load = true;
    ui->arrera_hub->setCurrentWidget(ui->load_page);
    ui->L_PAGE_LOAD->setText("Installation de "+soft+" ...");
    hub.install_software(soft);
}

void hub_gui::on_uninstall_application(QString soft)
{
    page_load = true;
    ui->arrera_hub->setCurrentWidget(ui->load_page);
    ui->L_PAGE_LOAD->setText("Désinstallation de "+soft+" ...");
    hub.uninstall_software(soft);
}

void hub_gui::on_update_application(QString soft)
{
    page_load = true;
    ui->arrera_hub->setCurrentWidget(ui->load_page);
    ui->L_PAGE_LOAD->setText("Mise à jour de "+soft+" ...");
    hub.update_software(soft);
}

void hub_gui::set_view_install_soft(QStringList list_available,QStringList list_installed){

    QLayout *layout = ui->install_soft->layout();

    if (layout != nullptr) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
        delete layout;
    }

    QVBoxLayout *layoutPrincipal = new QVBoxLayout(ui->install_soft);

    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);

    for (const QString &soft : list_available){
        RoundedFrame *frame = new RoundedFrame();

        frame->setFrameShape(QFrame::StyledPanel);
        QHBoxLayout *frameLayout = new QHBoxLayout(frame);

        QLabel *labelIcone = new QLabel();
        labelIcone->setFixedSize(64, 64);
        labelIcone->setAlignment(Qt::AlignCenter);
        labelIcone->setText("...");

        QLabel *labelNom = new QLabel(soft);
        QFont font = labelNom->font();
        font.setBold(true);
        font.setPointSize(15);
        labelNom->setFont(font);

        APushButton *btnAction = new APushButton();
        if (list_installed.contains(soft)){
            btnAction->setText("Désinstaller");
            btnAction->setIcon(QIcon(":/gui/asset/icone/gui/uninstall.png"));
            connect(btnAction, &QPushButton::clicked, this, [this, soft]() {
                on_uninstall_application(soft);
            });
        }else{
            btnAction->setText("Installer");
            btnAction->setIcon(QIcon(":/gui/asset/icone/gui/install.png"));
            connect(btnAction, &QPushButton::clicked, this, [this, soft]() {
                on_install_application(soft);
            });
        }
        btnAction->setIconSize(QSize(16, 16));

        frameLayout->addWidget(labelIcone);
        frameLayout->addWidget(labelNom);
        frameLayout->addStretch();
        frameLayout->addWidget(btnAction);

        layoutPrincipal->addWidget(frame);

        QString urlIcone = hub.get_url_img(soft);
        if (urlIcone != "error" && !urlIcone.isEmpty()) {
            QNetworkRequest request((QUrl(urlIcone)));
            QNetworkReply *reply = networkManager->get(request);

            connect(reply, &QNetworkReply::finished, this, [reply, labelIcone]() {
                if (reply->error() == QNetworkReply::NoError) {
                    QByteArray imageData = reply->readAll();
                    QPixmap pixmap;
                    if (pixmap.loadFromData(imageData)) {
                        labelIcone->setPixmap(
                            pixmap.scaled(64, 64,
                                          Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    }
                } else {
                    labelIcone->setText("Erreur\nImage");
                }
                reply->deleteLater();
            });
        } else {
            labelIcone->setText("Pas\nd'icône");
        }
    }

    layoutPrincipal->addStretch();
}

void hub_gui::set_view_update_soft(QStringList list_to_update)
{
    QLayout *layout = ui->update_soft->layout();

    if (layout != nullptr) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
        delete layout;
    }

    QVBoxLayout *layoutPrincipal = new QVBoxLayout(ui->update_soft);

    if (list_to_update.isEmpty()) {
        // Création du label pour indiquer qu'il n'y a rien à mettre à jour
        QLabel *labelNoUpdate = new QLabel("Aucune mise à jour disponible");
        QFont font = labelNoUpdate->font();
        font.setPointSize(16);
        font.setBold(true);
        labelNoUpdate->setFont(font);
        labelNoUpdate->setAlignment(Qt::AlignCenter);

        layoutPrincipal->addStretch();
        layoutPrincipal->addWidget(labelNoUpdate);
        layoutPrincipal->addStretch();

        return;
    }

    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);

    for (const QString &soft : list_to_update) {
        RoundedFrame *frame = new RoundedFrame();
        frame->setFrameShape(QFrame::StyledPanel);
        QHBoxLayout *frameLayout = new QHBoxLayout(frame);

        QLabel *labelIcone = new QLabel();
        labelIcone->setFixedSize(64, 64);
        labelIcone->setAlignment(Qt::AlignCenter);
        labelIcone->setText("...");

        QLabel *labelNom = new QLabel(soft);
        QFont font = labelNom->font();
        font.setBold(true);
        font.setPointSize(15);
        labelNom->setFont(font);

        APushButton *btnAction = new APushButton();
        btnAction->setText("Mettre à jour");

        btnAction->setIcon(QIcon(":/gui/asset/icone/gui/update_soft.png"));

        connect(btnAction, &QPushButton::clicked, this, [this, soft]() {
            on_update_application(soft);
        });

        frameLayout->addWidget(labelIcone);
        frameLayout->addWidget(labelNom);
        frameLayout->addStretch();
        frameLayout->addWidget(btnAction);

        layoutPrincipal->addWidget(frame);

        QString urlIcone = hub.get_url_img(soft);
        if (urlIcone != "error" && !urlIcone.isEmpty()) {
            QNetworkRequest request((QUrl(urlIcone)));
            QNetworkReply *reply = networkManager->get(request);

            connect(reply, &QNetworkReply::finished, this, [reply, labelIcone]() {
                if (reply->error() == QNetworkReply::NoError) {
                    QByteArray imageData = reply->readAll();
                    QPixmap pixmap;
                    if (pixmap.loadFromData(imageData)) {
                        labelIcone->setPixmap(
                            pixmap.scaled(64, 64,
                                          Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    }
                } else {
                    labelIcone->setText("Erreur\nImage");
                }
                reply->deleteLater();
            });
        } else {
            labelIcone->setText("Pas\nd'icône");
        }
    }

    layoutPrincipal->addStretch();
}
