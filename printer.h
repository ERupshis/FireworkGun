#pragma once

#include <sstream>
#include <set>

#include "shell.h"

class Printer {    
public:
    Printer(int x, int y, int shells_count);

    std::stringstream Draw(int time);
    int GetObjectsCount() const;
private:
    struct frame {
        int x = 0;
        int y = 0;
    } frame_;

    std::list<std::unique_ptr<Shell>> objects;

    char PrintShellOrEmpty(int x, int y, const std::set<Coord>& objects_coords);
    void RemoveDeadObjects(const std::vector<std::list<std::unique_ptr<Shell>>::iterator>& obj_to_remove);
    std::set<Coord> GetObjectsNewCoords(int time);
};