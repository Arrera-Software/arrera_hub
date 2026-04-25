#include "hub_gui.h"
#include "ui_hub_gui.h"

hub_gui::hub_gui(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::hub_gui)
{
    ui->setupUi(this);
}

hub_gui::~hub_gui()
{
    delete ui;
}
