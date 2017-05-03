#include "mymainwindow.h"

MyMainWindow::MyMainWindow(QWidget *parent) :
    QMainWindow(parent), m_receivedEntirely(true)
{
    m_joystickPosition.bDevice = 1;

    if(!initVJD()){
        m_serial = nullptr;
        std::cout << "vJoy device error" << std::endl;
        exit(1);
    }

    QSerialPortInfo wantedInfo;
    bool portFound = false;

    QList<QSerialPortInfo> serialPorts = QSerialPortInfo::availablePorts();
    for(QSerialPortInfo port : serialPorts){
        std::cout << "Port Name: " << port.portName().toStdString() << std::endl
                  << "Port description: " << port.description().toStdString() << std::endl << std::endl;
        if(port.description() == "Arduino Uno"){
            std::cout << "Port Found" << std::endl;
            wantedInfo = port;
            portFound = true;
        }
    }

    if(portFound){
        m_serial = new QSerialPort(wantedInfo);
    }else{
        m_serial = nullptr;
        std::cout << "Port not found" << std::endl;
        exit(1);
    }

    m_serial->setBaudRate(QSerialPort::Baud115200);
    if(!m_serial->open(QSerialPort::ReadWrite)){
        std::cout << "Port not opened" << std::endl;
        exit(1);
    }

    m_serial->setDataTerminalReady(true);
    m_serial->setRequestToSend(true);
    connect(m_serial, SIGNAL(readyRead()), this, SLOT(retrieveData()));

    m_serial->write("test");
}

MyMainWindow::~MyMainWindow(){
    if(m_serial->isOpen()){
        std::cout << "Port closed" << std::endl;
        m_serial->close();
    }
    delete m_serial;
}

void MyMainWindow::retrieveData(){
    buffer.append(m_serial->read(100));
    if(buffer.at(buffer.length() - 1) == '\r'){
        updateVJD();
        m_receivedEntirely = true;
        buffer.clear();
    }

    if(m_receivedEntirely){
        m_serial->write("send\n");
        m_receivedEntirely = false;
    }
}

bool MyMainWindow::initVJD(){
    if(!vJoyEnabled()){
            std::cout << "vJoy: vJoy not enabled" << std::endl;
            return false;
        }

        WORD verDLL, verDriver;
        if(!DriverMatch(&verDLL, &verDriver)){
            std::cout << "vJoy: Driver version and DLL version don't match" << std::endl;
            return false;
        }
        VjdStat status = GetVJDStatus(m_joystickPosition.bDevice);

        switch(status){
            case VJD_STAT_OWN:
                std::cout << "vJoy: Device already owned" << std::endl;
            break;
            case VJD_STAT_FREE:
                std::cout << "vJoy: Device free" << std::endl;
            break;
            case VJD_STAT_BUSY:
                std::cout << "vJoy: Device owned by another application" << std::endl;
                return false;
            break;
            case VJD_STAT_MISS:
                std::cout << "vJoy: Device is not installed" << std::endl;
                return false;
            break;
            default:
                std::cout << "vJoy: Error" << std::endl;
                return false;
            break;
        }

        if((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && (!AcquireVJD(m_joystickPosition.bDevice)))){
            std::cout << "vJoy: Device already owned or can't acquire vJoy device" << std::endl;
            return false;
        }
        std::cout << "vJoy: Success" << std::endl;
        return true;
}

void MyMainWindow::updateVJD(){
    QByteArray temp;
    int input[4];
    int inputPointer = 0;
    for(int i = 0;i < buffer.length() - 1;++i){
        if(buffer.at(i) == '\n'){
            input[inputPointer] = atoi(temp.data());
            inputPointer++;
            temp.clear();
        } else {
            temp.append(buffer.at(i));
        }
    }

    m_joystickPosition.wAxisX = input[0];
    m_joystickPosition.wAxisY = input[1];
    m_joystickPosition.wAxisZ = input[2];
    m_joystickPosition.wAxisXRot = input[3];

    UpdateVJD(m_joystickPosition.bDevice, &m_joystickPosition);
}
