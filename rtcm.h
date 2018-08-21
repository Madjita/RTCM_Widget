#ifndef RTCM_H
#define RTCM_H

#include <QObject>
#include <QThread>
#include <QDebug>

#include <iostream>
#include <cstring>

#include <QDataStream>
#include <QFile>
#include <QPair>


using namespace std;


// по 24 бита
struct Rtcm_word_1
{
    QString preambyla; //8
    QString type_sms; //6
    QString index_oporn_station; //10
};

struct Rtcm_word_2
{
    QString modiFy_Z; //[13] Модифицированный Z-счет
    QString sequence_no; //[3] № последовательности
    QString no_of_data_words; //[5] Число слов данных (длина кадра)
    QString station_health; //[3] Состояние ОС
};

struct Sms_1
{
    QString scale_factor; //масштабный коэффициент
    QString udre; // [2] ошибка дифференциальной дальности пользователя (UDRE)
    QString satellite_id; // [5]Номер спутника
    QString prc; // [16] Поправка псевдодальности
    QString rrc; //[8] Скорость приращения поправки псевдодальности
    QString iod; //[8] Признак конкретной посылки данных IOD
};

struct Sms_31
{
    QString scale_factor; //масштабный коэффициент
    QString udre; // [2] ошибка дифференциальной дальности пользователя (UDRE)
    QString satellite_id; // [5]Номер спутника
    QString prc; // [16] Поправка псевдодальности
    QString rrc; //[8] Скорость приращения поправки псевдодальности
    QString change_bit; // R
    QString time_of_day; //[7] Время данных Tb
};

struct Sms_3
{
    QString coordX_ECEF; //X-координата ECEF (3 старших байта) + X-координата ECEF (младший байт)
    QString coordY_ECEF; //Y-координата ECEF (2 старших байта) + Y-координата ECEF (2 младших байта)
    QString coordZ_ECEF; //Z-координата ECEF (старший байт) + Z-координата ECEF (3 младших байта)
};



class RTCM : public QObject
{
    Q_OBJECT
public:
    explicit RTCM(QObject *parent = nullptr);
    ~RTCM();

    int bitparity[6];
    int bitword[24];

    int bitpoint(char bt, int bp);

    QFile file;

    QByteArray bufDir;

    QStringList findString;

    int countErrParity; //Счетчик не совпадения четности


    Rtcm_word_1  rtsm_word_1;
    Rtcm_word_2  rtsm_word_2;
    QList<Sms_1> sms_1;
    QList<Sms_31> sms_31;
    QList<Sms_3> sms_3;

    QString bufMessage1;


    QString intWordToBitQString(int bitword[24]);


signals:
    void sendCadr1(QPair<QString, QStringList> cadr1);
    void sendCadr1_data(QPair<QString, QStringList> cadr1_data);

    void sendCadr31(QPair<QString, QStringList> );
    void sendCadr31_data(QPair<QString, QStringList> cadr31_data);

    void sendCadr3(QPair<QString, QStringList> );
    void sendCadr3_data(QPair<QString, QStringList> cadr31_data);

    //void sendHeader2(QString modiFy_Z,QString sequence_no,QString no_of_data_words,QString station_health);
    void sendHeader2(QPair<QString,QList<QString>> modiFy_Z_sequence_no_no_of_data_words_station_health);

    void sendMessage1(QString scale_factor,QString udre,QString satellite_id,QString prc,QString rrc,QString change_bit,QString time_of_day);

public slots:
    void process_start();

    void loadDataForFile();
    void loadDataCOM(QByteArray data);

    void work();

    void saveFile();


    void loadword(char ch1, char ch2, char ch3, char ch4, char ch5);


    Rtcm_word_1 readheader_1(int bitword[24]); // decode data in first word
    Rtcm_word_2 readheader_2(int bitword[24]); // decode data in second word
    void readmessage1(int bitword[24]);
    void readmessage31(int bitword[24]);
    void readmessage3(int bitword[24]);


    void kadr1();
    void kadr31();
    void kadr3();





    int toIntForBynaryQString(QString data);



private:
    char rawbuf[255];
    char ch;
    int k;
    signed char d;
    int l;
    int d29star = 0;
    int d30star = 1;

    bool flagHeader_1 = false;
    bool flagHeader_2 = false;


};

#endif // RTCM_H
