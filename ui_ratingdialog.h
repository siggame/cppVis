/********************************************************************************
** Form generated from reading UI file 'ratingdialog.ui'
**
** Created: Mon Mar 4 23:39:05 2013
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RATINGDIALOG_H
#define UI_RATINGDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSlider>

QT_BEGIN_NAMESPACE

class Ui_RatingDialog
{
public:
    QDialogButtonBox *buttonBox;
    QSlider *horizontalSlider;
    QLabel *label;

    void setupUi(QDialog *RatingDialog)
    {
        if (RatingDialog->objectName().isEmpty())
            RatingDialog->setObjectName(QString::fromUtf8("RatingDialog"));
        RatingDialog->resize(400, 300);
        buttonBox = new QDialogButtonBox(RatingDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(-100, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);
        horizontalSlider = new QSlider(RatingDialog);
        horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(120, 170, 160, 29));
        horizontalSlider->setMinimum(1);
        horizontalSlider->setMaximum(10);
        horizontalSlider->setOrientation(Qt::Horizontal);
        horizontalSlider->setTickPosition(QSlider::TicksBelow);
        horizontalSlider->setTickInterval(1);
        label = new QLabel(RatingDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(130, 70, 161, 71));

        retranslateUi(RatingDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), RatingDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), RatingDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(RatingDialog);
    } // setupUi

    void retranslateUi(QDialog *RatingDialog)
    {
        RatingDialog->setWindowTitle(QApplication::translate("RatingDialog", "Game Rating", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("RatingDialog", "Please rate the game", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RatingDialog: public Ui_RatingDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RATINGDIALOG_H
