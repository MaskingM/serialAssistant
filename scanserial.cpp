#include "scanserial.h"
ScanSerial* ScanSerial::instance = NULL;
QSerialPort* ScanSerial::serial = NULL;
string ScanSerial::comPort;
int ScanSerial::timerId = 0;
pthread_t ScanSerial::receiveThread = NULL;
selfThreadData ScanSerial::threadData = {0};

void* receiveSerialDataThread(void *pData){
    pSelfThreadData pThreadData = (pSelfThreadData)pData;
    while (pThreadData->flag){

        if(pThreadData->instance){
            pThreadData->instance->receiveSerial();
        }
        sleep_ms(5);
    }
    pthread_exit(0);
    return NULL;
}

ScanSerial::ScanSerial(){
    ScanSerial::serial = new QSerialPort(this);
    ScanSerial::timerId = 0;
    ScanSerial::comPort = "";
}

ScanSerial::~ScanSerial(){
    delete ScanSerial::serial;
    ScanSerial::serial = NULL;
    ScanSerial::comPort = "";
    if(ScanSerial::timerId){
        killTimer(ScanSerial::timerId);
    }
    ScanSerial::timerId = 0;
    // 关闭线程
    ScanSerial::closeReceiveThread();
}

ScanSerial* ScanSerial::getInstance(){
    if(!ScanSerial::instance){
        ScanSerial::instance = new ScanSerial;
        ScanSerial::threadData.instance = ScanSerial::instance;
        ScanSerial::threadData.flag = false;
    }
    return ScanSerial::instance;
}

void ScanSerial::destroyInstance(){
    if(ScanSerial::instance){
        delete ScanSerial::instance;
    }
    ScanSerial::instance = NULL;
    ScanSerial::threadData.instance = ScanSerial::instance;
    ScanSerial::threadData.flag = false;
}

vector<string> ScanSerial::getSerialList(){
    vector<string> list;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
        list.push_back(info.portName().toStdString());
    }
    return list;
}

int ScanSerial::openSerial(){
    int res = 0;
    do{
        if(!ScanSerial::serial){
            res = -1;
            break;
        }
        bool ret = ScanSerial::serial->open(QIODevice::ReadWrite);
        if(!ret){
            res = -2;
            break;
        }
        ScanSerial::timerId = ScanSerial::instance->startTimer(SERIAL_DISCONNECT_TIME);
        // 开启线程
        if(!ScanSerial::threadData.flag){
            ScanSerial::threadData.flag = true;
            pthread_create(&(ScanSerial::receiveThread), NULL, receiveSerialDataThread, &(ScanSerial::threadData));
        }
    }while(0);
    return res;
}
int ScanSerial::closeSerial(){
    int res = 0;
    do{
        if(!ScanSerial::serial){
            res = -1;
            break;
        }
        ScanSerial::serial->close();

        if(ScanSerial::timerId){
            ScanSerial::instance->killTimer(ScanSerial::timerId);
        }

        ScanSerial::timerId = 0;
        // 关闭线程
        ScanSerial::closeReceiveThread();
    }while(0);
    return res;
}

int ScanSerial::setSettings(string comPort, int buadRate, int dataBits, string parity, float stopBit){
    int res = 0;
    do{
        if(!ScanSerial::instance){
            res = -1;
            break;
        }
        if(comPort == ""){
            res = 1;
            break;
        }
        if(buadRate == 0){
            res = 2;
            break;
        }
        ScanSerial::comPort = comPort;
        ScanSerial::serial->setPortName(comPort.c_str());
        switch (buadRate) {
        case 1200:
            ScanSerial::serial->setBaudRate(QSerialPort::Baud1200);
            break;
        case 2400:
            ScanSerial::serial->setBaudRate(QSerialPort::Baud2400);
            break;
        case 4800:
            ScanSerial::serial->setBaudRate(QSerialPort::Baud4800);
            break;
        case 9600:
            ScanSerial::serial->setBaudRate(QSerialPort::Baud9600);
            break;
        case 19200:
            ScanSerial::serial->setBaudRate(QSerialPort::Baud19200);
            break;
        case 38400:
            ScanSerial::serial->setBaudRate(QSerialPort::Baud38400);
            break;
        case 57600:
            ScanSerial::serial->setBaudRate(QSerialPort::Baud57600);
            break;
        case 115200:
            ScanSerial::serial->setBaudRate(QSerialPort::Baud115200);
            break;
        default:
            break;
        }

        switch (dataBits) {
        case 5:
            ScanSerial::serial->setDataBits(QSerialPort::Data5);
            break;
        case 6:
            ScanSerial::serial->setDataBits(QSerialPort::Data6);
            break;
        case 7:
            ScanSerial::serial->setDataBits(QSerialPort::Data7);
            break;
        case 8:
            ScanSerial::serial->setDataBits(QSerialPort::Data8);
            break;
        default:
            break;
        }

        if(parity == "None"){
            ScanSerial::serial->setParity(QSerialPort::NoParity);
        }else if(parity == "Odd"){
            ScanSerial::serial->setParity(QSerialPort::OddParity);
        }else if(parity == "Even"){
            ScanSerial::serial->setParity(QSerialPort::EvenParity);
        }else if(parity == "Mark"){
            ScanSerial::serial->setParity(QSerialPort::MarkParity);
        }else if(parity == "Space"){
            ScanSerial::serial->setParity(QSerialPort::SpaceParity);
        }

        if(stopBit == 1){
            ScanSerial::serial->setStopBits(QSerialPort::OneStop);
        }else if(stopBit == 1.5){
            ScanSerial::serial->setStopBits(QSerialPort::OneAndHalfStop);
        }else if(stopBit == 2){
            ScanSerial::serial->setStopBits(QSerialPort::TwoStop);
        }
        ScanSerial::serial->setFlowControl(QSerialPort::NoFlowControl);	//设置流控制
    }while(0);
    return res;
}

void ScanSerial::timerEvent(QTimerEvent *event){
    if(ScanSerial::comPort != ""){
        vector<string> list = ScanSerial::getSerialList();
        bool isNotice = true;
        for(int i = 0; i < list.size(); i++){
            if(ScanSerial::comPort == list[i]){
                isNotice = false;
                break;
            }
        }
        if(isNotice){
            if(ScanSerial::timerId){
                ScanSerial::instance->killTimer(ScanSerial::timerId);
            }
            ScanSerial::timerId = 0;
            // 关闭线程
            ScanSerial::closeReceiveThread();
            emit sigSerialDisconnect();
        }
    }
}

int ScanSerial::writeSerial(char* pData, int  len){
    int res = 0;
    do{
        if(!ScanSerial::serial){
            res = -1;
            break;
        }
        if(len <= 0){
            break;
        }
        res = ScanSerial::serial->write(pData, len);
        ScanSerial::serial->flush();
    }while(0);
    return res;
}

void ScanSerial::receiveSerial(){
    do{
        if(!ScanSerial::instance){
            break;
        }
        QByteArray qBytes = ScanSerial::serial->readAll();
        if(qBytes.size()){
            char buf[SERIAL_RECEIVE_SIZE] = {0};
            int idx = 0;
            for(int i = 0; i < qBytes.size(); i++){
                buf[idx] = qBytes.data()[i];
                idx++;
            }

            #ifdef WIN32
                long long start = GetTickCount();
            #else
                struct timeval start;
                gettimeofday(&start, NULL);
            #endif
            while(1){
                qBytes = ScanSerial::serial->readAll();
                if(qBytes.size()){
                    #ifdef WIN32
                        start = GetTickCount();
                    #else
                        gettimeofday(&start, NULL);
                    #endif
                }
                for(int i = 0; i < qBytes.size(); i++){
                    buf[idx] = qBytes.data()[i];
                    idx++;
                }
                #ifdef WIN32
                    long long end = GetTickCount();
                    if(end - start >= 50){
                        break;
                    }
                #else
                    struct timeval end;
                    gettimeofday(&start, NULL);
                    if(1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec >= 50000){
                        break;
                    }
                #endif
            }
            serialReceiveData data = {0};
            data.len = idx;
            memcpy(data.data, buf, idx);
            emit ScanSerial::instance->sigSerialDataReceive((void*)&data);
        }
    }while(0);
}

void ScanSerial::closeReceiveThread(){
    if(ScanSerial::receiveThread){
        pthread_kill(ScanSerial::receiveThread, 0);
        ScanSerial::receiveThread = NULL;
        ScanSerial::threadData.flag = false;
    }
}

