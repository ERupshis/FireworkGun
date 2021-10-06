// FireworkGun.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <sstream>

#include "shell.h"

using namespace std::literals;

class Printer {
public:
    Printer(int x, int y, int shells_count) 
        : frame_({ x, y }) {
        for (int i = 0; i < shells_count; ++i) {
            objects.push_back(std::make_unique<CannonShell>(CannonShell(0)));
        }
    }

    std::stringstream Draw(int time) {
        std::stringstream out;
        std::set<Coord> objects_coords;
        std::vector<std::list<std::unique_ptr<Shell>>::iterator> obj_to_remove;

        for (auto& it = objects.begin(); it != objects.end(); ++it) {
            std::vector<std::unique_ptr<Shell>> tmp = it->get()->Refresh(time);
            if (tmp.size() != 0) { // -1 - mark of spark                
                for (size_t i = 0; i < tmp.size(); ++i) {
                    objects.push_back(std::move(tmp[i]));
                }

            }
            ShellData shell_b_data = it->get()->GetData('b');
            ShellData shell_c_data = it->get()->GetData('c');
            if (shell_c_data.begin_time_ == -1) {
                obj_to_remove.push_back(it);
            }
            else {
                objects_coords.insert(shell_c_data.coord_);
            }
        }

        for (int i = obj_to_remove.size() - 1; i >= 0; --i) {
            objects.erase(obj_to_remove[i]);
        }

        for (int y = frame_.y; y >= - 3; --y) {
            for (int x = -frame_.x / 2; x <= frame_.x / 2; ++x) {
                if (y >= 0) {
                    if (objects_coords.count({ x, y })) {
                        out << 'X';
                    }
                    else {
                        out << ' ';
                    }
                }
                else {
                    if (-1 <= x && x <= 1) {
                        out << 'G';
                    }
                    else if (y != -3) {
                        if (objects_coords.count({ x, y })) {
                            out << 'X';
                        }
                        else {
                            out << ' ';
                        }
                    }                    
                    else {
                        out << '_';
                    }
                }

            }
            out << '\n';
        }
        out << "Live obejcts: "s << objects.size();
        out << '\n';
        return out;
    }



    int GetObjectsCount() const {
        return objects.size();
    }

private:
    struct frame {
        int x = 0;
        int y = 0;
    } frame_;
    std::list<std::unique_ptr<Shell>> objects;
};

int main()
{   
    int n;
    std::cout << "Enter number of cannon shels: "s;
    std::cin >> n;

    std::srand(std::time(NULL));
    int time = 0;
    Printer printer(200, 75, n);
    while (printer.GetObjectsCount() > 0) {
        Sleep(500);
        std::cout << printer.Draw(time).str();
        ++time;
    }
}

