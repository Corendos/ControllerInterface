#include <iostream>
#include <fstream>

#include <windows.h>
#include <public.h>
#include <vjoyinterface.h>

#include "qextserialport.h"

using namespace std;

int main()
{
    if(!vJoyEnabled()){
        std::cout << "Error" << std::endl;
        return -1;
    }

    WORD verDLL, verDriver;
    if(!DriverMatch(&verDLL, &verDriver)){
        std::cout << "Error" << std::endl;
    }
    int interfaceIndex = 1;
    VjdStat status = GetVJDStatus(interfaceIndex);

    switch(status){
        case VJD_STAT_OWN:
            std::cout << "Device already owned" << std::endl;
        break;
        case VJD_STAT_FREE:
            std::cout << "Device free" << std::endl;
        break;
        case VJD_STAT_BUSY:
            std::cout << "Device owned by another application" << std::endl;
            return -1;
        break;
        case VJD_STAT_MISS:
            std::cout << "Device is not installed" << std::endl;
            return -1;
        break;
        default:
            std::cout << "Error" << std::endl;
            return -1;
        break;
    }

    if((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && (!AcquireVJD(interfaceIndex)))){
        std::cout << "Error" << std::endl;
        return -1;
    }
    else{
        std::cout << "Success" << std::endl;
    }

    std::ofstream file;
    file.open("test.txt");


    QextSerialPort serial("COM4");
    serial.open(QIODevice::ReadWrite);
    serial.setBaudRate(BAUD115200);
    serial.setDataBits(DATA_8);
    serial.setParity(PAR_NONE);
    serial.setStopBits(STOP_1);
    serial.flush();

    Sleep(2000);

    JOYSTICK_POSITION_V2 data;
    data.bDevice = interfaceIndex;

    char c;

    char joystickValueBuffer[10];
    int joystickValueBufferPointer = 0;
    int count = 0;
    int input[4];

    serial.write("send\n");
    while(1){
        if(serial.bytesAvailable()){
            serial.read(&c, 1);

            if(c == '\r'){
                serial.write("send\n");

                data.wAxisX = input[0];
                data.wAxisY = input[1];
                data.wAxisZ = input[2];
                data.wAxisXRot = input[3];

                UpdateVJD(data.bDevice, &data);

                joystickValueBufferPointer = 0;
                count = 0;
            }
            else if(c == '\n'){
                joystickValueBuffer[joystickValueBufferPointer++] = 0;
                input[count++] = atoi(joystickValueBuffer);
                joystickValueBufferPointer = 0;
            }
            else if(c == '\0'){
                std::cout << "wtf" << std::endl;
            }
            else{
                joystickValueBuffer[joystickValueBufferPointer++] = c;
            }
        }
    }

    RelinquishVJD(interfaceIndex);
    serial.close();
    file.close();

    return 0;
}

