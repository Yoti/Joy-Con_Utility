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
    {strdup("Restore Firmware"), strdup("Restore Firmware from SD Card to Joycon"), UI::restoreListFiles},
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
        
        if(hidGetHandheldMode() == false){
            printf("Not in Handheld mode. Plug in some Joycons.\n");
            consoleUpdate(NULL);
            svcSleepThread(4*1000000000ull); //Wait 4 Seconds and exit
            break;
        }
        
        printf("Welcome to the Joycon Utility\nThis application doesn't have any fancy gfx\n\n");
        printf("Left Joycon: %s\n", origLeft);
        printf("Right Joycon: %s\n\n", origRight);
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
        
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        u64 kUp = hidKeysUp(CONTROLLER_P1_AUTO);
        
        if(debounce){
            if(kUp & KEY_UP){
                debounce=0;
            }
            if(kUp & KEY_DOWN){
                debounce=0;
            }
        }
        
        if(kDown & KEY_UP){
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
        
        if(kDown & KEY_DOWN){
            if(debounce==0){
                debounce = 1;
                if(curPos == listEntries-1){
                    curPos=0;
                }else{
                    curPos++;
                }
            }
        }
        
        if(kDown & KEY_A){
            if(strcmp(ulist[curPos].menuList, "Exit") == 0){
                break;
            }
            ulist[curPos].entry();
            isChanged = true;
        }
        
        if(kDown & KEY_B){
            break;
        }
        
        if(kDown & KEY_PLUS){
            break;
        }
        
        consoleUpdate(NULL);
    }
}

void UI::changeMenu(void){
    int debounce = 0;
    int curPos = 0;
    int pad = 0;
    while(appletMainLoop()){
        consoleClear();
        printf("Change SN - Here you can change your Serial Number: \n\n");
        printf("Press [A] to Select - [B] to exit\n");
        printf("[L] to select LEFT Joycon - [R] to select RIGHT Joycon\n");
        printf("Selected Joycon: \x1B[32m%s Joycon\x1B[0m\n\n", pad ? "RIGHT" : "LEFT");
        
        for(int i=0;i<clistEntries;i++){
            if(curPos != i){
                printf("    %s\n", clist[i].menuList);
            }
            else{
                printf("\x1B[35m--> %s\x1B[0m\n", clist[i].menuList);
            }
        }
        
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        u64 kUp = hidKeysUp(CONTROLLER_P1_AUTO);
        
        if(debounce){
            if(kUp & KEY_UP){
                debounce=0;
            }
            if(kUp & KEY_DOWN){
                debounce=0;
            }
        }
        
        if(kDown & KEY_UP){
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
        
        if(kDown & KEY_DOWN){
            if(debounce==0){
                debounce = 1;
                if(curPos == clistEntries-1){
                    curPos=0;
                }else{
                    curPos++;
                }
            }
        }
        
        if(kDown & KEY_L){
            pad=0;
        }
        
        if(kDown & KEY_R){
            pad=1;
        }
        
        if(kDown & KEY_A){
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
        
        if(kDown & KEY_B){
            break;
        }
        
        if(kDown & KEY_PLUS){
            break;
        }
        
        consoleUpdate(NULL);
    }
}

void UI::restoreMenu(void){
    int debounce = 0;
    int curPos = 0;
    int pad = 0;
    while(appletMainLoop()){
        consoleClear();
        printf("Restore SN - Here you can restore your Serial Number: \n\n");
        printf("Press [A] to Select - [B] to exit\n");
        printf("[L] to select LEFT Joycon - [R] to select RIGHT Joycon\n");
        printf("Selected Joycon: \x1B[32m%s Joycon\x1B[0m\n\n", pad ? "RIGHT" : "LEFT");
        for(int i=0;i<rlistEntries;i++){
            if(curPos != i){
                printf("    %s\n", rlist[i].menuList);
            }
            else{
                printf("\x1B[35m--> %s\x1B[0m\n", rlist[i].menuList);
            }
        }
        
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        u64 kUp = hidKeysUp(CONTROLLER_P1_AUTO);
        
        if(debounce){
            if(kUp & KEY_UP){
                debounce=0;
            }
            if(kUp & KEY_DOWN){
                debounce=0;
            }
        }
        
        if(kDown & KEY_UP){
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
        
        if(kDown & KEY_DOWN){
            if(debounce==0){
                debounce = 1;
                if(curPos == rlistEntries-1){
                    curPos=0;
                }else{
                    curPos++;
                }
            }
        }
        
        if(kDown & KEY_L){
            pad=0;
        }
        
        if(kDown & KEY_R){
            pad=1;
        }
        
        if(kDown & KEY_A){
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
        
        if(kDown & KEY_B){
            break;
        }
        
        if(kDown & KEY_PLUS){
            break;
        }
        
        consoleUpdate(NULL);
    }
}

void UI::restoreListFiles(void){
    File *dfile = new File(strdup("."));
    char **list = dfile->getFileList();
    int listLength = dfile->getFileListLength();
    int curPos = 0;
    int debounce = 0;
    int pad = 0;
    while(1){
        consoleClear();
        printf("Select a file to Restore.\n\nPress [A] to Select - [B] to exit \n[L] to select LEFT Joycon - [R] to select RIGHT Joycon\n");
        printf("Selected Joycon: \x1B[32m%s Joycon\x1B[0m\n\n", pad ? "RIGHT" : "LEFT");
        for(int i=0;i<listLength;i++){
            if(curPos != i){
                printf("    %-50s\tSize: %liBytes\n", list[i], dfile->getFileSize(list[i]));
            }
            else{
                printf("\x1B[35m--> %-50s\tSize: %liBytes\x1B[0m\n", list[i], dfile->getFileSize(list[i]));
            }
        }
        consoleUpdate(NULL);
        
        hidScanInput();
        
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        u64 kUp = hidKeysUp(CONTROLLER_P1_AUTO);
        
        if(debounce){
            if(kUp & KEY_UP){
                debounce=0;
            }
            if(kUp & KEY_DOWN){
                debounce=0;
            }
        }
        
        if(kDown & KEY_UP){
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
        
        if(kDown & KEY_DOWN){
            if(debounce==0){
                debounce = 1;
                if(curPos == listLength-1){
                    curPos=0;
                }else{
                    curPos++;
                }
            }
        }
        
        if(kDown & KEY_L){
            pad=0;
        }
        
        if(kDown & KEY_R){
            pad=1;
        }
        
        if(kDown & KEY_A){
            if(strcmp(list[curPos], "EMPTY") != 0){
                UI::confirmWrite(pad, list[curPos]);
                svcSleepThread(3*1000000000ull);
            }
        }
        
        if(kDown & KEY_B){
            break;
        }
    }
    delete dfile;
}

void UI::backupListFiles(void){
    File *dfile = new File(strdup("."));
    char **list = dfile->getBackupFileList();
    int listLength = dfile->getFileListLength();
    int curPos = 0;
    int debounce = 0;
    int pad = 0;
    while(1){
        consoleClear();
        printf("Select a file to Backup.\n\nPress [A] to Select - [B] to exit \n[L] to select LEFT Joycon - [R] to select RIGHT Joycon\n");
        printf("Selected Joycon: \x1B[32m%s Joycon\x1B[0m\n\n", pad ? "RIGHT" : "LEFT");
        for(int i=0;i<listLength;i++){
            if(curPos != i){
                if(strcmp(list[i], "[NEW FILE]") == 0){
                    printf("    %-50s\n", list[i]);
                }
                else{
                    printf("    %-50s\tSize: %liBytes\n", list[i], dfile->getFileSize(list[i]));
                }
            }
            else{
                if(strcmp(list[i], "[NEW FILE]") == 0){
                    printf("\x1B[35m--> %-50s\x1B[0m\n", list[i]);
                }
                else{
                    printf("\x1B[35m--> %-50s\tSize: %liBytes\x1B[0m\n", list[i], dfile->getFileSize(list[i]));
                }
            }
        }
        consoleUpdate(NULL);
        
        hidScanInput();
        
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        u64 kUp = hidKeysUp(CONTROLLER_P1_AUTO);
        
        if(debounce){
            if(kUp & KEY_UP){
                debounce=0;
            }
            if(kUp & KEY_DOWN){
                debounce=0;
            }
        }
        
        if(kDown & KEY_UP){
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
        
        if(kDown & KEY_DOWN){
            if(debounce==0){
                debounce = 1;
                if(curPos == listLength-1){
                    curPos=0;
                }else{
                    curPos++;
                }
            }
        }
        
        if(kDown & KEY_L){
            pad=0;
        }
        
        if(kDown & KEY_R){
            pad=1;
        }
        
        if(kDown & KEY_A){
            JCFirm *reader = new JCFirm();
            if(strcmp(list[curPos], "[NEW FILE]") == 0){
                char *file = launchKeyboard(strdup("Enter new Filename:"), strdup(""), 20);
                reader->dumpFirmwareFile(pad, file);
                free(file);
            }else{
                reader->dumpFirmwareFile(pad, list[curPos]);
            }
            delete reader;
            svcSleepThread(3*1000000000ull);
        }
        
        if(kDown & KEY_B){
            break;
        }
    }
    delete dfile;
}

void UI::confirmWrite(int padnum, char *sFile){
    consoleClear();
    printf("Are you sure to flash the Backup to your Joycon?\n");
    printf("Press [Y] to continue, [B] to exit\n\n");
    consoleUpdate(NULL);
    while(1){
        hidScanInput();
        
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        
        if(kDown & KEY_Y){
            printf("\x1B[31mIT MAY PERMANENTLY DAMAGE YOUR JOYCON!\n");
            printf("DO NOT UNPLUG ANY JOYCON IN FLASHING STATE!\n");
            printf("USING A WRONG FIRMWARE FOR THE WRONG JOYCON WILL BRICK YOUR JOYCON!\n");
            printf("YOU CANNOT INTERRUPT THIS TASK!\n");
            printf("I WILL NOT TAKE ANY RISK FOR YOUR FAILURE!\nARE YOU SURE?\n");
            printf("Press [ZR] to continue, [B] to exit\n\n");
            consoleUpdate(NULL);
            while(1){
                hidScanInput();
                
                u64 nDown = hidKeysDown(CONTROLLER_P1_AUTO);
                
                if(nDown & KEY_ZR){
                    appletBeginBlockingHomeButtonShortAndLongPressed(0);
                    JCFirm *writer = new JCFirm();
                    int ret = writer->writeFirmwareFile(padnum, sFile);
                    delete writer;
                    appletEndBlockingHomeButtonShortAndLongPressed();
                    if((ret == -621) || (ret == -622)){
                        printf("Wrong firmware for this Joycon!!\n");
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
                else if(nDown & KEY_B)
                {
                    printf("\x1B[0mCANCELED!\n");
                    consoleUpdate(NULL);
                    break;
                }
            }
            break;
        }
        else if(kDown & KEY_B){
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

