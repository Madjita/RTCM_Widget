#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    port = new Port();
    rtcm = new RTCM();

    connect(port,&Port::sendData,rtcm,&RTCM::loadDataCOM);


    //connect(rtcm,&RTCM::sendHeader2,this,&MainWindow::getHeader2);

    connect(port,&Port::findPort,this,&MainWindow::setNameCOM);
    connect(port,&Port::isOpen,this,&MainWindow::comIsOpen);

    QStringList listSpeedCOM;
    listSpeedCOM << "115200"<<"57600" <<"28800"<<"14400" <<"7200" <<"4200" << "3600" << "2400"<<"1200";
    ui->comboBox_2->addItems(listSpeedCOM);


    dct = new DoubleCaptionTable();
    ui->tableView->setModel(dct);
//    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setWordWrap(true);
    ui->tableView->resizeRowsToContents();

    dctHeader2 = new Header2_TebleView();
    ui->tableView_2->setModel(dctHeader2);
//    ui->tableView_2->horizontalHeader()->hide();
    ui->tableView_2->verticalHeader()->hide();
    ui->tableView_2->setWordWrap(true);
    ui->tableView_2->resizeRowsToContents();

    dctMessage31Header = new Message31Header_TableView();
    ui->tableView_3->setModel(dctMessage31Header);
    ui->tableView_3->verticalHeader()->hide();
    ui->tableView_3->setWordWrap(true);
    ui->tableView_3->resizeRowsToContents();

    dctMessage31 = new Message31_TebleView();
    ui->tableView_4->setModel(dctMessage31);
    ui->tableView_4->verticalHeader()->hide();
    ui->tableView_4->setWordWrap(true);
    ui->tableView_4->resizeRowsToContents();

    dctMessage3Header = new Message3Header_TebleView();
    ui->tableView_5->setModel(dctMessage3Header);
    ui->tableView_5->verticalHeader()->hide();
    ui->tableView_5->setWordWrap(true);
    ui->tableView_5->resizeRowsToContents();

    dctMessage3 = new Message3_TebleView();
    ui->tableView_6->setModel(dctMessage3);
    ui->tableView_6->verticalHeader()->hide();
    ui->tableView_6->setWordWrap(true);
    ui->tableView_6->resizeRowsToContents();


    connect(rtcm,&RTCM::sendCadr1_data,dct,&DoubleCaptionTable::addData);
    connect(rtcm,&RTCM::sendCadr1,dctHeader2,&Header2_TebleView::addData);

    connect(rtcm,&RTCM::sendCadr31_data,dctMessage31,&Message31_TebleView::addData);
    connect(rtcm,&RTCM::sendCadr31,dctMessage31Header,&Message31Header_TableView::addData);

    connect(rtcm,&RTCM::sendCadr3_data,dctMessage3,&Message3_TebleView::addData);
    connect(rtcm,&RTCM::sendCadr3,dctMessage3Header,&Message3Header_TebleView::addData);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setNameCOM(QStringList name)
{
    ui->comboBox->addItems(name);
}

void MainWindow::comIsOpen(bool mod)
{
    if(mod == true)
    {
        ui->pushButton->setText("Отключить");

        ui->pushButton->setStyleSheet("QPushButton{background-color: green}");
    }
    else
    {
        ui->pushButton->setText("Подключить");
        ui->pushButton->setStyleSheet("QPushButton{background-color: grey}");
    }
}

void MainWindow::on_pushButton_clicked()
{
    if(ui->pushButton->text() == "Подключить")
    {
     port->Write_Setting_Port(ui->comboBox->currentText(),ui->comboBox_2->currentText().toInt(), 8, 0, 1,0);
     emit port->openPort();
     ui->comboBox_2->removeItem(ui->comboBox_2->count());
    }
    else
    {
        emit port->closePort();
        dct->clearData();
        dctHeader2->clearData();
        dctMessage31Header->clearData();
        dctMessage31->clearData();
        dctMessage3Header->clearData();
        dctMessage3->clearData();
    }
}

  /*
void MainWindow::getHeader1(QString type_sms,QString index_oporn_station)
{

    auto type_sms_List =type_sms.split(";");
    ui->label_type_sms->setText(type_sms_List.first());
    ui->label_type_sms_binary->setText(type_sms_List.last());

    auto index_oporn_station_List =index_oporn_station.split(";");
    ui->label_index_oporn_station->setText(index_oporn_station_List.first());


}

void MainWindow::getHeader2(QString modiFy_Z, QString sequence_no, QString no_of_data_words, QString station_health)
{

    auto modiFy_Z_List =modiFy_Z.split(";");
    ui->label_modiFy_Z->setText(modiFy_Z_List.first());
    ui->label_modiFy_Z_binary->setText(modiFy_Z_List.last());

    auto sequence_no_List =sequence_no.split(";");
    ui->label_sequence_no->setText(sequence_no_List.first());
    ui->label_sequence_no_binary->setText(sequence_no_List.last());

    auto no_of_data_words_List =no_of_data_words.split(";");
    ui->label_no_of_data_words->setText(no_of_data_words_List.first());
    ui->label_no_of_data_words_binary->setText(no_of_data_words_List.last());

    auto station_health_List =station_health.split(";");
    ui->label_station_health->setText(station_health_List.first());
    ui->label_station_health_binary->setText(station_health_List.last());

}
*/
