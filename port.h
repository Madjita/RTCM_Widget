
#ifndef PORT_H
#define PORT_H

#include <basetsd.h>
#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/qserialport.h>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QDebug>

#include <QDataStream>
#include <QFile>
#include <QThread>

typedef unsigned char byte;

struct Settings
{
    QString name;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;
};



class Port : public QObject
{
    Q_OBJECT

public:
    explicit Port(QObject *parent = 0);

    //Инициализация таймеров
    QTimer* timer_MRK_Data;


    QFile* file;

    QByteArray dataBuild;


public:
    ~Port();

public:
    QSerialPort* thisPort;
    Settings SettingsPort;



signals:
    void finihed_Port(); // Сигнал закртытия класса
    void error_(QString err); // Сигнал ошибок порта
    void outPort(QString data); // Сигнал вывода полученных данных
    void StopTimer();

    void openPort();
    void closePort();
    void findPort(QStringList);

    void isOpen(bool);


    void startTimerMrk(int);
    void stopTimerMrk();

    void writeData(QByteArray);

    void sendData(QByteArray);


public slots:
    void DisconnectPort(); // Слот отключения порта
    void ConnectPort(); // Слот подключения порта
    void Write_Setting_Port(QString name, int baudrate, int DataBits, int parity, int StopBits, int FlowControl); // Слот занесение настроек порта в класс
    void process_Port(); // Тело
    void WriteToPort(QByteArray data); // Слот отправки данных в порт

    void process_start();

private slots:
    void handleError(QSerialPort::SerialPortError error); // Слот обработки ошибок
    void ReadInProt(); //Слот чтения из порта по ReadyRead


};

#endif // PORT_H
