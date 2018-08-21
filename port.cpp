#include "port.h"



Port::Port(QObject *parent) :
    QObject(parent),
    thisPort(new QSerialPort())
{

    this->moveToThread(new QThread()); //помещаем класс в поток
    connect(this->thread(),&QThread::started,this,&Port::process_start);
    this->thread()->start();

}

Port::~Port()
{
    qDebug("By in Thread!");
    emit finihed_Port(); // Сигнал о завершении работы


    this->thread()->quit();

    // file->close();
}

void Port::process_start()
{

    //    file = new QFile("comport_my.txt");

    //    if(file->open(QIODevice::Append)) // | QIODevice::Text
    //    {
    //        qDebug () << "File Open : comport_my.txt";
    //    }






    thisPort = new QSerialPort();


    connect(this,&Port::openPort,this,&Port::ConnectPort);
    connect(this,&Port::closePort,this,&Port::DisconnectPort);

    connect(thisPort,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(handleError(QSerialPort::SerialPortError))); // подключаем првоерку ошибок порта
    connect(thisPort, &QSerialPort::readyRead,this,&Port::ReadInProt); //подключаем чтение с порта по сигналу readyRead()


    connect(  thisPort->thread(),SIGNAL(started()), this,SLOT(process_Port())); // Переназначения метода run
    connect(  this, SIGNAL(finihed_Port()),  thisPort->thread(),SLOT(quit())); // Переназначение метода выход
    connect(  thisPort->thread(),SIGNAL(finished()), this,SLOT(deleteLater())); // Удалить к чертям поток
    connect(  this,SIGNAL(finihed_Port()),  thisPort->thread(),SLOT(deleteLater())); // Удалить к чертям поток


    connect(this,SIGNAL(writeData(QByteArray)),this,SLOT(WriteToPort(QByteArray))); // отправить данные


    qRegisterMetaType<QSerialPort::SerialPortError>();
    qRegisterMetaType<QVector<int>>();


    auto listPortInfo = QSerialPortInfo::availablePorts();
    QStringList listPort;

    foreach (auto item, listPortInfo) {
            listPort.append(item.portName());
    }



   emit findPort(listPort);


}


void Port::process_Port() //Выполняется при старте класса
{

    qDebug("Open the port.cpp on the new Thread");
    connect(thisPort,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(handleError(QSerialPort::SerialPortError))); // подключаем првоерку ошибок порта
    // connect(thisPort, SIGNAL(readyRead()),this,SLOT(ReadInProt())); //подключаем чтение с порта по сигналу readyRead()




}

void Port::Write_Setting_Port(QString name, int baudrate, int DataBits, int parity, int StopBits, int FlowControl) //заносим параметры порта в структуру данных
{
    SettingsPort.name = name;

    SettingsPort.baudRate = static_cast<QSerialPort::BaudRate>(baudrate);
    SettingsPort.dataBits = static_cast<QSerialPort::DataBits>(DataBits);
    SettingsPort.parity = static_cast<QSerialPort::Parity>(parity);
    SettingsPort.stopBits = static_cast<QSerialPort::StopBits>(StopBits);
    SettingsPort.flowControl = static_cast<QSerialPort::FlowControl>(FlowControl);

}


void Port::ConnectPort(void) //Процедура подключения
{
    qDebug() << "OPEN the PORT " << SettingsPort.name;
    thisPort->setPortName(SettingsPort.name);



    if(thisPort->open(QIODevice::ReadWrite))
    {
        if(thisPort->setBaudRate(SettingsPort.baudRate) && thisPort->setDataBits(SettingsPort.dataBits)&& thisPort->setParity(SettingsPort.parity)&& thisPort->setStopBits(SettingsPort.stopBits)&& thisPort->setFlowControl(SettingsPort.flowControl))
        {
            if(thisPort->isOpen())
            {
                qDebug() << SettingsPort.name + " >> Open!\r";
                //error_((SettingsPort.name + " >> Открыт!\r").toLocal8Bit());
                emit isOpen(true);
            }
        }
        else
        {
            thisPort->close();
            qDebug() << SettingsPort.name + " >> Don't Open!\r";
            // error_(thisPort->errorString().toLocal8Bit());
            emit isOpen(false);
        }
    }else
    {
        thisPort->close();
        emit isOpen(false);
        qDebug () << thisPort->errorString();
        qDebug() << SettingsPort.name + " >> Don't Open!\r";
        // error_(thisPort->errorString().toLocal8Bit());
    }
}

void Port::handleError(QSerialPort::SerialPortError error) //Проверка ошибок при работе
{
    if((thisPort->isOpen()) && (error == QSerialPort::ResourceError))
    {
        error_(thisPort->errorString().toLocal8Bit());
        DisconnectPort();
    }
}

void Port::DisconnectPort() //Отключаем порт
{

    if(thisPort->isOpen())
    {
        thisPort->close();
        emit isOpen(false);
    }
}

void Port::WriteToPort(QByteArray data)//запись данных в порт
{

    if(thisPort->isOpen())
    {
        thisPort->write(data);
        thisPort->flush();
    }
}


void Port::ReadInProt() // чтение данных из порта
{

    // thisPort->waitForReadyRead(20);

    dataBuild.append(thisPort->readAll());

    //    for(int i=0; i< dataBuild.count();i++)
    //    {
    //        QTextStream stream (file);
    //        stream << dataBuild[i];
    //    }

    emit sendData(dataBuild);

    dataBuild.clear();

}
