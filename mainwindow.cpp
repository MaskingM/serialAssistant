#include "mainwindow.h"
/*
 * 获取日期时间字符串
 * @param   buf char*   字符串接收区
 * @return  void
*/
void getDateTimeStr(char* buf /* out */){
    do{
        if(!buf){
            break;
        }
        QDateTime currTime = QDateTime::currentDateTime();
        char tmpBuf[32] = {0};
        sprintf(buf, "[%d-", currTime.date().year());
        if(currTime.date().month() <= 9){
            sprintf(tmpBuf, "0%d-", currTime.date().month());
        }else {
            sprintf(tmpBuf, "%d-", currTime.date().month());
        }
        strcat(buf, tmpBuf);
        if(currTime.date().day() <= 9){
            sprintf(tmpBuf, "0%d ", currTime.date().day());
        }else {
            sprintf(tmpBuf, "%d ", currTime.date().day());
        }
        strcat(buf, tmpBuf);
        if(currTime.time().hour() <= 9){
            sprintf(tmpBuf, "0%d:", currTime.time().hour());
        }else {
            sprintf(tmpBuf, "%d:", currTime.time().hour());
        }
        strcat(buf, tmpBuf);
        if(currTime.time().minute() <= 9){
            sprintf(tmpBuf, "0%d:", currTime.time().minute());
        }else {
            sprintf(tmpBuf, "%d:", currTime.time().minute());
        }
        strcat(buf, tmpBuf);
        if(currTime.time().second() <= 9){
            sprintf(tmpBuf, "0%d.", currTime.time().second());
        }else {
            sprintf(tmpBuf, "%d.", currTime.time().second());
        }
        strcat(buf, tmpBuf);
        if(currTime.time().msec() < 10){
            sprintf(tmpBuf, "00%d] ", currTime.time().msec());
        }else if(currTime.time().msec() < 100){
            sprintf(tmpBuf, "0%d] ", currTime.time().msec());
        }else {
            sprintf(tmpBuf, "%d] ", currTime.time().msec());
        }
        strcat(buf, tmpBuf);
    }while(0);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->setWindowTitle("串口调试工具");
    this->setMinimumWidth(1024);
    this->setMinimumHeight(640);

    this->initUI();
    this->initConnectSignalSlots();
    this->buatRate = 0;
    this->comPort = "";
    this->dataBits = 8;
    this->parity = "None";
    this->stopBits = 1;
    this->isSerialOpen = false;
    this->file = NULL;
    this->timerId = 0;
}

MainWindow::~MainWindow()
{
    if(this->timerId){
        killTimer(this->timerId);
    }
    this->timerId = 0;
    if(this->file){
        fclose(this->file);
    }

    delete serialNameLabel;
    delete serialBuadRateLabel;
    delete serialDataBitsLabel;
    delete serialParityLabel;
    delete serialStopBitsLabel;
    delete serialNameCB;
    delete serialBuadRateCB;
    delete serialDataBitsCB;
    delete serialParityCB;
    delete serialStopBitsCB;
    delete openSerialBtn;
    delete receiveAreaLabel;
    delete receiveArea;
    delete receiveAreaClearBtn;
    delete sendAreaLabel;
    delete sendArea;
    delete sendAreaClearBtn;
    delete toBeSentDataLabel;
    delete toBeSentData;
    delete toBeSentDataBtn;
    delete filePathLabel;
    delete filePath;
    delete filePathSelectBtn;
    delete filePathSendBtn;
    delete progress;

}

void MainWindow::initUI(){
    // 设置标签
    this->serialNameLabel = new QLabel(this);
    this->serialNameLabel->setText(tr("选择串口："));
    this->serialNameLabel->move(16,20);
    this->serialNameLabel->show();

    this->serialBuadRateLabel = new QLabel(this);
    this->serialBuadRateLabel->setText(tr("设置波特率："));
    this->serialBuadRateLabel->move(180,20);
    this->serialBuadRateLabel->show();

    this->serialDataBitsLabel = new QLabel(this);
    this->serialDataBitsLabel->setText(tr("设置数据位："));
    this->serialDataBitsLabel->move(360,20);
    this->serialDataBitsLabel->show();

    this->serialParityLabel = new QLabel(this);
    this->serialParityLabel->setText(tr("设置奇偶校验："));
    this->serialParityLabel->move(540,20);
    this->serialParityLabel->show();

    this->serialStopBitsLabel = new QLabel(this);
    this->serialStopBitsLabel->setText(tr("设置停止位："));
    this->serialStopBitsLabel->move(730,20);
    this->serialStopBitsLabel->show();

    this->serialNameCB = new SelfComboBox(this);
    this->serialNameCB->move(80,20);
    this->serialNameCB->show();
    this->serialNameCB->setMaximumWidth(80);
    vector<string> list = ScanSerial::getSerialList();
    this->serialNameCB->addItem("");
    for(int i = 0; i < list.size(); i++){
        this->serialNameCB->addItem(list[i].c_str());
    }

    this->serialBuadRateCB = new SelfComboBox(this);
    this->serialBuadRateCB->move(260,20);
    this->serialBuadRateCB->show();
    this->serialBuadRateCB->setMaximumWidth(80);
    this->serialBuadRateCB->addItem("");
    this->serialBuadRateCB->addItem("1200");
    this->serialBuadRateCB->addItem("2400");
    this->serialBuadRateCB->addItem("4800");
    this->serialBuadRateCB->addItem("9600");
    this->serialBuadRateCB->addItem("19200");
    this->serialBuadRateCB->addItem("38400");
    this->serialBuadRateCB->addItem("57600");
    this->serialBuadRateCB->addItem("115200");

    this->serialDataBitsCB = new SelfComboBox(this);
    this->serialDataBitsCB->move(440,20);
    this->serialDataBitsCB->show();
    this->serialDataBitsCB->setMaximumWidth(80);
    this->serialDataBitsCB->addItem("5");
    this->serialDataBitsCB->addItem("6");
    this->serialDataBitsCB->addItem("7");
    this->serialDataBitsCB->addItem("8");
    this->serialDataBitsCB->setCurrentIndex(3);

    this->serialParityCB = new SelfComboBox(this);
    this->serialParityCB->move(630,20);
    this->serialParityCB->show();
    this->serialParityCB->setMaximumWidth(80);
    this->serialParityCB->addItem("None");
    this->serialParityCB->addItem("Odd");
    this->serialParityCB->addItem("Even");
    this->serialParityCB->addItem("Mark");
    this->serialParityCB->addItem("Space");

    this->serialStopBitsCB = new SelfComboBox(this);
    this->serialStopBitsCB->move(810,20);
    this->serialStopBitsCB->show();
    this->serialStopBitsCB->setMaximumWidth(80);
    this->serialStopBitsCB->addItem("1");
    this->serialStopBitsCB->addItem("1.5");
    this->serialStopBitsCB->addItem("2");

    this->openSerialBtn = new QPushButton(this);
    this->openSerialBtn->setText("打开串口");
    this->openSerialBtn->setMaximumWidth(80);
    this->openSerialBtn->move(920,20);
    this->openSerialBtn->show();

    // 数据接收显示区
    this->receiveAreaLabel = new QLabel(this);
    this->receiveAreaLabel->setText(tr("数据接收区："));
    this->receiveAreaLabel->move(16,60);
    this->receiveAreaLabel->show();
    this->receiveArea = new QTextEdit(this);
    this->receiveArea->show();
    this->receiveArea->move(16,90);
    this->receiveArea->setReadOnly(true);
    this->receiveAreaClearBtn = new QPushButton(this);
    this->receiveAreaClearBtn->setText("清空接收区");
    this->receiveAreaClearBtn->setMaximumWidth(80);
    this->receiveAreaClearBtn->show();

    // 发送显示区
    this->sendAreaLabel = new QLabel(this);
    this->sendAreaLabel->setText(tr("数据发送区："));
    this->sendAreaLabel->show();
    this->sendArea = new QTextEdit(this);
    this->sendArea->show();
    this->sendArea->move(16,90);
    this->sendArea->setReadOnly(true);
    this->sendAreaClearBtn = new QPushButton(this);
    this->sendAreaClearBtn->setText("清空发送区");
    this->sendAreaClearBtn->setMaximumWidth(80);
    this->sendAreaClearBtn->show();

    // 待发送数据
    this->toBeSentDataLabel = new QLabel(this);
    this->toBeSentDataLabel->setText(tr("数据待发区："));
    this->toBeSentDataLabel->show();
    this->toBeSentData = new QLineEdit(this);
    this->toBeSentDataLabel->show();
    this->toBeSentDataBtn = new QPushButton(this);
    this->toBeSentDataBtn->setText("发送");
    this->toBeSentDataBtn->setMaximumWidth(80);
    this->toBeSentDataBtn->show();
    this->toBeSentData->setMaxLength(128);
    QRegExp hexReg("([0-9a-fA-F]{2}[ ]{1}){1,80}");
    this->toBeSentData->setValidator(new QRegExpValidator(hexReg,this));

    // 文件发送
    this->filePathLabel = new QLabel(this);
    this->filePathLabel->setText(tr("文件路径："));
    this->filePathLabel->show();
    this->filePath = new QLineEdit(this);
    this->filePath->show();
    this->filePath->setReadOnly(true);
//    this->filePath->setText("C:/Users/ASUS/Desktop/HDTTV0403_0202_CHECK.bin");
    this->filePathSelectBtn = new QPushButton(this);
    this->filePathSelectBtn->setText("选择文件");
    this->filePathSelectBtn->setMaximumWidth(80);
    this->filePathSelectBtn->show();
    this->filePathSendBtn = new QPushButton(this);
    this->filePathSendBtn->setText("发送");
    this->filePathSendBtn->setMaximumWidth(80);
    this->filePathSendBtn->show();

    this->progress = new QProgressBar(this);
    this->progress->show();
    this->progress->setRange(0,100);
}

void MainWindow::initConnectSignalSlots(){
    connect(this->serialNameCB, SIGNAL(clicked()), this, SLOT(onClickSerialNameComboBox()));
    connect(this->openSerialBtn, SIGNAL(clicked()), this, SLOT(onClickOpenSerialBtn()));
    connect(this->serialNameCB, SIGNAL(activated(const QString &)), this, SLOT(onComboBoxComNameQActivated(const QString &)));
    connect(this->serialBuadRateCB, SIGNAL(activated(const QString &)), this, SLOT(onComboBoxBaudRateActivated(const QString &)));
    connect(this->serialDataBitsCB, SIGNAL(activated(const QString &)), this, SLOT(onComboBoxDataBitsActivated(const QString &)));
    connect(this->serialParityCB, SIGNAL(activated(const QString &)), this, SLOT(onComboBoxParityActivated(const QString &)));
    connect(this->serialStopBitsCB, SIGNAL(activated(const QString &)), this, SLOT(onComboBoxStopBitsActivated(const QString &)));
    connect(ScanSerial::getInstance(), SIGNAL(sigSerialDisconnect()), this, SLOT(onSerialDisconnect()));
    connect(this->receiveAreaClearBtn, SIGNAL(clicked()), this, SLOT(onClearReceiveData()));
    connect(this->sendAreaClearBtn, SIGNAL(clicked()), this, SLOT(onClearSendData()));
    connect(this->toBeSentDataBtn, SIGNAL(clicked()), this, SLOT(onSendData()));
    connect(ScanSerial::getInstance(), SIGNAL(sigSerialDataReceive(void*)), this, SLOT(onReceiveSerialData(void*)));
    connect(this->filePathSelectBtn, SIGNAL(clicked()), this, SLOT(onSelectFile()));
    connect(this->filePathSendBtn, SIGNAL(clicked()), this, SLOT(onSendFile()));
}

void MainWindow::onComboBoxComNameQActivated(const QString &arg)
{
    this->comPort = arg.toStdString();
}

void MainWindow::onClickSerialNameComboBox(){
    // 判断列表是否需要重置
    vector<string> list = ScanSerial::getSerialList();
    bool isReset = false;
    if(this->serialNameCB->count() - 1 > list.size()){
        isReset = true;
    }else {
        for(int i = 1; i < this->serialNameCB->count(); i++){

            if(this->serialNameCB->itemText(i).toStdString() != list[i-1]){
                isReset = true;
                break;
            }
        }
    }

    if(isReset || this->serialNameCB->count() == 1){
        for(int i = 0; i < this->serialNameCB->count(); i++){
            this->serialNameCB->removeItem(i);
            i--;
        }
        this->serialNameCB->addItem("");
        bool isResetItem = false;
        int curIdx = 0;
        for(int i = 0; i < list.size(); i++){
            this->serialNameCB->addItem(list[i].c_str());
            if(this->comPort == list[i]){
                isResetItem = true;
                curIdx = i;
            }
        }
        if(isResetItem){
            this->serialNameCB->setCurrentIndex(curIdx);
        }else {
            this->comPort = "";
        }
    }
}

void MainWindow::onComboBoxBaudRateActivated(const QString &arg)
{
    this->buatRate = arg.toInt();
}

void MainWindow::onClickOpenSerialBtn(){
    do{
        if(!this->isSerialOpen){
            if(this->comPort == ""){
                QMessageBox::information(this, "提示", "未选择串口");
                break;
            }
            if(this->buatRate == 0){
                QMessageBox::information(this, "提示", "未设置波特率");
                break;
            }
            ScanSerial::getInstance();
            int res = ScanSerial::setSettings(this->comPort, this->buatRate, this->dataBits, this->parity, this->stopBits);
            if(!res){
                res = ScanSerial::openSerial();
                if(!res){
                    this->serialNameCB->setDisabled(true);
                    this->serialBuadRateCB->setDisabled(true);
                    this->serialDataBitsCB->setDisabled(true);
                    this->serialParityCB->setDisabled(true);
                    this->serialStopBitsCB->setDisabled(true);
                    this->openSerialBtn->setText("关闭串口");
                    this->isSerialOpen = true;
                }else {
                    vector<string> list = ScanSerial::getSerialList();
                    bool hasCurCom = false;
                    for(int i = 0; i < list.size(); i++){
                        if(list[i] == this->comPort){
                            hasCurCom = true;
                            break;
                        }
                    }
                    if(!hasCurCom){
                        QMessageBox::information(this, "提示", "串口未连接");
                        break;
                    }
                }
            }
        }else {
            ScanSerial::closeSerial();
            this->serialNameCB->setDisabled(false);
            this->serialBuadRateCB->setDisabled(false);
            this->serialDataBitsCB->setDisabled(false);
            this->serialParityCB->setDisabled(false);
            this->serialStopBitsCB->setDisabled(false);
            this->openSerialBtn->setText("打开串口");
            this->isSerialOpen = false;
            this->filePathSendBtn->setDisabled(false);
        }

    }while(0);
}


void MainWindow::onComboBoxDataBitsActivated(const QString &arg){
    this->dataBits = arg.toInt();
}

void MainWindow::onComboBoxParityActivated(const QString &arg){
    this->parity = arg.toStdString();
}

void MainWindow::onComboBoxStopBitsActivated(const QString &arg){
    this->stopBits = arg.toFloat();
}

void MainWindow::onSerialDisconnect(){
    QMessageBox::information(this, "提示", "串口连接断开");
    ScanSerial::closeSerial();
    this->serialNameCB->setDisabled(false);
    this->serialBuadRateCB->setDisabled(false);
    this->serialDataBitsCB->setDisabled(false);
    this->serialParityCB->setDisabled(false);
    this->serialStopBitsCB->setDisabled(false);
    this->openSerialBtn->setText("打开串口");
    this->isSerialOpen = false;
    // 断开连接后，
    if(this->timerId){
        killTimer(this->timerId);
    }
    this->timerId = 0;
    if(this->file){
        fclose(this->file);
    }
    // 设置按钮状态
    this->filePathSelectBtn->setDisabled(false);
    this->filePathSendBtn->setDisabled(false);
}

void MainWindow::resizeEvent(QResizeEvent *event){
    int _w = this->width() - 126;
    int _h = (this->height() - 200) / 2;
    this->receiveArea->setMinimumWidth(_w);
    this->receiveArea->setMaximumWidth(_w);
    this->receiveAreaClearBtn->move(this->width() - 96,90);
    this->receiveArea->setMinimumHeight(_h);
    this->receiveArea->setMaximumHeight(_h);

    this->sendAreaLabel->move(16,_h + 90);
    this->sendArea->setMinimumWidth(_w);
    this->sendArea->setMaximumWidth(_w);
    this->sendArea->move(16,_h + 120);
    this->sendAreaClearBtn->move(this->width() - 96,_h + 120);
    this->sendArea->setMinimumHeight(_h);
    this->sendArea->setMaximumHeight(_h);

    this->toBeSentDataLabel->move(16, this->height() - 70);
    this->toBeSentData->move(86, this->height() - 70);
    this->toBeSentData->setMinimumWidth(this->width() / 2 - 186);
    this->toBeSentData->setMaximumWidth(this->width() / 2 - 186);
    this->toBeSentDataBtn->move(this->width() / 2 - 86, this->height() - 70);

    this->filePathLabel->move(this->width() / 2 + 20, this->height() - 70);
    this->filePath->move(this->width() / 2 + 90, this->height() - 70);
    this->filePath->setMinimumWidth(this->width() / 2 - 286);
    this->filePath->setMaximumWidth(this->width() / 2 - 286);
    this->filePathSelectBtn->move(this->width() - 186, this->height() - 70);
    this->filePathSendBtn->move(this->width() - 96, this->height() - 70);

    this->progress->move(0, this->height() - 20);
    this->progress->setMinimumWidth(this->width());
    this->progress->setMaximumWidth(this->width());
}

void MainWindow::onClearReceiveData(){
    this->receiveArea->clear();
}

void MainWindow::onClearSendData(){
    this->sendArea->clear();
}

void MainWindow::onSendData(){
    do{
        if(!isSerialOpen){
            QMessageBox::information(this, "提示", "串口未打开");
            break;
        }
        if(this->toBeSentData->text().trimmed() == ""){
            cout << "未输入字符" << endl;
        }
        char buf[256] = {0};
        char* p = (char*)this->toBeSentData->text().toStdString().c_str();
        int idx = 0;
        do{
            if(!p){
                break;
            }
            if(p[0] == ' '){
                p++;
            }
            if((p[0] >= '0' && '9' >= p[0]) || (p[0] >= 'a' && 'f' >= p[0]) || (p[0] >= 'A' && 'F' >= p[0])){
                int num = 0;
                sscanf(p, "%02X", &num);
                buf[idx] = num;
                idx++;
            }

        }while(p = strstr(p, " "));
        int count = ScanSerial::writeSerial(buf, idx);
        if(count > 0){
            char printBuf[PRINT_BUF_SIZE] = {0};
            getDateTimeStr(printBuf);
            strcat(printBuf, this->toBeSentData->text().toStdString().c_str());
            this->sendArea->append(printBuf);
        }
    }while(0);
}

void MainWindow::onReceiveSerialData(void* pData){
    pSerialReceiveData pSerialData = (pSerialReceiveData)pData;
    char buf[SERIAL_STRING_SIZE] = {0};
    if(pSerialData->len >= SERIAL_STRING_SIZE){
        return;
    }
    for(int i = 0; i < pSerialData->len; i++){
        sprintf(((char*)buf) + (i * 3), "%02X ", pSerialData->data[i] & 0xFF);
    }
    char printBuf[PRINT_BUF_SIZE] = {0};
    getDateTimeStr(printBuf);
    strcat(printBuf, buf);
    this->receiveArea->append(printBuf);
}

void MainWindow::onSelectFile(){
    do{
        QString path = QFileDialog::getOpenFileName(this, "选择文件", NULL, "*.bin;*.BIN;*.txt;*.TXT");
        if(path == ""){
            break;
        }
        this->filePath->setText(path);
    }while(0);
}

void MainWindow::onSendFile(){
    do{
        // 判断串口是否打开
        if(!isSerialOpen){
            QMessageBox::information(this, "提示", "串口未打开");
            break;
        }
        // 判断是否选择文件
        if(this->filePath->text() == ""){
            QMessageBox::information(this, "提示", "未选择文件");
            break;
        }
        // 判断当前是否正在发送文件，通过定时器是否正在执行进行判断
        if(this->timerId || this->file){
            QMessageBox::information(this, "提示", "已有文件正在发送，请稍后");
            break;
        }
        // 进行打开文件，并检测文件是否过大
        this->file = fopen(this->filePath->text().toStdString().c_str(), "rb");
        if(!this->file){
            QMessageBox::information(this, "提示", "文件打开失败，请重试");
            break;
        }
        fseek(this->file, 0L, SEEK_END);
        this->fileSize = ftell(this->file);
        if(this->fileSize <= 0 || this->fileSize >= 1024 * 1024 * 512){
            QMessageBox::information(this, "提示", "文件内容为空或者文件超过 512 MB");
            fclose(this->file);
            this->file = NULL;
            break;
        }
        fseek(this->file, 0L, SEEK_SET);
        this->sendedSize = 0;
        // 开启定时器
        this->timerId = this->startTimer(WND_TIME_OUT);
        // 禁用选择文件按钮，和发送文件按钮
        this->filePathSelectBtn->setDisabled(true);
        this->filePathSendBtn->setDisabled(true);
    }while(0);
}

/*
 * 发送文件，放在定时器中执行，
 * 本身考虑，文件发送属于大量数据，发送放在主线程中会造成 UI 卡死，
 * 所以原本打算每次发送文件时，开启子线程进行，但是串口发送放在子线程中时
 * 会出现对象无法创建之类的错误，会综合考虑，每次定时器执行时，只发送一帧数据
 * 这样既不会影响界面，也可以连续发送数据
*/
void MainWindow::timerEvent(QTimerEvent *event){
    //
    if(this->isSerialOpen){
        if(this->file){
            char buf[READ_FILE_BUF_SIZE] = {0};
            char printBuf[PRINT_BUF_SIZE] = {0};
            int count = fread(buf, 1, READ_FILE_BUF_SIZE, this->file);
            if(count > 0){
                int res = ScanSerial::writeSerial(buf, count);
                if(res > 0){
                    // 更新已发送
                    this->sendedSize += count;
                    // 更新进度条
                    this->progress->setValue(((float(this->sendedSize) / this->fileSize * 100)));
                    // 更新发送区
                    getDateTimeStr(printBuf);
                    int idx = strlen(printBuf);
                    char tmpBuf[4] = {0};
                    for(int i = 0; i < count; i++){
                        sprintf(tmpBuf, "%02X ", buf[i] & 0xFF);
                        strcat(printBuf, tmpBuf);
                    }
                    this->sendArea->append(printBuf);
                }
            }
            if(this->sendedSize >= this->fileSize){
                // 发送完成，清除定时器
                if(this->timerId){
                    killTimer(this->timerId);
                }
                this->timerId = 0;
                this->filePathSelectBtn->setDisabled(false);
                this->filePathSendBtn->setDisabled(false);
                fclose(this->file);
                this->file = NULL;
            }
        }
    }
}
