#include "rtcm.h"


RTCM::RTCM(QObject *parent) : QObject(parent),
    countErrParity(0)
{
    //        this->moveToThread(new QThread()); //помещаем класс в поток
    //        connect(this->thread(),&QThread::started,this,&RTCM::process_start);
    //        this->thread()->start();

    process_start();
}

RTCM::~RTCM()
{
    file.close();
    this->thread()->quit();
}


int RTCM::bitpoint(char bt, int bp)
{
    return (bt >> bp) & 0x01;
}

QString RTCM::intWordToBitQString(int bitword[24])
{
    QString str = "";

    for(int k=0;k < 24;k++)
    {
        str += QString::number(bitword[k]);
    }

    return str;
}


void RTCM::loadword(char ch1, char ch2, char ch3, char ch4, char ch5){

    int cnt;

    if (d30star == 0){
        for (cnt = 0; cnt < 6; cnt++){
            bitword[cnt] = bitpoint(ch1, cnt);
        }
        for (cnt = 0; cnt < 6; cnt++){
            bitword[cnt + 6] = bitpoint(ch2, cnt);
        }
        for (cnt = 0; cnt < 6; cnt++){
            bitword[cnt + 12] = bitpoint(ch3, cnt);
        }
        for (cnt = 0; cnt < 6; cnt++){
            bitword[cnt + 18] = bitpoint(ch4, cnt);
        }
    }
    else {
        for (cnt = 0; cnt < 6; cnt++){
            bitword[cnt] = 1 - bitpoint(ch1, cnt);
        }
        for (cnt = 0; cnt < 6; cnt++){
            bitword[cnt + 6] = 1 - bitpoint(ch2, cnt);
        }
        for (cnt = 0; cnt < 6; cnt++){
            bitword[cnt + 12] = 1 - bitpoint(ch3, cnt);
        }
        for (cnt = 0; cnt < 6; cnt++){
            bitword[cnt + 18] = 1 - bitpoint(ch4, cnt);
        }
    }

    for (cnt = 0; cnt < 6; cnt++){
        bitparity[cnt] = bitpoint(ch5, cnt);
    }

    //    d29star = bitparity[4];
    //    d30star = bitparity[5];
}

Rtcm_word_1 RTCM::readheader_1(int bitword[24])
{
    // Rtcm_word_1 data;
    QString bitStr = intWordToBitQString(bitword);

    rtsm_word_1.preambyla.clear();
    rtsm_word_1.type_sms.clear();
    rtsm_word_1.index_oporn_station.clear();

    for(int i=0; i < 8;i++)
    {
        rtsm_word_1.preambyla.append(bitStr[i]);

    }
    bitStr.remove(0,8);

    for(int i=0; i < 6;i++)
    {
        rtsm_word_1.type_sms.append(bitStr[i]);

    }
    bitStr.remove(0,6);

    rtsm_word_1.index_oporn_station = bitStr;

    flagHeader_1 = true;

    return rtsm_word_1;
}

Rtcm_word_2 RTCM::readheader_2(int bitword[24])
{
    // Rtcm_word_2 data;
    QString bitStr = intWordToBitQString(bitword);

    rtsm_word_2.modiFy_Z.clear();
    rtsm_word_2.sequence_no.clear();
    rtsm_word_2.no_of_data_words.clear();
    rtsm_word_2.station_health.clear();

    for(int i=0; i < 13;i++)
    {
        rtsm_word_2.modiFy_Z.append(bitStr[i]);

    }
    bitStr.remove(0,13);

    for(int i=0; i < 3;i++)
    {
        rtsm_word_2.sequence_no.append(bitStr[i]);

    }
    bitStr.remove(0,3);

    for(int i=0; i < 5;i++)
    {
        rtsm_word_2.no_of_data_words.append(bitStr[i]);
    }
    bitStr.remove(0,5);

    rtsm_word_2.station_health = bitStr;

    flagHeader_2 = true;

    float modiFy_Z = rtsm_word_2.modiFy_Z.toInt(nullptr,2)*0.6;
    int sequence_no =  rtsm_word_2.sequence_no.toInt(nullptr,2);
    int lenghtMessage = rtsm_word_2.no_of_data_words.toInt(nullptr,2);

    QString indication_code; //REFERENCE STATION HEALTH STATUS* INDICATOR

    switch (sequence_no) {
    case 0:{
        indication_code = "UDRE Scale Factor = 1";
        break;
    }
    case 1:{
        indication_code = "UDRE Scale Factor = 0.75";
        break;
    }
    case 2:{
        indication_code = "UDRE Scale Factor = 0.5";
        break;
    }
    case 3:{
        indication_code = "UDRE Scale Factor = 0.3";
        break;
    }
    case 4:{
        indication_code = "UDRE Scale Factor = 0.2";
        break;
    }
    case 5:{
        indication_code = "UDRE Scale Factor = 0.1";
        break;
    }
    case 6:{
        indication_code = "Reference Station Transmission Not Monitored";
        break;
    }
    case 7:{
        indication_code = "Reference Station Not Working";
        break;
    }
    }

    QPair<QString,QList<QString>> list;

    list.first = QString::number(modiFy_Z);

    list.second << indication_code << QString::number(lenghtMessage) << QString::number(rtsm_word_2.station_health.toInt(nullptr,2));

    emit sendHeader2(list);


    //     emit sendHeader2(QString::number(modiFy_Z)+";"+rtsm_word_2.modiFy_Z,indication_code+";"+rtsm_word_2.sequence_no,QString::number(lenghtMessage)+";"+rtsm_word_2.no_of_data_words,QString::number(rtsm_word_2.station_health.toInt(nullptr,2))+";"+rtsm_word_2.station_health);

    return rtsm_word_2;
}

int lol24 = 0;

void RTCM::readmessage1(int bitword[24])
{
    QString bitStr = intWordToBitQString(bitword);

    int lenghtMessage = rtsm_word_2.no_of_data_words.toInt(0,2);

    if(bufMessage1.count() !=  (lenghtMessage*bitStr.count()))
    {
        bufMessage1.append(bitStr);

        if(bufMessage1.count() == (lenghtMessage*bitStr.count()))
        {
            sms_1.clear();

            QString duf = bufMessage1;
            while(bufMessage1.count() > 16)
            {
                //Посмотреть на конец 96 - (40*2) = 15 посмотреть на 15 последних бит
                Sms_1 sms1;

                sms1.scale_factor = bufMessage1[0];
                bufMessage1.remove(0,1);

                for(int i=0; i< 2;i++)
                {
                    sms1.udre.append(bufMessage1[i]);
                }
                bufMessage1.remove(0,2);
                for(int i=0; i< 5;i++)
                {
                    sms1.satellite_id.append(bufMessage1[i]);
                }
                bufMessage1.remove(0,5);
                for(int i=0; i< 16;i++)
                {
                    sms1.prc.append(bufMessage1[i]);
                }
                bufMessage1.remove(0,16);
                for(int i=0; i< 8;i++)
                {
                    sms1.rrc.append(bufMessage1[i]);
                }
                bufMessage1.remove(0,8);
                for(int i=0; i< 8;i++)
                {
                    sms1.iod.append(bufMessage1[i]);
                }
                bufMessage1.remove(0,8);

                //Разбор сообщения 1
                QString scale_factor_sms1_QString;
                int scale_factor_sms1 = sms1.scale_factor.toInt(nullptr,2);
                switch (scale_factor_sms1) {
                case 0:{
                    scale_factor_sms1_QString="Scale factor for pseudorange correction is 0.02 meter and for range rate correction is 0.002 meter/second";
                    break;
                }
                case 1:{
                    scale_factor_sms1_QString="Scale factor for pseudorange correction is 0.32 meter and for range rate correction is 0.032 meter/second";
                    break;
                }
                }

                QString udre_sms1_QString;
                int udre_sms1 = sms1.udre.toInt(nullptr,2);
                switch (udre_sms1) {
                case 0:{
                    udre_sms1_QString="≤ 1 meter";
                    break;
                }
                case 1:{
                    udre_sms1_QString="> 1 meter and ≤ 4 meters";
                    break;
                }
                case 2:{
                    udre_sms1_QString="> 4 meters and ≤ 8 meters";
                    break;
                }
                case 3:{
                    udre_sms1_QString="> 8 meters";
                    break;
                }
                }

                QString satellite_id_sms1_QString;
                int satellite_id_sms1 = sms1.satellite_id.toInt(nullptr,2);
                if(satellite_id_sms1 == 0)
                {
                    satellite_id_sms1 = 32;
                }

                QString prc_sms1_QString;
                float prc_sms1 =0;
                if(sms1.prc != "1000000000000000")
                {
                    if(scale_factor_sms1 == 0)
                    {
                        prc_sms1 = sms1.prc.toInt(nullptr,2)*0.02; //метры
                    }
                    else
                    {
                        prc_sms1 = sms1.prc.toInt(nullptr,2)*0.32; //метры
                    }

                    prc_sms1_QString = QString::number(prc_sms1,'f',2)+ " m";
                }
                else
                {
                    //Error
                    prc_sms1_QString ="Binary 1000 0000 0000 0000 indicates a problem and the User Equipment should immediately stop using this satellite";
                    prc_sms1 = 32768;
                }



                QString rrc_sms1_QString;
                float rrc_sms1 = 0;
                if(sms1.rrc != "10000000")
                {
                    if(scale_factor_sms1 == 0)
                    {
                        rrc_sms1 = sms1.rrc.toInt(nullptr,2)*0.32; //метры
                    }
                    else
                    {
                        rrc_sms1 = sms1.rrc.toInt(nullptr,2)*0.032; //метры
                    }

                    rrc_sms1_QString = QString::number(rrc_sms1,'f',2) + " m/s";
                }
                else
                {
                    //Error
                    rrc_sms1_QString ="Binary 1000 0000 indicates a problem and the User Equipment should immediately stop using this satellite";
                    rrc_sms1 = 128;
                }

                QString iod_sms1_QString;
                float iod_sms1 = sms1.iod.toInt(nullptr,2);


                void sendMessage1(QString scale_factor,QString udre,QString satellite_id,QString prc,QString rrc,QString change_bit,QString time_of_day);


                /////////////////////////////








                sms_1.append(sms1);
            }

            kadr1();

            bufMessage1.clear();
            flagHeader_1 = false;
            flagHeader_2 = false;
        }
    }

}

void RTCM::readmessage31(int bitword[])
{
    QString bitStr = intWordToBitQString(bitword);

    int lenghtMessage = rtsm_word_2.no_of_data_words.toInt(0,2);

    if(bufMessage1.count() !=  (lenghtMessage*bitStr.count()))
    {
        bufMessage1.append(bitStr);

        if(bufMessage1.count() == (lenghtMessage*bitStr.count()))
        {
            sms_31.clear();

            QString duf = bufMessage1;
            while(bufMessage1.count() > 16)
            {

                Sms_31 sms31;

                sms31.scale_factor = bufMessage1[0];
                bufMessage1.remove(0,1);

                for(int i=0; i< 2;i++)
                {
                    sms31.udre.append(bufMessage1[i]);
                }
                bufMessage1.remove(0,2);
                for(int i=0; i< 5;i++)
                {
                    sms31.satellite_id.append(bufMessage1[i]);
                }
                bufMessage1.remove(0,5);
                for(int i=0; i< 16;i++)
                {
                    sms31.prc.append(bufMessage1[i]);
                }
                bufMessage1.remove(0,16);
                for(int i=0; i< 8;i++)
                {
                    sms31.rrc.append(bufMessage1[i]);
                }
                bufMessage1.remove(0,8);

                sms31.change_bit = bufMessage1[0];
                bufMessage1.remove(0,1);

                for(int i=0; i< 7;i++)
                {
                    sms31.time_of_day.append(bufMessage1[i]);
                }
                bufMessage1.remove(0,7);




                //Разбор сообщения 1
                QString scale_factor_sms31_QString;
                int scale_factor_sms31 = sms31.scale_factor.toInt(nullptr,2);
                switch (scale_factor_sms31) {
                case 0:{
                    scale_factor_sms31_QString="Scale factor for pseudorange correction is 0.02 meter and for range rate correction is 0.002 meter/second";
                    break;
                }
                case 1:{
                    scale_factor_sms31_QString="Scale factor for pseudorange correction is 0.32 meter and for range rate correction is 0.032 meter/second";
                    break;
                }
                }

                QString udre_sms31_QString;
                int udre_sms31 = sms31.udre.toInt(nullptr,2);
                switch (udre_sms31) {
                case 0:{
                    udre_sms31_QString="≤ 1 meter";
                    break;
                }
                case 1:{
                    udre_sms31_QString="> 1 meter and ≤ 4 meters";
                    break;
                }
                case 2:{
                    udre_sms31_QString="> 4 meters and ≤ 8 meters";
                    break;
                }
                case 3:{
                    udre_sms31_QString="> 8 meters";
                    break;
                }
                }

                QString satellite_id_sms31_QString;
                int satellite_id_sms31= sms31.satellite_id.toInt(nullptr,2);
                if(satellite_id_sms31 == 0)
                {
                    satellite_id_sms31 = 32;
                }

                QString prc_sms31_QString;
                float prc_sms31 =0;
                if(sms31.prc != "1000000000000000")
                {
                    if(scale_factor_sms31 == 0)
                    {
                        prc_sms31 = sms31.prc.toInt(nullptr,2)*0.02; //метры
                    }
                    else
                    {
                        prc_sms31 = sms31.prc.toInt(nullptr,2)*0.32; //метры
                    }

                    prc_sms31_QString = QString::number(prc_sms31,'f',2)+ " m";
                }
                else
                {
                    //Error
                    prc_sms31_QString ="Binary 1000 0000 0000 0000 indicates a problem and the User Equipment should immediately stop using this satellite";
                    prc_sms31 = 32768;
                }



                QString rrc_sms31_QString;
                float rrc_sms31 = 0;
                if(sms31.rrc != "10000000")
                {
                    if(scale_factor_sms31 == 0)
                    {
                        rrc_sms31 = sms31.rrc.toInt(nullptr,2)*0.32; //метры
                    }
                    else
                    {
                        rrc_sms31 = sms31.rrc.toInt(nullptr,2)*0.032; //метры
                    }

                    rrc_sms31_QString = QString::number(rrc_sms31,'f',2) + " m/s";
                }
                else
                {
                    //Error
                    rrc_sms31_QString ="Binary 1000 0000 indicates a problem and the User Equipment should immediately stop using this satellite";
                    rrc_sms31 = 128;
                }

                QString change_bit_sms31_QString;
                int change_bit_sms31 = sms31.change_bit.toInt(nullptr,2);

                QString time_of_day_sms31_QString;
                int time_of_day_sms31 = sms31.time_of_day.toInt(nullptr,2)*30; //период умноженный на 30 cек

                /////////////////////////////


                sms_31.append(sms31);
            }

            kadr31();
            bufMessage1.clear();
            flagHeader_1 = false;
            flagHeader_2 = false;
        }
    }
}

void RTCM::readmessage3(int bitword[])
{
    QString bitStr = intWordToBitQString(bitword);

    int lenghtMessage = rtsm_word_2.no_of_data_words.toInt(0,2);

    if(bufMessage1.count() !=  (lenghtMessage*bitStr.count()))
    {
        bufMessage1.append(bitStr);

        if(bufMessage1.count() == (lenghtMessage*bitStr.count()))
        {
            sms_3.clear();

            QString duf = bufMessage1;

            Sms_3 sms3;

            for(int i=0; i< 24;i++)
            {
                sms3.coordX_ECEF.append(bufMessage1[i]);
            }
            bufMessage1.remove(0,24);
            for(int i=0; i< 8;i++)
            {
                sms3.coordX_ECEF.append(bufMessage1[i]);
            }
            bufMessage1.remove(0,8);
            for(int i=0; i< 16;i++)
            {
                sms3.coordY_ECEF.append(bufMessage1[i]);
            }
            bufMessage1.remove(0,16);
            for(int i=0; i< 16;i++)
            {
                sms3.coordY_ECEF.append(bufMessage1[i]);
            }
            bufMessage1.remove(0,16);
            for(int i=0; i< 8;i++)
            {
                sms3.coordZ_ECEF.append(bufMessage1[i]);
            }
            bufMessage1.remove(0,8);
            for(int i=0; i< 24;i++)
            {
                sms3.coordZ_ECEF.append(bufMessage1[i]);
            }
            bufMessage1.remove(0,24);


            sms_3.append(sms3);



            kadr3();
            bufMessage1.clear();
            flagHeader_1 = false;
            flagHeader_2 = false;
        }
    }
}

void RTCM::kadr1()
{
    QPair<QString, QStringList> list_type_smsANDindex_oporn_station;
    QStringList list;

    int index_oporn_station = rtsm_word_1.index_oporn_station.toInt(nullptr,2);
    int type_sms = rtsm_word_1.type_sms.toInt(nullptr,2);


    float modiFy_Z = rtsm_word_2.modiFy_Z.toInt(nullptr,2)*0.6;
    int sequence_no =  rtsm_word_2.sequence_no.toInt(nullptr,2);
    int lenghtMessage = rtsm_word_2.no_of_data_words.toInt(nullptr,2);
    int helth =  rtsm_word_2.sequence_no.toInt(nullptr,2);

    QString indication_code; //REFERENCE STATION HEALTH STATUS* INDICATOR

    switch (helth) {
    case 0:{
        indication_code = "1 = UDRE Scale Factor";
        break;
    }
    case 1:{
        indication_code = "0.75 = UDRE Scale Factor";
        break;
    }
    case 2:{
        indication_code = "0.5 = UDRE Scale Factor";
        break;
    }
    case 3:{
        indication_code = "0.3 = UDRE Scale Factor";
        break;
    }
    case 4:{
        indication_code = "0.2 = UDRE Scale Factor";
        break;
    }
    case 5:{
        indication_code = "0.1 = UDRE Scale Factor";
        break;
    }
    case 6:{
        indication_code = "Reference Station Transmission Not Monitored";
        break;
    }
    case 7:{
        indication_code = "Reference Station Not Working";
        break;
    }
    }


    list <<QString::number(index_oporn_station);
    list << QString::number(modiFy_Z);
    list << QString::number(sequence_no) << QString::number(lenghtMessage) << indication_code;


    list_type_smsANDindex_oporn_station.first = QString::number(type_sms);



    //////Разбор/////////////

    for (int i=0;i < sms_1.count();i++)
    {

        //Разбор сообщения 1
        QString scale_factor_sms1_QString;
        int scale_factor_sms1 = sms_1.value(i).scale_factor.toInt(nullptr,2);
        switch (scale_factor_sms1) {
        case 0:{
            scale_factor_sms1_QString="0.02 м и 0.002 м/с";
            break;
        }
        case 1:{
            scale_factor_sms1_QString="0.32 м 0.032 м/с";
            break;
        }
        }

        QString udre_sms1_QString;
        int udre_sms1 = sms_1.value(i).udre.toInt(nullptr,2);
        switch (udre_sms1) {
        case 0:{
            udre_sms1_QString=" ≤ 1 м";
            break;
        }
        case 1:{
            udre_sms1_QString=" > 1 м и ≤ 4 м";
            break;
        }
        case 2:{
            udre_sms1_QString=" > 4 м и ≤ 8 м";
            break;
        }
        case 3:{
            udre_sms1_QString=" > 8 м";
            break;
        }
        }

        QString satellite_id_sms1_QString;
        int satellite_id_sms1 = sms_1.value(i).satellite_id.toInt(nullptr,2);
        if(satellite_id_sms1 == 0)
        {
            satellite_id_sms1 = 32;
        }
        satellite_id_sms1_QString = QString::number(satellite_id_sms1);

        QString prc_sms1_QString;
        float prc_sms1 =0;
        if(sms_1.value(i).prc != "1000000000000000")
        {
            if(scale_factor_sms1 == 0)
            {
                prc_sms1 = sms_1.value(i).prc.toInt(nullptr,2)*0.02; //метры
            }
            else
            {
                prc_sms1 = sms_1.value(i).prc.toInt(nullptr,2)*0.32; //метры
            }

            prc_sms1_QString = QString::number(prc_sms1,'f',2)+ " м";
        }
        else
        {
            //Error
            prc_sms1_QString ="Binary 1000 0000 0000 0000 indicates a problem and the User Equipment should immediately stop using this satellite";
            prc_sms1 = 32768;
        }



        QString rrc_sms1_QString;
        float rrc_sms1 = 0;
        if(sms_1.value(i).rrc != "10000000")
        {
            if(scale_factor_sms1 == 0)
            {
                rrc_sms1 = sms_1.value(i).rrc.toInt(nullptr,2)*0.32; //метры
            }
            else
            {
                rrc_sms1 = sms_1.value(i).rrc.toInt(nullptr,2)*0.032; //метры
            }

            rrc_sms1_QString = QString::number(rrc_sms1,'f',2) + " м/с";
        }
        else
        {
            //Error
            rrc_sms1_QString ="Binary 1000 0000 indicates a problem and the User Equipment should immediately stop using this satellite";
            rrc_sms1 = 128;
        }

        QString iod_sms1_QString;
        float iod_sms1 = sms_1.value(i).iod.toInt(nullptr,2);
        iod_sms1_QString = QString::number(iod_sms1);


        QPair<QString, QStringList> list_sms_1;

        list_sms_1.first = satellite_id_sms1_QString;
        list_sms_1.second  << scale_factor_sms1_QString <<  udre_sms1_QString << prc_sms1_QString <<rrc_sms1_QString << iod_sms1_QString;

        emit sendCadr1_data(list_sms_1);

    }

    list_type_smsANDindex_oporn_station.second = list;

    emit sendCadr1(list_type_smsANDindex_oporn_station);


}

void RTCM::kadr31()
{
    QPair<QString, QStringList> card31_list;
    QStringList list;

    int index_oporn_station = rtsm_word_1.index_oporn_station.toInt(nullptr,2);
    int type_sms = rtsm_word_1.type_sms.toInt(nullptr,2);


    float modiFy_Z = rtsm_word_2.modiFy_Z.toInt(nullptr,2)*0.6;
    int sequence_no =  rtsm_word_2.sequence_no.toInt(nullptr,2);
    int lenghtMessage = rtsm_word_2.no_of_data_words.toInt(nullptr,2);
    int helth =  rtsm_word_2.sequence_no.toInt(nullptr,2);

    QString indication_code; //REFERENCE STATION HEALTH STATUS* INDICATOR

    switch (helth) {
    case 0:{
        indication_code = "1 = UDRE Scale Factor";
        break;
    }
    case 1:{
        indication_code = "0.75 = UDRE Scale Factor";
        break;
    }
    case 2:{
        indication_code = "0.5 = UDRE Scale Factor";
        break;
    }
    case 3:{
        indication_code = "0.3 = UDRE Scale Factor";
        break;
    }
    case 4:{
        indication_code = "0.2 = UDRE Scale Factor";
        break;
    }
    case 5:{
        indication_code = "0.1 = UDRE Scale Factor";
        break;
    }
    case 6:{
        indication_code = "Reference Station Transmission Not Monitored";
        break;
    }
    case 7:{
        indication_code = "Reference Station Not Working";
        break;
    }
    }


    list <<QString::number(index_oporn_station);
    list << QString::number(modiFy_Z);
    list << QString::number(sequence_no) << QString::number(lenghtMessage) << indication_code;


    for (int i=0;i < sms_31.count();i++) {

        //Разбор сообщения 1
        QString scale_factor_sms31_QString;
        int scale_factor_sms31 = sms_31.value(i).scale_factor.toInt(nullptr,2);
        switch (scale_factor_sms31) {
        case 0:{
            scale_factor_sms31_QString="0.02 м и 0.002 м/с";
            break;
        }
        case 1:{
            scale_factor_sms31_QString="0.32 м и 0.032 м/с";
            break;
        }
        }

        QString udre_sms31_QString;
        int udre_sms31 = sms_31.value(i).udre.toInt(nullptr,2);
        switch (udre_sms31) {
        case 0:{
            udre_sms31_QString=" ≤ 1 м";
            break;
        }
        case 1:{
            udre_sms31_QString=" > 1 м и ≤ 4 м";
            break;
        }
        case 2:{
            udre_sms31_QString=" > 4 м и ≤ 8 м";
            break;
        }
        case 3:{
            udre_sms31_QString=" > 8 м";
            break;
        }
        }

        QString satellite_id_sms31_QString;
        int satellite_id_sms31= sms_31.value(i).satellite_id.toInt(nullptr,2);
        if(satellite_id_sms31 == 0)
        {
            satellite_id_sms31 = 32;
        }
        satellite_id_sms31_QString = QString::number(satellite_id_sms31);

        QString prc_sms31_QString;
        float prc_sms31 =0;
        if(sms_31.value(i).prc != "1000000000000000")
        {
            if(scale_factor_sms31 == 0)
            {
                prc_sms31 = sms_31.value(i).prc.toInt(nullptr,2)*0.02; //метры
            }
            else
            {
                prc_sms31 = sms_31.value(i).prc.toInt(nullptr,2)*0.32; //метры
            }

            prc_sms31_QString = QString::number(prc_sms31,'f',2)+ " м";
        }
        else
        {
            //Error
            prc_sms31_QString ="Binary 1000 0000 0000 0000 indicates a problem and the User Equipment should immediately stop using this satellite";
            prc_sms31 = 32768;
        }



        QString rrc_sms31_QString;
        float rrc_sms31 = 0;
        if(sms_31.value(i).rrc != "10000000")
        {
            if(scale_factor_sms31 == 0)
            {
                rrc_sms31 = sms_31.value(i).rrc.toInt(nullptr,2)*0.32; //метры
            }
            else
            {
                rrc_sms31 = sms_31.value(i).rrc.toInt(nullptr,2)*0.032; //метры
            }

            rrc_sms31_QString = QString::number(rrc_sms31,'f',2) + " м/с";
        }
        else
        {
            //Error
            rrc_sms31_QString ="Binary 1000 0000 indicates a problem and the User Equipment should immediately stop using this satellite";
            rrc_sms31 = 128;
        }

        QString change_bit_sms31_QString;
        int change_bit_sms31 = sms_31.value(i).change_bit.toInt(nullptr,2);
        change_bit_sms31_QString = QString::number(change_bit_sms31);

        QString time_of_day_sms31_QString;
        int time_of_day_sms31 = sms_31.value(i).time_of_day.toInt(nullptr,2)*30; //период умноженный на 30 cек
        time_of_day_sms31_QString = QString::number(time_of_day_sms31);

        /////////////////////////////

        QPair<QString, QStringList> list_sms_31;

        list_sms_31.first = satellite_id_sms31_QString;
        list_sms_31.second  << scale_factor_sms31_QString <<  udre_sms31_QString << prc_sms31_QString <<rrc_sms31_QString << change_bit_sms31_QString << time_of_day_sms31_QString;

        emit sendCadr31_data(list_sms_31);


    }


    card31_list.first = QString::number(type_sms);
    card31_list.second = list;

    emit sendCadr31(card31_list);

}

void RTCM::kadr3()
{
    QPair<QString, QStringList> card3_list;
    QStringList list;

    int index_oporn_station = rtsm_word_1.index_oporn_station.toInt(nullptr,2);
    int type_sms = rtsm_word_1.type_sms.toInt(nullptr,2);


    float modiFy_Z = rtsm_word_2.modiFy_Z.toInt(nullptr,2)*0.6;
    int sequence_no =  rtsm_word_2.sequence_no.toInt(nullptr,2);
    int lenghtMessage = rtsm_word_2.no_of_data_words.toInt(nullptr,2);
    int helth =  rtsm_word_2.sequence_no.toInt(nullptr,2);

    QString indication_code; //REFERENCE STATION HEALTH STATUS* INDICATOR

    switch (helth) {
    case 0:{
        indication_code = "1 = UDRE Scale Factor";
        break;
    }
    case 1:{
        indication_code = "0.75 = UDRE Scale Factor";
        break;
    }
    case 2:{
        indication_code = "0.5 = UDRE Scale Factor";
        break;
    }
    case 3:{
        indication_code = "0.3 = UDRE Scale Factor";
        break;
    }
    case 4:{
        indication_code = "0.2 = UDRE Scale Factor";
        break;
    }
    case 5:{
        indication_code = "0.1 = UDRE Scale Factor";
        break;
    }
    case 6:{
        indication_code = "Reference Station Transmission Not Monitored";
        break;
    }
    case 7:{
        indication_code = "Reference Station Not Working";
        break;
    }
    }


    list <<QString::number(index_oporn_station);
    list << QString::number(modiFy_Z);
    list << QString::number(sequence_no) << QString::number(lenghtMessage) << indication_code;


    //точность после запятой 2 знака (в метрах)
    double x = toIntForBynaryQString(sms_3.first().coordX_ECEF)*0.01;//meters

    double y = toIntForBynaryQString(sms_3.first().coordY_ECEF)*0.01;//meters

    double z = toIntForBynaryQString(sms_3.first().coordZ_ECEF)*0.01; //meters



    QPair<QString, QStringList> list_sms_3;

    list_sms_3.second  << QString::number(x,'f',2) <<  QString::number(y,'f',2);
    list_sms_3.first = QString::number(z,'f',2);


    emit sendCadr3_data(list_sms_3);


    card3_list.first = QString::number(type_sms);
    card3_list.second = list;

    emit sendCadr3(card3_list);
}

int RTCM::toIntForBynaryQString(QString data)
{
    int x=0;
    for(int i=0; i < data.count();i++)
    {
        if(data[i] == "1")
        {
            x |= (1 << (31-i));
        }
        else
        {
            x |= (0 << (31-i));
        }

    }

    return x;
}

void RTCM::process_start()
{

    //    memset(&rtsm_word_1,0,sizeof(rtsm_word_1));
    //    memset(&rtsm_word_2,0,sizeof(rtsm_word_2));

    //    QString str =
    //            "59 7E 7D 5F 66 66 4A 5D 7E 5C 60 7D 67 7C 4D 40 68 4A 64 5D 7F 49 42 40 7C 7B 7F 7B 7F 76 5F 7C 77 68 7A 47 7D 4F 65 41 40 4A 4C 4C 51 40 52 7D 4F 68 6D 7C 75 7F 55 75 41 48 71 76 67 7D 77 73 52 40 41 4C 40 51 66 79 43 60 63 66 4A 55 7C 4D 68 41 40 7B 67 7F 6F 72 45 56 40 58 48 60 59 54 43 46 40 74 77 7D 7B 4A 44 70 42 40 59 51 5F 51 4D 7C 72 40 7E 72 6F 6C 6B 7C 78 7F 64 4E 7C 77 4A 48 50 40 40 63 64 40 6E 72 47 6C 40 40 73 5F 7C 6B 7C 7F 7F 4F 66 41 43 60 53 59 75 46 41 4E 7F 7D 68 6D 6F 74 5F 55 47 72 7D 7B 7F 41 74 79 49 4A 4E 55 66 41 42 60 59 59 43 6E 41 71 5F 42 58 63 65 7F 57 75 5B 47 7F 49 42 40 59 44 40 44 40 6C 5F 7C 7F 68 43 78 42 70 46 79 7F 75 73 73 6E 7F 6D 42 70 57 52 43 4A 40 6A 4A 7E 77 4E 49 58 42 48 4C 6D 7F 7D 73 7F 54 66 79 43 60 63 66 7C 5F 7C 7E 57 7E 7F 78 6D 7F 6F 72 45 56 40 58 4C 60 67 6B 7C 79 7F 4B 48 41 44 75 41 70 42 40 61 7E 60 6E 72 43 68 40 7E 74 6F 68 6B 7C 78 7F 64 76 7F 77 4A 7A 6F 7F 7F 7C 69 40 6E 72 47 6C 40 40 73 5F 7C 6B 7C 7F 7F 4F 66 41 43 60 53 59 43 48 41 43 7F 7D 68 6D 6F 74 5F 55 47 72 7D 7B 7F 41 74 79 49 4A 4E 55 66 41 42 60 59 59 53 65 41 7E 5F 42 58 63 65 7F 57 75 5B 47 7F 49 4C 40 64 7B 7F 7B 7F 53 60 43 40 57 7C 47 7D 4F 59 74 7F 75 73 73 6E 7F 6D 42 70 57 52 43 4A 40 6A 4A 7E 77 4E 49 58 42 48 4C 6D 7F 7D 73 7F 54 66 79 43 60 63 66 6C 52 7C 50 68 41 40 47 77 7F 6F 72 45 73 7F 67 7D 5F 40 54 43 46 40 51 48 43 44 75 48 70 42 40 5E 60 60 6E 72 43 4D 7F 41 4B 50 72 6B 7C 78 7F 41 49 42 48 75 4C 50 40 40 7D 6E 40 6E 72 47 6C 40 40 73 5F 7C 6B 7C 7F 7F 4F 66 41 43 60 53 59 53 43 41 4C 7F 7D 68 6D 6F 74 5F 55 47 72 7D 7B 7F 41 74 79 49 4A 4E 55 66 41 42 60 59 59 6B 6A 41 79 5F 42 58 53 7E 7F 57 75 5B 62 40 76 73 7F 7E 7B 7F 7B 7F 76 5F 7C 7F 68 66 47 7D 4F 59 51 40 4A 4C 4C 74 7F 6D 42 70 72 6D 7C 75 7F 70 4A 7E 77 4E 6C 67 7D 77 73 77 7F 7D 73 7F 71 59 46 7C 5F 79 66 54 59 7C 4C 68 41 40 67 60 7F 6D 72 45 7A 7F 67 75 5F 5C 54 43 46 40 74 77 7C 7B 4A 52 70 42 40 4E 6C 60 6E 72 43 4D 7F 41 47 50 50 54 43 47 40 5B 49 41 48 75 53 50 40 40 7D 4B 7F 51 4D 78 76 40 40 73 5F 59 54 43 40 40 55 66 41 43 60 76 66 54 71 7E 4E 7F 7D 68 6D 6F 74 5F 55 47 72 7D 7B 7F 41 74 79 49 4A 4E 55 66 41 42 60 59 59 7B 61 41 76 5F 42 58 53 7E 7F 57 75 5B 62 40 76 73 7F 7E 7B 7F 7B 7F 76 5F 7C 7F 68 66 47 7D 4F 69 4A 40 4A 4C 4C 51 40 52 7D 4F 68 6D 7C 75 7F 55 75 41 48 71 76 67 7D 77 73 52 40 42 4C 40 6B 59 46 7C 5F 5C 59 7B 69 43 67 57 7E 7F 58 7A 7F 6D 72 45 5F 40 58 46 60 64 6B 7C 79 7F 4B 68 40 44 75 6C 4F 7D 7F 59 55 5F 51 4D 7C 72 40 7E 70 6F 73 6B 7C 78 7F 41 49 41 48 75 76 6F 7F 7F 62 63 40 6E 72 47 6C 40 40 73 5F 7C 6B 7C 7F 7F 4F 66 41 43 60 53 59 7B 45 41 5B 7F 7D 68 6D 4A 4B 60 6A 78 68 7D 7B 7F 41 51 46 76 75 71 4F 66 41 42 60 7C 66 68 53 7E 6F 5F 42 58 53 5B 40 68 4A 64 78 40 76 73 7F 5B 44 40 44 40 6C 5F 7C 7F 68 43 78 42 70 76 62 7F 75 73 73 4B 40 52 73 6F 42 52 43 4A 40 4F 75 41 48 71 53 58 42 48 4C 48 40 42 4C 40 4E 66 79 43 60 46 59 57 62 43 60 57 7E 7F 58 7A 7F 6D 72 45 5F 40 58 4E 60 78 6B 7C 79 7F 6E 57 7F 7B 4A 76 4F 7D 7F 65 45 5F 51 4D 7C 57 7F 41 4F 50 69 6B 7C 78 7F 64 76 7C 77 4A 40 50 40 40 5D 5C 7F 51 4D 78 53 7F 7F 4C 60 43 54 43 40 40 70 59 7E 7C 5F 6C 66 68 75 7E 78 40 42 57 52 50 4B 60 6A 78 4D 42 44 40 7E 4B 46 76 75 71 6A 59 7E 7D 5F 66 66 70 58 7E 74 5F 42 58 73 4C 40 68 4A 64 5D 7F 49 4C 40 41 44 40 44 40 49 50 40 40 57 65 47 7D 4F 71 57 40 4A 4C 4C 74 7F 6D 4C 50 7D 6D 7C 75 7F 70 4A 7E 77 4E 6C 67 7D 77 73 77 7F 7D 73 7F 71 59 46 7C 5F 79 66 70 50 7E 4D 68 41 40 57 7B 7F 6D 72 45 5F 40 58 41 60 6A 6B 7C 79 7F 4B 68 40 44 75 6C 4F 40 78 7D 48 40 51 4D 5C 7F 7F 5B 73 5F 50 54 43 41 40 7A 4B 42 78 4A 5C 60 7F 7F 73 61 7F 6D 72 7F 58 66 41 43 60 76 66 70 7E 7E 63 40 42 57 52 75 74 5F 55 47 57 42 44 40 7E 6E 79 49 4A 4E 70 59 7E 7D 5F 43 79 60 69 41 5C 60 7D 67 4C 56 40 68 4A 64 78 40 76 73 7F 5B 44 40 44 40 6C 6F 7F 7F 68 7F 47 7D 4F 51 40 40 4A 4C 4C 51 40 52 7B 6F 7B 6D 7C 75 7F 70 4A 7E 77 4E 6C 67 7D 77 4B 58 40 42 4C 40 6B 59 46 7C 5F 5C 79 60 65 41 5B 68 41 40 77 6C 7F 6D 72 45 7A 7F 67 7A 5F 4E 54 43 46 40 51 68 42 44 75 65 4F 40 78 5D 7A 7F 6E 72 63 40 40 64 42 60 52 54 43 41 40 7A 4B 41 78 4A 66 5F 40 40 6C 6C 7F 6D 72 7F 58 66 41 43 60 76 46 5F 72 7E 4E 7F 7D 68 6D 6F 74 5F 55 47 72";

    //    foreach (QString s, str.split(' ')) {
    //        bufDir.append(QByteArray::fromHex( s.toLocal8Bit()));
    //    }


    // loadDataForFile();

    //  work();

    file.setFileName("rtcmout_my.txt");
    if(file.open(QIODevice::Append | QIODevice::Text))
    {
        qDebug () << "File open to write data: rtcmout_my.txt";
    }
}

void RTCM::loadDataForFile()
{
    QFile file("rtcmout.bin");
    if(file.open(QIODevice::ReadOnly))
    {
        bufDir = file.readAll();

        file.close();
    }

    QString str =
            "59 7E 7D 5F 66 66 4A 5D 7E 5C 60 7D 67 7C 4D 40 68 4A 64 5D 7F 49 42 40 7C 7B 7F 7B 7F 76 5F 7C 77 68 7A 47 7D 4F 65 41 40 4A 4C 4C 51 40 52 7D 4F 68 6D 7C 75 7F 55 75 41 48 71 76 67 7D 77 73 52 40 41 4C 40 51 66 79 43 60 63 66 4A 55 7C 4D 68 41 40 7B 67 7F 6F 72 45 56 40 58 48 60 59 54 43 46 40 74 77 7D 7B 4A 44 70 42 40 59 51 5F 51 4D 7C 72 40 7E 72 6F 6C 6B 7C 78 7F 64 4E 7C 77 4A 48 50 40 40 63 64 40 6E 72 47 6C 40 40 73 5F 7C 6B 7C 7F 7F 4F 66 41 43 60 53 59 75 46 41 4E 7F 7D 68 6D 6F 74 5F 55 47 72 7D 7B 7F 41 74 79 49 4A 4E 55 66 41 42 60 59 59 43 6E 41 71 5F 42 58 63 65 7F 57 75 5B 47 7F 49 42 40 59 44 40 44 40 6C 5F 7C 7F 68 43 78 42 70 46 79 7F 75 73 73 6E 7F 6D 42 70 57 52 43 4A 40 6A 4A 7E 77 4E 49 58 42 48 4C 6D 7F 7D 73 7F 54 66 79 43 60 63 66 7C 5F 7C 7E 57 7E 7F 78 6D 7F 6F 72 45 56 40 58 4C 60 67 6B 7C 79 7F 4B 48 41 44 75 41 70 42 40 61 7E 60 6E 72 43 68 40 7E 74 6F 68 6B 7C 78 7F 64 76 7F 77 4A 7A 6F 7F 7F 7C 69 40 6E 72 47 6C 40 40 73 5F 7C 6B 7C 7F 7F 4F 66 41 43 60 53 59 43 48 41 43 7F 7D 68 6D 6F 74 5F 55 47 72 7D 7B 7F 41 74 79 49 4A 4E 55 66 41 42 60 59 59 53 65 41 7E 5F 42 58 63 65 7F 57 75 5B 47 7F 49 4C 40 64 7B 7F 7B 7F 53 60 43 40 57 7C 47 7D 4F 59 74 7F 75 73 73 6E 7F 6D 42 70 57 52 43 4A 40 6A 4A 7E 77 4E 49 58 42 48 4C 6D 7F 7D 73 7F 54 66 79 43 60 63 66 6C 52 7C 50 68 41 40 47 77 7F 6F 72 45 73 7F 67 7D 5F 40 54 43 46 40 51 48 43 44 75 48 70 42 40 5E 60 60 6E 72 43 4D 7F 41 4B 50 72 6B 7C 78 7F 41 49 42 48 75 4C 50 40 40 7D 6E 40 6E 72 47 6C 40 40 73 5F 7C 6B 7C 7F 7F 4F 66 41 43 60 53 59 53 43 41 4C 7F 7D 68 6D 6F 74 5F 55 47 72 7D 7B 7F 41 74 79 49 4A 4E 55 66 41 42 60 59 59 6B 6A 41 79 5F 42 58 53 7E 7F 57 75 5B 62 40 76 73 7F 7E 7B 7F 7B 7F 76 5F 7C 7F 68 66 47 7D 4F 59 51 40 4A 4C 4C 74 7F 6D 42 70 72 6D 7C 75 7F 70 4A 7E 77 4E 6C 67 7D 77 73 77 7F 7D 73 7F 71 59 46 7C 5F 79 66 54 59 7C 4C 68 41 40 67 60 7F 6D 72 45 7A 7F 67 75 5F 5C 54 43 46 40 74 77 7C 7B 4A 52 70 42 40 4E 6C 60 6E 72 43 4D 7F 41 47 50 50 54 43 47 40 5B 49 41 48 75 53 50 40 40 7D 4B 7F 51 4D 78 76 40 40 73 5F 59 54 43 40 40 55 66 41 43 60 76 66 54 71 7E 4E 7F 7D 68 6D 6F 74 5F 55 47 72 7D 7B 7F 41 74 79 49 4A 4E 55 66 41 42 60 59 59 7B 61 41 76 5F 42 58 53 7E 7F 57 75 5B 62 40 76 73 7F 7E 7B 7F 7B 7F 76 5F 7C 7F 68 66 47 7D 4F 69 4A 40 4A 4C 4C 51 40 52 7D 4F 68 6D 7C 75 7F 55 75 41 48 71 76 67 7D 77 73 52 40 42 4C 40 6B 59 46 7C 5F 5C 59 7B 69 43 67 57 7E 7F 58 7A 7F 6D 72 45 5F 40 58 46 60 64 6B 7C 79 7F 4B 68 40 44 75 6C 4F 7D 7F 59 55 5F 51 4D 7C 72 40 7E 70 6F 73 6B 7C 78 7F 41 49 41 48 75 76 6F 7F 7F 62 63 40 6E 72 47 6C 40 40 73 5F 7C 6B 7C 7F 7F 4F 66 41 43 60 53 59 7B 45 41 5B 7F 7D 68 6D 4A 4B 60 6A 78 68 7D 7B 7F 41 51 46 76 75 71 4F 66 41 42 60 7C 66 68 53 7E 6F 5F 42 58 53 5B 40 68 4A 64 78 40 76 73 7F 5B 44 40 44 40 6C 5F 7C 7F 68 43 78 42 70 76 62 7F 75 73 73 4B 40 52 73 6F 42 52 43 4A 40 4F 75 41 48 71 53 58 42 48 4C 48 40 42 4C 40 4E 66 79 43 60 46 59 57 62 43 60 57 7E 7F 58 7A 7F 6D 72 45 5F 40 58 4E 60 78 6B 7C 79 7F 6E 57 7F 7B 4A 76 4F 7D 7F 65 45 5F 51 4D 7C 57 7F 41 4F 50 69 6B 7C 78 7F 64 76 7C 77 4A 40 50 40 40 5D 5C 7F 51 4D 78 53 7F 7F 4C 60 43 54 43 40 40 70 59 7E 7C 5F 6C 66 68 75 7E 78 40 42 57 52 50 4B 60 6A 78 4D 42 44 40 7E 4B 46 76 75 71 6A 59 7E 7D 5F 66 66 70 58 7E 74 5F 42 58 73 4C 40 68 4A 64 5D 7F 49 4C 40 41 44 40 44 40 49 50 40 40 57 65 47 7D 4F 71 57 40 4A 4C 4C 74 7F 6D 4C 50 7D 6D 7C 75 7F 70 4A 7E 77 4E 6C 67 7D 77 73 77 7F 7D 73 7F 71 59 46 7C 5F 79 66 70 50 7E 4D 68 41 40 57 7B 7F 6D 72 45 5F 40 58 41 60 6A 6B 7C 79 7F 4B 68 40 44 75 6C 4F 40 78 7D 48 40 51 4D 5C 7F 7F 5B 73 5F 50 54 43 41 40 7A 4B 42 78 4A 5C 60 7F 7F 73 61 7F 6D 72 7F 58 66 41 43 60 76 66 70 7E 7E 63 40 42 57 52 75 74 5F 55 47 57 42 44 40 7E 6E 79 49 4A 4E 70 59 7E 7D 5F 43 79 60 69 41 5C 60 7D 67 4C 56 40 68 4A 64 78 40 76 73 7F 5B 44 40 44 40 6C 6F 7F 7F 68 7F 47 7D 4F 51 40 40 4A 4C 4C 51 40 52 7B 6F 7B 6D 7C 75 7F 70 4A 7E 77 4E 6C 67 7D 77 4B 58 40 42 4C 40 6B 59 46 7C 5F 5C 79 60 65 41 5B 68 41 40 77 6C 7F 6D 72 45 7A 7F 67 7A 5F 4E 54 43 46 40 51 68 42 44 75 65 4F 40 78 5D 7A 7F 6E 72 63 40 40 64 42 60 52 54 43 41 40 7A 4B 41 78 4A 66 5F 40 40 6C 6C 7F 6D 72 7F 58 66 41 43 60 76 46 5F 72 7E 4E 7F 7D 68 6D 6F 74 5F 55 47 72";

    foreach (QString s, str.split(' ')) {
        bufDir.append(QByteArray::fromHex( s.toLocal8Bit()));
    }
}

void RTCM::loadDataCOM(QByteArray data)
{

    bufDir.append(data);
    work();

}

void RTCM::work()
{
    int p1=0,p2=0,p3=0,p4=0,p5=0,p6=0;

    while(bufDir.count() >= 5)
    {
        QString find = "";

        QString data = "";

        p1=0;
        p2=0;
        p3=0;
        p4=0;
        p5=0;
        p6=0;

        loadword(bufDir[0],bufDir[1],bufDir[2],bufDir[3],bufDir[4]);

        for(int i=0; i < 24;i++)
        {
            cout  << bitword[i];

            data.append(QString::number(bitword[i]));

        }


        if((bitword[0] == 0) && (bitword[1] ==1) && (bitword[2] ==1) && (bitword[3] ==0) && (bitword[4] == 0) && (bitword[5] ==1) && (bitword[6] ==1) && (bitword[7]==0))
        {
            find = "0x66";

            p1 = d29star;
            p2 = d30star;
            p3 = d29star;
            p4 = d30star;
            p5 = d30star;
            p6 = d29star;

            for(int j=1;j <= 24;j++)
            {
                if(j == 1 || j==2 || j==3 ||j==5||j==6||j==10||j==11||j==12||j==13||j==14||j==17||j==18||j==20||j==23)
                {
                    p1 ^= bitword[j-1];
                }

                if(j ==2||j==3||j==4||j==6||j==7||j==11||j==12||j==13||j==14||j==15||j==18||j==19||j==21||j==24)
                {
                    p2 ^= bitword[j-1];
                }

                if(j == 1 || j==3 || j==4||j==5||j==7||j==8||j==12||j==13||j==14||j==15||j==16||j==19||j==20||j==22)
                {
                    p3 ^= bitword[j-1];
                }
                if(j == 2 || j==4 || j==5||j==6||j==8||j==9||j==13||j==14||j==15||j==16||j==17||j==20||j==21||j==23)
                {
                    p4 ^= bitword[j-1];
                }
                if(j == 1 || j==3|| j==5||j==6||j==7||j==9||j==10||j==14||j==15||j==16||j==17||j==18||j==21||j==22||j==24)
                {
                    p5 ^= bitword[j-1];
                }
                if(j == 3 || j==5|| j==6||j==8||j==9||j==10||j==11||j==13||j==15||j==19||j==22||j==23||j==24)
                {
                    p6 ^= bitword[j-1];
                }
            }
        }
        else
        {
            p1 = d29star;
            p2 = d30star;
            p3 = d29star;
            p4 = d30star;
            p5 = d30star;
            p6 = d29star;

            for(int j=1;j <= 24;j++)
            {
                if(j == 1 || j==2 || j==3 ||j==5||j==6||j==10||j==11||j==12||j==13||j==14||j==17||j==18||j==20||j==23)
                {
                    p1 ^= bitword[j-1];
                }

                if(j ==2||j==3||j==4||j==6||j==7||j==11||j==12||j==13||j==14||j==15||j==18||j==19||j==21||j==24)
                {
                    p2 ^= bitword[j-1];
                }

                if(j == 1 || j==3 || j==4||j==5||j==7||j==8||j==12||j==13||j==14||j==15||j==16||j==19||j==20||j==22)
                {
                    p3 ^= bitword[j-1];
                }
                if(j == 2 || j==4 || j==5||j==6||j==8||j==9||j==13||j==14||j==15||j==16||j==17||j==20||j==21||j==23)
                {
                    p4 ^= bitword[j-1];
                }
                if(j == 1 || j==3|| j==5||j==6||j==7||j==9||j==10||j==14||j==15||j==16||j==17||j==18||j==21||j==22||j==24)
                {
                    p5 ^= bitword[j-1];
                }
                if(j == 3 || j==5|| j==6||j==8||j==9||j==10||j==11||j==13||j==15||j==19||j==22||j==23||j==24)
                {
                    p6 ^= bitword[j-1];
                }
            }
        }


        data.append("\t");
        cout  << "\t";

        for(int i=0; i < 6;i++)
        {
            cout  << bitparity[i];

            data.append(QString::number(bitparity[i]));
        }

        data.append("\t");
        data.append(QString::number(p1));
        data.append(QString::number(p2));
        data.append(QString::number(p3));
        data.append(QString::number(p4));
        data.append(QString::number(p5));
        data.append(QString::number(p6));
        data.append("\t");


        cout << "\t"<<p1<<p2<<p3<<p4<<p5<<p6<<"\t";

        if(p1 == bitparity[0] && p2 == bitparity[1] && p3 == bitparity[2] && p4== bitparity[3] && p5 == bitparity[4] && p6 == bitparity[5] ) //&& find == "0x66"
        {
            cout << "(True)\t";

            data.append("(True)\t");

            if(find == "0x66")
            {
                QString lol = "";
                for(int k=8;k < 14;k++ )
                {
                    lol += QString::number(bitword[k]);
                }
                cout <<lol.toLocal8Bit().data() <<" = " << lol.toInt(0,2) << "\t";

                flagHeader_1 = false;
                flagHeader_2 = false;

                //Расшифровка RTCM Слово 1
                readheader_1(bitword);

                data.append(lol);
                data.append(" = ");
                data.append(QString::number(lol.toInt(0,2)));
                data.append("\t");

                lol24=0;
            }
            else
            {

                if(flagHeader_1 == true && flagHeader_2 == false)
                {
                    //Расшифровка RTCM Слово 2
                    readheader_2(bitword);
                }
                else
                {
                    if(flagHeader_1 == true && flagHeader_2 == true)
                    {
                        switch (rtsm_word_1.type_sms.toInt(0,2))
                        {
                        //Расшифровка Сообщения 1
                        case 1:{
                            cout << "\tFind 1 Message\t";
                            int lenghtMessage = rtsm_word_2.no_of_data_words.toInt(0,2);
                            lol24++;
                            cout<<"\t" << lol24 << " ["<<lenghtMessage<<"]\t";
                            readmessage1(bitword);
                            break;
                        }

                            //Расшифровка Сообщения 31
                        case 31:{
                            cout << "\tFind 31 Message\t";
                            int lenghtMessage = rtsm_word_2.no_of_data_words.toInt(0,2);
                            lol24++;
                            cout<<"\t" << lol24 << " ["<<lenghtMessage<<"]\t";
                            readmessage31(bitword);
                            break;
                        }

                            //Расшифровка Сообщения 3
                        case 3:{
                            cout << "\tFind 3 Message\t";
                            int lenghtMessage = rtsm_word_2.no_of_data_words.toInt(0,2);
                            lol24++;
                            cout<<"\t" << lol24 << " ["<<lenghtMessage<<"]\t";
                            readmessage3(bitword);
                            break;
                        }
                        }

                    }
                }
            }

        }
        else
        {
            countErrParity++;
        }



        qDebug() <<  find << "\tErr = " << countErrParity;

        data.append(find);
        data.append("\n");

        bufDir.remove(0,5);


        d29star = bitparity[4];
        d30star = bitparity[5];

        findString.append(data);



    } //while(bufDir.isEmpty() == false);


    if(findString.isEmpty() == false)
        saveFile();

}

void RTCM::saveFile()
{


    for(int i=0; i< findString.count();i++)
    {
        QTextStream stream (&file);
        stream << findString[i];

    }

    findString.clear();
}

