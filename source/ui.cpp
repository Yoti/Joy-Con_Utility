#include "ui.h"
#include <cstdlib>
#include <switch.h>
#include <malloc.h>

UI::UI(){
    //constructor
}

UI::~UI(){
    //destructor
}

typedef struct{
    char *menuList;
    char *definition;
    void (*entry)();
    void *arg;
} menuEntry;

menuEntry ulist[] = {
    {strdup("Backup Firmware"), strdup("Backup your current Firmware to SD Card"), UI::backupListFiles},
    {strdup("Restore Firmware"), strdup("Restore Firmware from SD Card to Joy-Con"), UI::restoreListFiles},
    {strdup("Change SN"), strdup("Change your current SN to a userdefined"), UI::changeMenu},
    {strdup("Restore SN"), strdup("Restore SN to normal"), UI::restoreMenu},
    {strdup("Exit"), NULL, NULL},
};
int listEntries = 5;

menuEntry clist[] = {
    {strdup("Change SN"), NULL, NULL},
    {strdup("Back"), NULL, NULL},
};
int clistEntries = 2;

menuEntry rlist[] = {
    {strdup("Restore SN"), NULL, NULL},
    {strdup("Back"), NULL, NULL},
};
int rlistEntries = 2;

void UI::mainMenu(void){
    PadState keys;
    padInitializeDefault(&keys);
    int debounce = 0;
    int curPos = 0;
    JCFirm joy;
    char origLeft[15];
    char origRight[15];
    bool isChanged = true;

    joy.getSNFromPad(origLeft, origRight);
    while(appletMainLoop()){
        consoleClear();

        if(isChanged){
            joy.getSNFromPad(origLeft, origRight);
            isChanged = false;
        }
/*
        if(hidGetHandheldMode() == false){
            printf("Not in Handheld mode. Plug in some Joy-Cons.\n");
            consoleUpdate(NULL);
            svcSleepThread(4*1000000000ull); //Wait 4 Seconds and exit
            break;
        }
*/
        printf("Welcome to the Joy-Con Utility\nThis application doesn't have any fancy gfx\n\n");
        printf("Left Joy-Con: %s\n", origLeft);
        printf("Right Joy-Con: %s\n\n", origRight);
        printf("Main menu: \n\n");

        for(int i=0;i<listEntries;i++){
            if(curPos != i){
                printf("    %s\n", ulist[i].menuList);
            }
            else{
                printf("\x1B[35m--> %s\x1B[0m\n", ulist[i].menuList);
            }
        }

        for(int i=0;i<30-listEntries;i++){
            printf("\n");
        }
        printf("\x1B[35m");
        for(int i=0;i<80;i++){
            printf("*");
        }
        printf("\x1B[0m\n\n");

        if(ulist[curPos].definition != NULL){
            printf("%s\n", ulist[curPos].definition);
        }

        padUpdate(&keys);
        u64 kDown = padGetButtonsDown(&keys);
        u64 kUp = padGetButtonsUp(&keys);

        if(debounce){
            if(kUp & HidNpadButton_Up){
                debounce=0;
            }
            if(kUp & HidNpadButton_Down){
                debounce=0;
            }
        }

        if(kDown & HidNpadButton_Up){
            if(debounce==0){
                debounce=1;
                if(curPos == 0){
                    curPos=listEntries-1;
                }
                else{
                    curPos--;
                }
            }
        }

        if(kDown & HidNpadButton_Down){
            if(debounce==0){
                debounce = 1;
                if(curPos == listEntries-1){
                    curPos=0;
                }else{
                    curPos++;
                }
            }
        }

        if(kUp & HidNpadButton_A){
            if(strcmp(ulist[curPos].menuList, "Exit") == 0){
                break;
            }
            ulist[curPos].entry();
            isChanged = true;
        }

        if(kUp & HidNpadButton_B){
            break;
        }

        if(kDown & HidNpadButton_Plus){
            break;
        }

        consoleUpdate(NULL);
    }
}

void UI::changeMenu(void){
    PadState keys;
    padInitializeDefault(&keys);
    int debounce = 0;
    int curPos = 0;
    int pad = 0;
    while(appletMainLoop()){
        consoleClear();
        printf("Change SN - here you can change your Serial Number: \n\n");
        printf("Press [A] to select - [B] to exit\n");
        printf("[L] to select LEFT Joy-Con - [R] to select RIGHT Joy-Con\n");
        printf("Selected Joy-Con: \x1B[32m%s Joy-Con\x1B[0m\n\n", pad ? "RIGHT" : "LEFT");

        for(int i=0;i<clistEntries;i++){
            if(curPos != i){
                printf("    %s\n", clist[i].menuList);
            }
            else{
                printf("\x1B[35m--> %s\x1B[0m\n", clist[i].menuList);
            }
        }

        padUpdate(&keys);
        u64 kDown = padGetButtonsDown(&keys);
        u64 kUp = padGetButtonsUp(&keys);

        if(debounce){
            if(kUp & HidNpadButton_Up){
                debounce=0;
            }
            if(kUp & HidNpadButton_Down){
                debounce=0;
            }
        }

        if(kDown & HidNpadButton_Up){
            if(debounce==0){
                debounce=1;
                if(curPos == 0){
                    curPos=clistEntries-1;
                }
                else{
                    curPos--;
                }
            }
        }

        if(kDown & HidNpadButton_Down){
            if(debounce==0){
                debounce = 1;
                if(curPos == clistEntries-1){
                    curPos=0;
                }else{
                    curPos++;
                }
            }
        }

        if(kDown & HidNpadButton_L){
            pad=0;
        }

        if(kDown & HidNpadButton_R){
            pad=1;
        }

        if(kUp & HidNpadButton_A){
            if(strcmp(clist[curPos].menuList, "Change SN") == 0){
                JCFirm joy;
                if(joy.changeSN(pad)){
                    printf("\n\nChanged SN to %s\n", joy.getSNFromPad(pad));
                    consoleUpdate(NULL);
                    svcSleepThread(3*1000000000ull);
                    break;
                }else{
                    printf("\n\nError\n");
                    consoleUpdate(NULL);
                    svcSleepThread(3*1000000000ull);
                    break;
                }
            }
            if(strcmp(clist[curPos].menuList, "Back") == 0){
                break;
            }
            clist[curPos].entry();
        }

        if(kUp & HidNpadButton_B){
            break;
        }

        if(kDown & HidNpadButton_Plus){
            break;
        }

        consoleUpdate(NULL);
    }
}

void UI::restoreMenu(void){
    PadState keys;
    padInitializeDefault(&keys);
    int debounce = 0;
    int curPos = 0;
    int pad = 0;
    while(appletMainLoop()){
        consoleClear();
        printf("Restore SN - here you can restore your Serial Number: \n\n");
        printf("Press [A] to select - [B] to exit\n");
        printf("[L] to select LEFT Joy-Con - [R] to select RIGHT Joy-Con\n");
        printf("Selected Joy-Con: \x1B[32m%s Joy-Con\x1B[0m\n\n", pad ? "RIGHT" : "LEFT");
        for(int i=0;i<rlistEntries;i++){
            if(curPos != i){
                printf("    %s\n", rlist[i].menuList);
            }
            else{
                printf("\x1B[35m--> %s\x1B[0m\n", rlist[i].menuList);
            }
        }

        padUpdate(&keys);
        u64 kDown = padGetButtonsDown(&keys);
        u64 kUp = padGetButtonsUp(&keys);

        if(debounce){
            if(kUp & HidNpadButton_Up){
                debounce=0;
            }
            if(kUp & HidNpadButton_Down){
                debounce=0;
            }
        }

        if(kDown & HidNpadButton_Up){
            if(debounce==0){
                debounce=1;
                if(curPos == 0){
                    curPos=rlistEntries-1;
                }
                else{
                    curPos--;
                }
            }
        }

        if(kDown & HidNpadButton_Down){
            if(debounce==0){
                debounce = 1;
                if(curPos == rlistEntries-1){
                    curPos=0;
                }else{
                    curPos++;
                }
            }
        }

        if(kDown & HidNpadButton_L){
            pad=0;
        }

        if(kDown & HidNpadButton_R){
            pad=1;
        }

        if(kUp & HidNpadButton_A){
            if(strcmp(rlist[curPos].menuList, "Restore SN") == 0){
                JCFirm joy;
                if(joy.restoreSN(pad)){
                    printf("\n\nRestored SN to %s\n", joy.getSNFromPad(pad));
                    consoleUpdate(NULL);
                    svcSleepThread(3*1000000000ull);
                    break;
                }else{
                    printf("\n\nError\n");
                    consoleUpdate(NULL);
                    svcSleepThread(3*1000000000ull);
                    break;
                }
            }
            if(strcmp(rlist[curPos].menuList, "Back") == 0){
                break;
            }
            clist[curPos].entry();
        }

        if(kUp & HidNpadButton_B){
            break;
        }

        if(kDown & HidNpadButton_Plus){
            break;
        }

        consoleUpdate(NULL);
    }
}

void UI::restoreListFiles(void){
    PadState keys;
    padInitializeDefault(&keys);
    File *dfile = new File(strdup("."));
    char **list = dfile->getFileList();
    int listLength = dfile->getFileListLength();
    int curPos = 0;
    int debounce = 0;
    int pad = 0;
    while(1){
        consoleClear();
        printf("Select a file to restore.\n\nPress [A] to select - [B] to exit \n[L] to select LEFT Joy-Con - [R] to select RIGHT Joy-Con\n");
        printf("Selected Joy-Con: \x1B[32m%s Joy-Con\x1B[0m\n\n", pad ? "RIGHT" : "LEFT");
        for(int i=0;i<listLength;i++){
            if(curPos != i){
                printf("    %-50s\tSize: %li Bytes\n", list[i], dfile->getFileSize(list[i]));
            }
            else{
                printf("\x1B[35m--> %-50s\tSize: %li Bytes\x1B[0m\n", list[i], dfile->getFileSize(list[i]));
            }
        }
        consoleUpdate(NULL);

        padUpdate(&keys);

        u64 kDown = padGetButtonsDown(&keys);
        u64 kUp = padGetButtonsUp(&keys);

        if(debounce){
            if(kUp & HidNpadButton_Up){
                debounce=0;
            }
            if(kUp & HidNpadButton_Down){
                debounce=0;
            }
        }

        if(kDown & HidNpadButton_Up){
            if(debounce==0){
                debounce=1;
                if(curPos == 0){
                    curPos=listLength-1;
                }
                else{
                    curPos--;
                }
            }
        }

        if(kDown & HidNpadButton_Down){
            if(debounce==0){
                debounce = 1;
                if(curPos == listLength-1){
                    curPos=0;
                }else{
                    curPos++;
                }
            }
        }

        if(kDown & HidNpadButton_L){
            pad=0;
        }

        if(kDown & HidNpadButton_R){
            pad=1;
        }

        if(kUp & HidNpadButton_A){
            if(strcmp(list[curPos], "EMPTY") != 0){
                UI::confirmWrite(pad, list[curPos]);
                svcSleepThread(3*1000000000ull);
            }
        }

        if(kUp & HidNpadButton_B){
            break;
        }
    }
    delete dfile;
}

void UI::backupListFiles(void){
    PadState keys;
    padInitializeDefault(&keys);
    File *dfile = new File(strdup("."));
    char **list = dfile->getBackupFileList();
    int listLength = dfile->getFileListLength();
    int curPos = 0;
    int debounce = 0;
    int pad = 0;
    while(1){
        consoleClear();
        printf("Select a file to backup.\n\nPress [A] to select - [B] to exit \n[L] to select LEFT Joy-Con - [R] to select RIGHT Joy-Con\n");
        printf("Selected Joy-Con: \x1B[32m%s Joy-Con\x1B[0m\n\n", pad ? "RIGHT" : "LEFT");
        for(int i=0;i<listLength;i++){
            if(curPos != i){
                if(strcmp(list[i], "[NEW FILE]") == 0){
                    printf("    %-50s\n", list[i]);
                }
                else{
                    printf("    %-50s\tSize: %li Bytes\n", list[i], dfile->getFileSize(list[i]));
                }
            }
            else{
                if(strcmp(list[i], "[NEW FILE]") == 0){
                    printf("\x1B[35m--> %-50s\x1B[0m\n", list[i]);
                }
                else{
                    printf("\x1B[35m--> %-50s\tSize: %li Bytes\x1B[0m\n", list[i], dfile->getFileSize(list[i]));
                }
            }
        }
        consoleUpdate(NULL);

        padUpdate(&keys);

        u64 kDown = padGetButtonsDown(&keys);
        u64 kUp = padGetButtonsUp(&keys);

        if(debounce){
            if(kUp & HidNpadButton_Up){
                debounce=0;
            }
            if(kUp & HidNpadButton_Down){
                debounce=0;
            }
        }

        if(kDown & HidNpadButton_Up){
            if(debounce==0){
                debounce=1;
                if(curPos == 0){
                    curPos=listLength-1;
                }
                else{
                    curPos--;
                }
            }
        }

        if(kDown & HidNpadButton_Down){
            if(debounce==0){
                debounce = 1;
                if(curPos == listLength-1){
                    curPos=0;
                }else{
                    curPos++;
                }
            }
        }

        if(kDown & HidNpadButton_L){
            pad=0;
        }

        if(kDown & HidNpadButton_R){
            pad=1;
        }

        if(kUp & HidNpadButton_A){
            JCFirm *reader = new JCFirm();
            if(strcmp(list[curPos], "[NEW FILE]") == 0){
                char *file = launchKeyboard(strdup("Enter new filename with bin extension:"), strdup(""), 32);
                if(file != NULL){
                    reader->dumpFirmwareFile(pad, file);
                }
                free(file);
            }else{
                reader->dumpFirmwareFile(pad, list[curPos]);
            }
            delete reader;
            svcSleepThread(3*1000000000ull);
        }

        if(kUp & HidNpadButton_B){
            break;
        }
    }
    delete dfile;
}

void UI::confirmWrite(int padnum, char *sFile){
    PadState keys;
    padInitializeDefault(&keys);
    consoleClear();
    printf("Are you sure to flash the backup to your Joy-Con?\n");
    printf("Press [Y] to continue, [B] to exit\n\n");
    consoleUpdate(NULL);
    while(1){
        padUpdate(&keys);

        u64 kDown = padGetButtonsDown(&keys);
        u64 kUp = padGetButtonsUp(&keys);

        if(kDown & HidNpadButton_Y){
            printf("\x1B[31mIT MAY PERMANENTLY DAMAGE YOUR JOY-CON!\n");
            printf("DO NOT UNPLUG ANY JOY-CON IN FLASHING STATE!\n");
            printf("USING A WRONG FIRMWARE FOR THE WRONG JOY-CON WILL BRICK YOUR JOY-CON!\n");
            printf("YOU CANNOT INTERRUPT THIS TASK!\n");
            printf("I WILL NOT TAKE ANY RISK FOR YOUR FAILURE!\nARE YOU SURE?\n");
            printf("Press [ZR] to continue, [B] to exit\n\n");
            consoleUpdate(NULL);
            while(1){
                padUpdate(&keys);

                u64 nDown = padGetButtonsDown(&keys);
                u64 kUp = padGetButtonsUp(&keys);

                if(nDown & HidNpadButton_ZR){
                    appletBeginBlockingHomeButtonShortAndLongPressed(0);
                    JCFirm *writer = new JCFirm();
                    int ret = writer->writeFirmwareFile(padnum, sFile);
                    delete writer;
                    appletEndBlockingHomeButtonShortAndLongPressed();
                    if((ret == -621) || (ret == -622)){
                        printf("Wrong firmware for this Joy-Con!!\n");
                        consoleUpdate(NULL);
                        break;
                    }
                    if(ret == -5){
                        printf("Serial number mismatch!\n");
                        consoleUpdate(NULL);
                        break;
                    }
                    if(ret == -3){
                        printf("Write error!\n");
                        consoleUpdate(NULL);
                        break;
                    }
                    if(ret == -2){
                        printf("File open or buffer error!\n");
                        consoleUpdate(NULL);
                    }
                    if(ret == -1){
                        printf("Wrong file size\n");
                        consoleUpdate(NULL);
                    }
                    break;
                }
                else if(kUp & HidNpadButton_B)
                {
                    printf("\x1B[0mCANCELED!\n");
                    consoleUpdate(NULL);
                    break;
                }
            }
            break;
        }
        else if(kUp & HidNpadButton_B){
            printf("CANCELED!\n");
            consoleUpdate(NULL);
            break;
        }
    }
}

char *UI::launchKeyboard(char *header_str, char *init_str, int max_length){
    SwkbdConfig kbd;
    Result r = swkbdCreate(&kbd, 0);
    if(R_SUCCEEDED(r)){
        swkbdConfigMakePresetDefault(&kbd);
        swkbdConfigSetHeaderText(&kbd, header_str);
        swkbdConfigSetInitialText(&kbd, init_str);
        swkbdConfigSetStringLenMax(&kbd, max_length);

        char *ret = (char *)malloc(max_length+1);

        r = swkbdShow(&kbd, (char *)ret, max_length+1);
        if(R_SUCCEEDED(r)){
            return ret;
        }
        free(ret);
    }

    return NULL;
}
