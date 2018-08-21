#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>

#include "doublecaptiontable.h"

#include "header2_tebleview.h"

#include "message31_tebleview.h"
#include "message31header_tableview.h"

#include "message3_tebleview.h"
#include "message3header_tebleview.h"

#include <port.h>
#include <rtcm.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Port* port;
    RTCM* rtcm;

public slots:
    void setNameCOM(QStringList name);
    void comIsOpen(bool);

private slots:

    void on_pushButton_clicked();

//    void getHeader1(QString type_sms,QString index_oporn_station);
//    void getHeader2(QString modiFy_Z,QString sequence_no,QString no_of_data_words,QString station_health);

private:
    Ui::MainWindow *ui;

    DoubleCaptionTable *dct;
    Header2_TebleView* dctHeader2;

    Message31Header_TableView *dctMessage31Header;
    Message31_TebleView  *dctMessage31;

    Message3Header_TebleView *dctMessage3Header;
    Message3_TebleView  *dctMessage3;
};

#endif // MAINWINDOW_H
