#include "ui.h"
#include <cstdlib>
#include <switch.h>
#include <malloc.h>

int main(int argc, char **argv){
    consoleInit(NULL);
    hiddbgInitialize();
    hidsysInitialize();

    AppletType at = appletGetAppletType();

    if(at != AppletType_Application){
        printf("You're running that in Applet mode. Please run it as Application\nExiting in 4 Seconds...\n");
        consoleUpdate(NULL);
        svcSleepThread(4*1000000000ull); //Wait 4 Seconds and exit
        consoleExit(NULL);
        return 0;
    }

    UI::mainMenu();

    hiddbgExit();
    hidsysExit();
    consoleExit(NULL);
    return 0;
}
