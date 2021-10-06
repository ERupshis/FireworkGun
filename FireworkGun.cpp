// FireworkGun.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <memory>
#include <Windows.h>
#include <cstdlib>
#include <tuple>
#include <list>
#include <optional>
#include <cmath>
#include <set>

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
        : begin_time_(time), 
        life_time_(life_time), 
        velocity_(velocity), 
        direction_angle_(direction_angle), 
        coord_(coord)
    {
    }

    int begin_time_ = 0;
    int life_time_ = 0;
    double velocity_ = 0;
    double velocity_curr_ = 0;
    int direction_angle_ = 0;
    Coord coord_ = { 0,0 };
    Coord coord_curr_ = { 0,0 };
};

enum class Child {
    FRAGMENT,
    SPARK
};

class Shell {
public:
    Shell(int time) // constructor for CannonShell       
    {   
        data_.begin_time_ = time;
        data_.coord_ = { 0, 0 };
        data_.coord_curr_= { 0, 0 };
        data_.direction_angle_ = 30 + (std::rand() % 120);//
        data_.life_time_ = std::rand() % 15 + 8;
        data_.velocity_ = 20.0 + std::rand() % 20;       
    }

    Shell(int time, const ShellData& parent, enum class Child type) // Constructor for FragmentShell or Spark
    {
        data_.begin_time_ = time;
        data_.coord_ = parent.coord_curr_;
        data_.coord_curr_ = parent.coord_curr_;
        data_.life_time_ = parent.life_time_ / 2;
        if (type == Child::FRAGMENT) {
            int rand_angle = (std::rand() % 40) - 20; // 6 degree deviation
            data_.direction_angle_ = parent.direction_angle_ + rand_angle;
            data_.velocity_ = parent.velocity_curr_ * 2;
        }
        else {
            data_.direction_angle_ = (std::rand() % 359);            
            data_.velocity_ = parent.velocity_curr_ + 2 * parent.life_time_;
        }
    }    
    
    std::vector<std::unique_ptr<Shell>> Refresh(int time) {
        if (data_.life_time_ + data_.begin_time_ <= time) {
            return Destroy(time);            
        }
        UpdShell(time);
        return {};
    }   
    
    virtual ~Shell() {}

    ShellData GetData() const {
        return data_;
    }

    void SetData(const ShellData& data) {
        data_ = data;
    }
private:
    ShellData data_;  

    virtual std::vector<std::unique_ptr<Shell>> Destroy(int time) = 0;
    

    void UpdShell(int time) {
        ShellData data = static_cast<Shell*>(this)->GetData();
        static_cast<Shell*>(this)->SetData(UpdShellCurrCoords(data, time));
    }

    ShellData UpdShellCurrCoords(const ShellData& data, int time) {
        ShellData res = data;

        int x = res.velocity_ * std::cos(res.direction_angle_ * std::acos(-1) / 180) * (time - res.begin_time_) / 10;
        int y = res.velocity_ * std::sin(res.direction_angle_ * std::acos(-1) / 180) * (time - res.begin_time_) / 10 - 4.9 * (time - res.begin_time_) / 15 * (time - res.begin_time_) /10;

        res.velocity_curr_ = std::sqrt((res.velocity_ * std::cos(res.direction_angle_ * std::acos(-1) / 180))
            * (res.velocity_ * std::cos(res.direction_angle_ * std::acos(-1) / 180))
            + (res.velocity_ * std::sin(res.direction_angle_ * std::acos(-1) / 180) - 9,8 * (time - res.begin_time_) / 10
            * (res.velocity_ * std::sin(res.direction_angle_ * std::acos(-1) / 180) - 9,8 * (time - res.begin_time_) / 10)));

        res.coord_curr_.x = res.coord_.x + x;
        res.coord_curr_.y = ((res.coord_.y + y < 0) ? 0 : res.coord_.y + y);
        return res;
    }
};

class Spark : public Shell {
public:
    Spark(int time, const ShellData& parent_data)
        :Shell(time, parent_data, Child::SPARK) {
    }
private:
    std::vector<std::unique_ptr<Shell>> Destroy(int time) override {
        ShellData& data = static_cast<Shell*>(this)->GetData();
        data.begin_time_ = -1;
        static_cast<Shell*>(this)->SetData(data);
        return {};
    }
       
};

class ShellFragment : public Shell {
public:
    ShellFragment(int time, const ShellData& parent_data)
        :Shell(time, parent_data, Child::FRAGMENT) {
    }
    
private:
    std::vector<std::unique_ptr<Shell>> Destroy(int time) override {
        std::vector<std::unique_ptr<Shell>> res;
        ShellData& data = static_cast<Shell*>(this)->GetData();

        size_t sparks_num = (std::rand() % 3); // tenth
        for (size_t i = 0; i < ((sparks_num == 0) ? 10 : sparks_num * 10); ++i) {
            res.push_back(std::make_unique<Spark>(Spark(time, data))); // Spark!!!
        }
        data.begin_time_ = -1;
        static_cast<Shell*>(this)->SetData(data);
        return res;
    }    
};

class CannonShell : public Shell {
public:
    CannonShell(int time)
       :Shell(time) {       
    }       

    ~CannonShell() {        
    }
private:
    std::vector<std::unique_ptr<Shell>> Destroy(int time) override {
        std::vector<std::unique_ptr<Shell>> res;
        ShellData data = static_cast<Shell*>(this)->GetData();

        if (std::rand() % 2) {
            res.push_back(std::make_unique<ShellFragment>(ShellFragment(time, data)));
            res.push_back(std::make_unique<ShellFragment>(ShellFragment(time, data)));
        }
        else {
            size_t sparks_num = (std::rand() % 4); // tenth
            for (size_t i = 0; i < ((sparks_num == 0) ? 10 : sparks_num * 10); ++i) {
                res.push_back(std::make_unique<Spark>(Spark(time, data))); // Spark!!!
            }
        }
        data.begin_time_ = -1;
        static_cast<Shell*>(this)->SetData(data);
        return res;
    }

    
    
};

int main()
{   
    std::srand(std::time(NULL));
    int time = 0;
    std::list<std::unique_ptr<Shell>> objects;    
    for (int i = 0; i < 20; ++i) {
        objects.push_back(std::make_unique<CannonShell>(CannonShell(time)));
    }
    while (objects.size() > 0) {
        //int min_x = INT32_MAX, max_x = INT32_MIN, min_y = INT32_MAX, max_y = INT32_MIN;
        int min_x = -100, max_x = 100, min_y = 0, max_y = 75;
        std::set<Coord> objects_coords;        
        Sleep(600);        
        std::vector<std::list<std::unique_ptr<Shell>>::iterator> obj_to_remove;
        for (auto& it = objects.begin(); it != objects.end(); ++it) {
            std::vector<std::unique_ptr<Shell>> tmp = it->get()->Refresh(time);
            if (tmp.size() != 0) { // -1 - mark of spark                
                for (size_t i = 0; i < tmp.size(); ++i) {
                    objects.push_back(std::move(tmp[i]));
                }
                
            } 
            const ShellData& shell_data = it->get()->GetData();
            if (shell_data.begin_time_ == -1) {
                obj_to_remove.push_back(it);
            }
            else {                
                objects_coords.insert(shell_data.coord_curr_);
            }
        }


        for (int i = obj_to_remove.size() - 1; i >= 0; --i) {
            objects.erase(obj_to_remove[i]);
        }
        for (int y = max_y; y >= min_y - 3; --y) {
            for (int x = min_x; x <= max_x; ++x) {
                if (y >= min_y) {
                    if (objects_coords.count({x, y })) {
                        std::cout << 'X';
                    }
                    else {
                        std::cout << '-';
                    }
                }
                else {
                    if (-1 <= x && x <= 1) {
                        std::cout << 'G';
                    }
                    else {
                        std::cout << ' ';
                    }
                }

            }
            std::cout << '\n';
        }
                  
        std::cout << '\n' << '\n' << '\n';
        ++time;

    }
}

