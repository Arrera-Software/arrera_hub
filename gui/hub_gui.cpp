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
}

hub_gui::~hub_gui()
{
    delete ui;
}
