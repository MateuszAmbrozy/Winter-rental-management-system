#include "stdafx.h"
#include "DateManagement.h"

int main() 
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | 0x0004 | 0x0002 | FOREGROUND_INTENSITY);
    DateManagement* dm = new DateManagement();
    dm->update();
    delete dm;

    return 0;
}
