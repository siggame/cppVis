#ifndef RATINGDIALOG_H
#define RATINGDIALOG_H

#include <QDialog>

namespace Ui {
class RatingDialog;
}

class RatingDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit RatingDialog(const std::string& url, QWidget *parent = 0);
    ~RatingDialog();

 private slots:
    void buttonAccepted();
    
private:
    Ui::RatingDialog *ui;
    std::string m_url;
};

#endif // RATINGDIALOG_H
