// FireworkGun.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <sstream>

#include "shell.h"
#include "printer.h"

using namespace std::literals;

int main()
{   
    int n;
    std::cout << "Enter number of cannon shels: "s;
    std::cin >> n;

    std::srand(std::time(NULL));
    int time = 0;
    Printer printer(200, 75, n);
    while (printer.GetObjectsCount() > 0) {
        Sleep(750);
        std::cout << printer.Draw(time).str();
        ++time;
    }
}

