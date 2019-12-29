#ifndef SCANSERIAL_H
#define SCANSERIAL_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <iostream>
#include <vector>
#include <string>
#include <QTime>
#include <pthread.h>
#include "commonutils.h"

using namespace std;
#define SERIAL_DISCONNECT_TIME  100
#define SERIAL_RECEIVE_SIZE     1024
#define SERIAL_STRING_SIZE     SERIAL_RECEIVE_SIZE * 3

class ScanSerial;

typedef struct _self_thread_data{
    bool flag;                  // 线程运行标志
    ScanSerial* instance;
} selfThreadData, *pSelfThreadData;

typedef struct _serial_receive_data{
    int len;
    char data[SERIAL_RECEIVE_SIZE];
} serialReceiveData, *pSerialReceiveData;

class ScanSerial: public QObject
{
    Q_OBJECT
signals:
    // 串口断开
    void sigSerialDisconnect();
    // 读取到数据
    void sigSerialDataReceive(void*);
private:
    static ScanSerial* instance;
    static QSerialPort* serial;
    static string comPort;
    static int timerId;
    static pthread_t receiveThread;
    static selfThreadData threadData;
protected:
    void timerEvent(QTimerEvent *event);
public:
    static ScanSerial* getInstance();
    static void destroyInstance();
    static vector<string> getSerialList();
    static int setSettings(string comPort, int buadRate, int dataBits, string parity, float stopBit);
    static int openSerial();
    static int closeSerial();
    static int writeSerial(char*pData, int len);
    static void receiveSerial();
private:
    ScanSerial();
    virtual~ScanSerial();
    static void closeReceiveThread();
};

#endif // SCANSERIAL_H
