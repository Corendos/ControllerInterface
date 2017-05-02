#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>

class Controller
{
public:
    Controller();
    int getAxisValue(int axisIndex);
    std::vector<int> getAxis();
    void updateLoop();

private:
    int m_axisCount;
    std::vector<int> m_currentAxisState;
};

#endif // CONTROLLER_H
