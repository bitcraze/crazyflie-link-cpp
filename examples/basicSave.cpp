#include <iostream>
#include <chrono>
#include <fstream>
#include "Crazyflie.h"


using std::cout;
using std::cin;
using std::endl;
using namespace bitcraze::crazyflieLinkCpp;

int main()
{
    Crazyflie crazyflie("usb://0");
    std::string line, csvItem, csvId;
    int i = 0;
    int j = 0;

    // crazyflie.printToc();
    uint16_t numOfElements = crazyflie.getTocInfo()._numberOfElements;
    cout << "------------------------------------\n" << "Number of Parameters: " << (int) numOfElements << "\n\n" << endl;


    while(j != 5)
    {
        cout << "1 - save TOC to file.\n2 - print all TOC from the file\n3 - print specific param ID from the file.\n5 - exit." << endl;
        cin >> j;

        if(j == 1)
        {
            crazyflie.saveToc("tocSave");
        }
        if(j == 2)
        {
            std::ifstream tocFile ("tocSave.csv");
            //print all TOC from file
            if (tocFile.is_open()) {
                while (getline(tocFile,line)) 
                {
                    cout << line << endl;
                }
            }
            tocFile.close(); 
        }
        if(j == 3)
        {
            std::ifstream tocFile ("toc.csv");
            cout << "\nEnter line num: " << endl;
            cin >> i;

            int lineNumber = 0;
            int lineNumberSought = i; 

            if (tocFile.is_open()){
                while (getline(tocFile,line)){
                    lineNumber++;
                    if(lineNumber == lineNumberSought + 1){
                        cout << line << endl;
                        std::istringstream myline(line);
                        while(getline(myline, csvItem, ',')){
                            cout << csvItem << endl;
                        }
                    }
                }
            }
            tocFile.close();
        }
    }

    return 0;
}