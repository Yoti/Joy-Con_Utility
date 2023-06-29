#include "jcfirm.h"
#include "file.h"
#include <switch.h>
#include <iostream>
#include <string.h>
#include <dirent.h>

class UI{
public:
    UI(void);
    ~UI(void);

    int pad = 0;

    static void mainMenu(void);
    static void changeMenu(void);
    static void restoreMenu(void);
    static void restoreListFiles(void);
    static void backupListFiles(void);
    static void confirmWrite(int padnum, char *sFile);
    static char *launchKeyboard(char *header_str, char *init_str, int max_length);
private:

protected:

};
