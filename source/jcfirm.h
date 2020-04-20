#include <switch.h>
#include <stdio.h>
#include <string.h>

class JCFirm{
public:
    JCFirm();
    ~JCFirm();
    
    bool getUIDFromPads(void);
    bool dumpFirmwareFile(int padnum, char *sFile);
    int writeFirmwareFile(int padnum, char *sFile);
    char *getSNFromPad(int padnum);
    void getSNFromPad(char *sn0, char *sn1);
    char *getSNFromPadBackup(int padnum);
    void getSNFromPadBackup(char *sn0, char *sn1);
    int getTypeFromPad(int padnum);
    bool changeSN(int padnum);
    bool restoreSN(int padnum);
    bool backupSN(int padnum);
    float getProgress(void);
    int getEntries(void);
    
protected:
    u64 PadIds[2];
    float progress;
    FILE *fd;
    void *firmBuffer;
    char *retSN;
    void *rBuffer;
    char SN0[14];
    char SN1[14];
    s32 entries;
};
