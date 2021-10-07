#include "printer.h"


Printer::Printer(int x, int y, int shells_count)
    :frame_({ x, y })
{
    for (int i = 0; i < shells_count; ++i) {
        objects.push_back(std::make_unique<CannonShell>(CannonShell(0)));
    }
}

char Printer::PrintShellOrEmpty(int x, int y, const std::set<Coord>& objects_coords) {
    if (objects_coords.count({ x, y })) {
        return 'X';
    }
    else {
        return ' ';
    }
} 

std::stringstream Printer::Draw(int time) {
    std::stringstream out;
    std::set<Coord> objects_coords = GetObjectsNewCoords(time);
    for (int y = frame_.y; y >= -3; --y) {
        for (int x = -frame_.x / 2; x <= frame_.x / 2; ++x) {
            if (y >= 0) {
                out << PrintShellOrEmpty(x, y, objects_coords);
            }
            else {
                if (-1 <= x && x <= 1) {
                    out << 'G';
                }
                else if (y != -3) {
                    out << PrintShellOrEmpty(x, y, objects_coords);
                }
                else {
                    out << '_';
                }
            }
        }
        out << '\n';
    }
    using namespace std::literals;
    out << "Live obejcts: "s << objects.size();
    out << '\n';
    return out;
}

int Printer::GetObjectsCount() const {
    return objects.size();
}

void Printer::RemoveDeadObjects(const std::vector<std::list<std::unique_ptr<Shell>>::iterator>& obj_to_remove) {
    for (int i = obj_to_remove.size() - 1; i >= 0; --i) {
        objects.erase(obj_to_remove[i]);
    }
}

std::set<Coord> Printer::GetObjectsNewCoords(int time) {
    std::set<Coord> objects_coords;
    std::vector<std::list<std::unique_ptr<Shell>>::iterator> obj_to_remove;
    for (auto& it = objects.begin(); it != objects.end(); ++it) {
        std::vector<std::unique_ptr<Shell>> new_objects = it->get()->Refresh(time); // new objects after explosion 
        if (new_objects.size() != 0) { // no new onjects                
            for (size_t i = 0; i < new_objects.size(); ++i) {
                objects.push_back(std::move(new_objects[i]));
            }
        }
        ShellData shell_c_data = it->get()->GetData('c');
        if (shell_c_data.begin_time_ == -1) { // mark of dead objects
            obj_to_remove.push_back(it);
        }
        else {
            objects_coords.insert(shell_c_data.coord_); // add new coords of objects
        }
    }
    RemoveDeadObjects(obj_to_remove);
    return objects_coords;
}
