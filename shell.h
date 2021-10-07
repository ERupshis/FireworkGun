#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <Windows.h>
#include <cstdlib>
#include <tuple>
#include <list>
#include <optional>
#include <cmath>

struct Coord {
    int x = 0;
    int y = 0;

    bool operator < (Coord other) const {
        if (x < other.x) {
            return true;
        }
        else if (x == other.x) {
            return y < other.y;
        }
        return false;
    }
};

struct ShellData {
    ShellData() = default;

    ShellData(int time, int life_time, int velocity, int direction_angle, Coord coord)
        : begin_time_(time), life_time_(life_time), velocity_(velocity),
        direction_angle_(direction_angle), coord_(coord)
    {
    }

    int begin_time_ = 0;
    int life_time_ = 0;
    double velocity_ = 0;
    int direction_angle_ = 0;
    Coord coord_ = { 0,0 };
};

enum class Child {
    FRAGMENT,
    SPARK
};

namespace shell_sup { // evaluation methods
    double GetRadianAngle(int degree);
    int GetDegreeAngle(double radian);
    int CountCoordX(int time, int divider, const ShellData& b_data);
    int CountCoordY(int time, int divider, const ShellData& b_data);
    double CountCurrSpeed(int time, int divider, const ShellData& b_data); // sqrt((Vo*cosa)^2 + (Vo*sina - g*t))    
    int CountCurrAngle(int time, int divider, const ShellData& b_data);
}
/*-------------------------------------------------------------*/
class Shell {    
protected:
    Shell(int time); // constructor for CannonShell  
    Shell(int time, const ShellData& c_parent, enum class Child type); // Constructor for FragmentShell or Spark    
public:
    virtual ~Shell() {}

    std::vector<std::unique_ptr<Shell>> Refresh(int time); // if method returns nothing - object dead time didn't come or it's a final stage of object life,                                                             )
    ShellData GetData(char type) const;
    void SetData(const ShellData& data); // apliccable only for current data    

private:
    ShellData b_data_ = {}; // data in the beginning
    ShellData c_data_ = {}; // data at the moment of time

    virtual std::vector<std::unique_ptr<Shell>> Destroy(int time) = 0; // responsible for life of object and creation new ones      

    void UpdShellCurrData(int time); // responsible for current speed and position calculation    
};
/*-------------------------------------------------------------*/
class Spark : public Shell {
public:
    Spark(int time, const ShellData& parent_data);
private:
    std::vector<std::unique_ptr<Shell>> Destroy(int time) override;
};
/*-------------------------------------------------------------*/
class ShellFragment : public Shell {
public:
    ShellFragment(int time, const ShellData& parent_data);
private:
    std::vector<std::unique_ptr<Shell>> Destroy(int time) override;
};
/*-------------------------------------------------------------*/
class CannonShell : public Shell {
public:
    CannonShell(int time);
private:
    std::vector<std::unique_ptr<Shell>> Destroy(int time) override;    
};