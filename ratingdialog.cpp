#include "ratingdialog.h"
#include "ui_ratingdialog.h"

#include <iostream>
#include <QUrl>

RatingDialog::RatingDialog(const std::string& url, QWidget *parent) :
    QDialog(parent), ui(new Ui::RatingDialog), m_url(url)
{
    ui->setupUi(this);

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(buttonAccepted()));
}

void RatingDialog::buttonAccepted()
{
    QUrl url(QString(m_url));
    //url.setHost();

    std::cout<<"Value: "<< ui->horizontalSlider->value() << std::endl;
}

RatingDialog::~RatingDialog()
{
    delete ui;
}
