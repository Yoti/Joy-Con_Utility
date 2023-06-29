#include "ui.h"
#include <cstdlib>
#include <malloc.h>

JCFirm::JCFirm(){
    getUIDFromPads();
    firmBuffer = memalign(0x1000, 0x200);
    //initialze
}

JCFirm::~JCFirm(){
    free(firmBuffer);
    //deinitialize
}

bool JCFirm::getUIDFromPads(void){
    Result pads;

    pads = hidsysGetUniquePadsFromNpad(CONTROLLER_HANDHELD, PadIds, 2, &entries);
    if(entries != 2){
        return false;
    }
    if(R_SUCCEEDED(pads)){
        return true;
    }
    else{
        return false;
    }

    return false;
}

bool JCFirm::dumpFirmwareFile(int padnum, char *sFile){
    printf("\nDumping..\n");
    printf("Press [B] to cancel\n");

    fd = fopen(sFile, "wb");
    if((fd==NULL) || (firmBuffer == NULL)){
        return false;
    }

    for(int i=0;i<0x400;i++){
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        hiddbgReadSerialFlash(i*0x200, firmBuffer, 0x200, PadIds[padnum]);
        fwrite(firmBuffer, 1, 0x200, fd);

        progress = (float) (i+1) / 0x400 * 100;
        printf("Progress: %.1f%%\r", progress);
        consoleUpdate(NULL);
        if(kDown & HidNpadButton_B){
            printf("\nUSER CANCELED\n");
            break;
        }
    }
    fclose(fd);

    printf("\nDONE!");
    consoleUpdate(NULL);

    return true;
}

int JCFirm::writeFirmwareFile(int padnum, char *sFile){
    size_t filesize = File::getFileSize(sFile);
    if((filesize > 0x80000) || (filesize < 0x80000)){
        return -1;
    }

    fd = fopen(sFile, "rb");
    if((fd==NULL) || (firmBuffer == NULL)){
        return -2;
    }

    char checkSN[15];
    char checkSN1[15];
    char *ptrSN = getSNFromPadBackup(padnum);

    //Original Serial from backup
    fseek(fd, 0x6002, SEEK_SET);
    fread(checkSN, 1, 14, fd);

    //Backup Serial from backup
    fseek(fd, 0xF002, SEEK_SET);
    fread(checkSN1, 1, 14, fd);

    char type[1];
    fseek(fd, 0x6012, SEEK_SET);
    fread(type, 1, 1, fd);

    if(type[0] == 0x01){
        if(getTypeFromPad(padnum) != 1){
            free(ptrSN);
            return -621;
        }
    }
    else if(type[0] == 0x02){
        if(getTypeFromPad(padnum) != 2){
            free(ptrSN);
            return -622;
        }
    }

    fseek(fd, 0, SEEK_SET); //Almost bricked my Joy-Con because I forgot it. ':)

    /*
     We are checking if we are allowed to restore that firmware by checking the
     serial from the dump with the Joy-Con. DO NOT TRY FLASHING THE RIGHT FIRMWARE
     TO A LEFT JOY-CON OR THE OPPOSITE WAY! IT'S A PAIN IN THE A$$ TO FIX THIS!
     */
    if(memcmp(checkSN1, ptrSN, 14) != 0){
        if(memcmp(checkSN, ptrSN, 14) != 0){
            fclose(fd);
            free(ptrSN);
            return -5;
        }
    }
    free(ptrSN);

    for(int i=0;i<0x400;i++){
        fread(firmBuffer, 1, 0x200, fd);
        Result r = hiddbgWriteSerialFlash(i*0x200, firmBuffer, 0x1000, 0x200, PadIds[padnum]);

        if(!R_SUCCEEDED(r)){
            return -3;
        }

        progress = (float) (i+1) / 0x400 * 100;
        printf("\x1B[31mWRITING: %.1f%%\r", progress);
        consoleUpdate(NULL);
    }
    fclose(fd);

    printf("\x1B[0m\nDONE!");
    consoleUpdate(NULL);
    return 0;
}

char *JCFirm::getSNFromPad(int padnum){
    char *retSN = (char *)malloc(14);
    hiddbgReadSerialFlash(0x6002, retSN, 14, PadIds[padnum]);
    return retSN;
}

void JCFirm::getSNFromPad(char *sn0, char *sn1){
    hiddbgReadSerialFlash(0x6002, SN0, 14, PadIds[0]);
    hiddbgReadSerialFlash(0x6002, SN1, 14, PadIds[1]);
    memcpy(sn0, SN0, 14);
    memcpy(sn1, SN1, 14);
}

/*  This checks first if there is a backup serial available. When it's not, asume
    that the serial number on the original location is right.
 */
char *JCFirm::getSNFromPadBackup(int padnum){
    char *retSN = (char *)malloc(14);
    hiddbgReadSerialFlash(0xF002, retSN, 14, PadIds[padnum]);
    if(memcmp(retSN, "X", 1) == 0){
        return retSN;
    }
    free(retSN);
    return getSNFromPad(padnum);
}

/*
    Same as the function above but it copies the SN to a buffer.
 */
void JCFirm::getSNFromPadBackup(char *sn0, char *sn1){
    hiddbgReadSerialFlash(0xF002, SN0, 14, PadIds[0]);
    hiddbgReadSerialFlash(0xF002, SN1, 14, PadIds[1]);
    if(memcmp(SN0, "X", 1) != 0){
        char *sn = getSNFromPad(0);
        memcpy(sn0, sn, 14);
        free(sn);
    }
    else if(memcmp(SN1, "X", 1) != 0){
        char *sn = getSNFromPad(1);
        memcpy(sn1, sn, 14);
        free(sn);
    }

    memcpy(sn0, SN0, 14);
    memcpy(sn1, SN1, 14);
}

int JCFirm::getTypeFromPad(int padnum){
    char type[1];
    hiddbgReadSerialFlash(0x6012, type, 1, PadIds[padnum]);

    if(type[0] == 0x01){
        return 1;
    }
    else if(type[0] == 0x02){
        return 2;
    }

    return 0;
}

bool JCFirm::changeSN(int padnum){
    if(!backupSN(padnum)){
        return false;
    }
    char *origSN = getSNFromPad(padnum);

    char *newSN = UI::launchKeyboard(strdup("Enter your new SN. Cancel will write a ZERO SN!"), origSN, 14);

    rBuffer = memalign(0x1000, 16);

    if(newSN != NULL){
        memset((char*)rBuffer, 0, 16);
        memcpy((char*)rBuffer+2, newSN, 14);
        hiddbgWriteSerialFlash(0x6000, rBuffer, 0x1000, 16, PadIds[padnum]);
    }
    else{
        memset((char*)rBuffer, 0, 16);
        memcpy((char*)rBuffer+2, origSN, 14);
        memset((char*)rBuffer+5, 0x30, 11);
        hiddbgWriteSerialFlash(0x6000, rBuffer, 0x1000, 16, PadIds[padnum]);
    }

    free(newSN);
    free(rBuffer);
    free(origSN);
    return true;
}

bool JCFirm::restoreSN(int padnum){
    rBuffer = memalign(0x1000, 16);
    if(rBuffer == NULL){
        return false;
    }
    hiddbgReadSerialFlash(0xF002, rBuffer, 14, PadIds[padnum]);
    if(memcmp(rBuffer, "X", 1) != 0){
        return false;
    }

    hiddbgReadSerialFlash(0xF000, rBuffer, 16, PadIds[padnum]);
    Result r = hiddbgWriteSerialFlash(0x6000, rBuffer, 0x1000, 16, PadIds[padnum]);
    if(!R_SUCCEEDED(r)){
        return false;
    }

    memset(rBuffer, 0xFF, 16);
    r = hiddbgWriteSerialFlash(0xF000, rBuffer, 0x1000, 16, PadIds[padnum]);
    if(!R_SUCCEEDED(r)){
        return false;
    }

    free(rBuffer);
    return true;
}

bool JCFirm::backupSN(int padnum){
    rBuffer = memalign(0x1000, 16);
    if(rBuffer == NULL){
        return false;
    }
    hiddbgReadSerialFlash(0xF002, rBuffer, 14, PadIds[padnum]);
    if(memcmp(rBuffer, "X", 1) != 0){
        hiddbgReadSerialFlash(0x6000, rBuffer, 16, PadIds[padnum]);

        Result r = hiddbgWriteSerialFlash(0xF000, rBuffer, 0x1000, 16, PadIds[padnum]);
        if(!R_SUCCEEDED(r)){
            free(rBuffer);
            return false;
        }
    }

    free(rBuffer);
    return true;
}

float JCFirm::getProgress(void){
    return progress;
}

s32 JCFirm::getEntries(void){
    return entries;
}
