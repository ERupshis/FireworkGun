#include "shell.h"

namespace shell_sup { // evaluation methods
    double GetRadianAngle(int degree) {
        return degree * std::acos(-1) / 180.0;
    }

    int GetDegreeAngle(double radian) {
        return 180.0 * radian / std::acos(-1);
    }

    int CountCoordX(int time, int divider, const ShellData& b_data) {
        return b_data.velocity_ * std::cos(GetRadianAngle(b_data.direction_angle_)) * (time - b_data.begin_time_) / divider;
    }

    int CountCoordY(int time, int divider, const ShellData& b_data) {
        return (b_data.velocity_ * std::sin(GetRadianAngle(b_data.direction_angle_)) * (time - b_data.begin_time_)
            - 4.9 * (time - b_data.begin_time_) / divider * (time - b_data.begin_time_)) / divider;
    }

    int CountCurrAngle(int time, int divider, const ShellData& b_data) { // atan(Vy/Vx) = atan((Vo*sina - gt) / (Vo * cosa))
        double g = 9.8;
        int elaps_time = (time - b_data.begin_time_) / divider;
        double res;
        if (b_data.direction_angle_ <= 90 || b_data.direction_angle_ > 270) {
            res = std::atan((b_data.velocity_ * std::sin(GetRadianAngle(b_data.direction_angle_)) - g * elaps_time)
                / (b_data.velocity_ * std::cos(GetRadianAngle(b_data.direction_angle_))));               
            return (360 + GetDegreeAngle(res)) % 360;            
        }
        else {
            int angle = b_data.direction_angle_ - 180;
            res = std::atan((b_data.velocity_ * std::sin(GetRadianAngle(b_data.direction_angle_)) - g * elaps_time)
                / (b_data.velocity_ * std::cos(GetRadianAngle(b_data.direction_angle_))));            
            return 180 + GetDegreeAngle(res);
        }        
    }

    double CountCurrSpeed(int time, int divider, const ShellData& b_data) { // sqrt((Vo*cosa)^2 + (Vo*sina - g*t))
        double g = 9.8;
        int elaps_time = (time - b_data.begin_time_) / divider;        
        return std::sqrt(std::pow(b_data.velocity_ * std::cos(shell_sup::GetRadianAngle(b_data.direction_angle_)), 2.0)
            + std::pow(b_data.velocity_ * std::sin(shell_sup::GetRadianAngle(b_data.direction_angle_)) - g * elaps_time, 2.0));
    }
}
/*-------------------------------------------------------------*/
Shell::Shell(int time) // constructor for CannonShell       
{
    b_data_.begin_time_ = time;
    b_data_.coord_ = { 0, 0 };  
    b_data_.direction_angle_ = 30 + (std::rand() % 120);
    b_data_.life_time_ = 8 + (std::rand() % 15);
    b_data_.velocity_ = 20.0 + (std::rand() % 20);

    c_data_.life_time_ = b_data_.life_time_;
}

Shell::Shell(int time, const ShellData& c_parent, enum class Child type) // Constructor for FragmentShell or Spark
{
    b_data_.begin_time_ = time;
    b_data_.coord_ = c_parent.coord_;
    b_data_.life_time_ = c_parent.life_time_ / 2;
    c_data_.life_time_ = b_data_.life_time_;
    if (type == Child::FRAGMENT) {
        int rand_angle = (std::rand() % 40) - 20; // +-20 degree deviation
        b_data_.direction_angle_ = c_parent.direction_angle_ + rand_angle;
        b_data_.velocity_ = c_parent.velocity_ * 2;
    }
    else {
        b_data_.direction_angle_ = (std::rand() % 359); // random angle of sprak direction
        b_data_.velocity_ = c_parent.velocity_ + c_parent.life_time_;
    }
}

std::vector<std::unique_ptr<Shell>> Shell::Refresh(int time) { // if method returns nothing - object dead time didn't come or it's a final stage of object life,                                                             )
    if (b_data_.life_time_ + b_data_.begin_time_ <= time) {
        return Destroy(time); // pure virtual. Each type of object has its own definition
                                // if result is {} - endlife of object without new child objects (sparks of fragments),
                                //otherwise it was split on smaller object (fragmens of spraks)
    }
    UpdShellCurrData(time); // refresh current coords and speed of shell
    return {}; // object is in the mid of its life
}
ShellData Shell::GetData(char type) const {
    if (type == 'b') {
        return b_data_;
    }
    else {
        return c_data_;
    }
}

void Shell::SetData(const ShellData& data) { // apliccable only for current data
    c_data_ = data;
}      

void Shell::UpdShellCurrData(int time) { // responsible for current speed and position calculation        
    int divider = 10; // reduce distance of object move on picture

    int d_x = shell_sup::CountCoordX(time, divider, b_data_);
    int d_y = shell_sup::CountCoordY(time, divider, b_data_);
   
    c_data_.coord_.y = ((b_data_.coord_.y + d_y < -2) ? -2 : b_data_.coord_.y + d_y); // objects cannot fall bellow floor
    c_data_.coord_.x = ((c_data_.coord_.y == -2) ? c_data_.coord_.x : b_data_.coord_.x + d_x);
    c_data_.direction_angle_ = shell_sup::CountCurrAngle(time, divider, b_data_);    
    c_data_.velocity_ = shell_sup::CountCurrSpeed(time, divider, b_data_);
}
/*-------------------------------------------------------------*/

Spark::Spark(int time, const ShellData& parent_data)
    :Shell(time, parent_data, Child::SPARK) {
}

std::vector<std::unique_ptr<Shell>> Spark::Destroy(int time) {
    ShellData c_data = GetData('c');
    c_data.begin_time_ = -1; // marker for dead object
    SetData(c_data);
    return {}; // no new object at after death
}
/*-------------------------------------------------------------*/
ShellFragment::ShellFragment(int time, const ShellData& parent_data)
    :Shell(time, parent_data, Child::FRAGMENT) {
}

std::vector<std::unique_ptr<Shell>> ShellFragment::Destroy(int time) {
    std::vector<std::unique_ptr<Shell>> res;
    ShellData c_data = GetData('c'); // use current parent data for new objects

    size_t sparks_num = (std::rand() % 3); // tenth
    for (size_t i = 0; i < (10 + sparks_num * 10); ++i) {
        res.push_back(std::make_unique<Spark>(Spark(time, c_data))); // new sparks only
    }

    c_data.begin_time_ = -1; // marker of dead object
    SetData(c_data);
    return res; // new objects
}
/*-------------------------------------------------------------*/
CannonShell::CannonShell(int time)
    :Shell(time) {
}

std::vector<std::unique_ptr<Shell>> CannonShell::Destroy(int time) {
    std::vector<std::unique_ptr<Shell>> res;
    ShellData c_data = GetData('c');

    if (std::rand() % 2) { // cannon shell splits on 2 fragments
        res.push_back(std::make_unique<ShellFragment>(ShellFragment(time, c_data)));
        res.push_back(std::make_unique<ShellFragment>(ShellFragment(time, c_data)));
    }
    else { // cannon shell splits on 10x sparks
        size_t sparks_num = (std::rand() % 3); // tenth
        for (size_t i = 0; i < (10 + sparks_num * 10); ++i) {
            res.push_back(std::make_unique<Spark>(Spark(time, c_data))); // Spark!!!
        }
    }

    c_data.begin_time_ = -1; // marker of dead object
    SetData(c_data);
    return res;
}
