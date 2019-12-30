#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QFileDialog>
#include <QProgressBar>
#include <QTime>
#include <QDateTime>
#include "scanserial.h"
#include "selfcombobox.h"

#define WND_TIME_OUT            60
#define READ_FILE_BUF_SIZE      512
#define PRINT_BUF_SIZE          512 * 4


class MainWindow : public QMainWindow
{
    Q_OBJECT
signals:
    // 文件发送完成信号
    void sigFinishSendFile();
    // 进度条信号
    void sigUpdateProcess(int);
private:
    QLabel* serialNameLabel;            // UI 串口名标签
    QLabel* serialBuadRateLabel;        // UI 波特率标签
    QLabel* serialDataBitsLabel;        // UI 数据位标签
    QLabel* serialParityLabel;          // UI 奇偶校验标签
    QLabel* serialStopBitsLabel;        // UI 停止位标签
    SelfComboBox* serialNameCB;         // UI 串口名下拉菜单
    SelfComboBox* serialBuadRateCB;     // UI 波特率下拉菜单
    SelfComboBox* serialDataBitsCB;     // UI 数据位下拉菜单
    SelfComboBox* serialParityCB;       // UI 奇偶校验下拉菜单
    SelfComboBox* serialStopBitsCB;     // UI 停止位下拉菜单
    QPushButton* openSerialBtn;         // UI 打开串口按钮

    QLabel* receiveAreaLabel;           // UI 数据接收区标签
    QTextEdit* receiveArea;             // UI 数据接收区显示
    QPushButton* receiveAreaClearBtn;   // UI 清空数据显示按钮

    QLabel* sendAreaLabel;              // UI 数据发送区标签
    QTextEdit* sendArea;                // UI 数据发送区显示
    QPushButton* sendAreaClearBtn;

    QLabel* toBeSentDataLabel;
    QLineEdit* toBeSentData;
    QPushButton* toBeSentDataBtn;

    QLabel* filePathLabel;
    QLineEdit* filePath;
    QPushButton* filePathSelectBtn;
    QPushButton* filePathSendBtn;

    QProgressBar* progress;

    int buatRate;
    string comPort;
    int dataBits;
    string parity;
    float stopBits;
    bool isSerialOpen;
    FILE* file;
    int fileSize;                   // 文件大小
    int sendedSize;                 // 已发送大小
    int timerId;

protected:
    //将matlab窗口设置为随着窗口变化而变化
    virtual void resizeEvent(QResizeEvent *event) override;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    // 初始化 UI
    void initUI();
    // 连接 信号和槽
    void initConnectSignalSlots();
protected:
    // 定时器执行函数
    void timerEvent(QTimerEvent *event);
private slots:
    void onComboBoxComNameQActivated(const QString &arg);
    void onComboBoxBaudRateActivated(const QString &arg);
    void onComboBoxDataBitsActivated(const QString &arg);
    void onComboBoxParityActivated(const QString &arg);
    void onComboBoxStopBitsActivated(const QString &arg);
    void onClickSerialNameComboBox();
    void onClickOpenSerialBtn();
    void onSerialDisconnect();
    void onClearReceiveData();
    void onClearSendData();
    void onSendData();
    void onReceiveSerialData(void* pData);
    void onSelectFile();
    void onSendFile();
};

#endif // MAINWINDOW_H
