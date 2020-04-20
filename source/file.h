#include <switch.h>
#include <iostream>
#include <string.h>
#include <dirent.h>

class File{
public:
    File(void);
    File(char *dir);
    ~File(void);
    
    bool setFileDirectory(char *dir);
    char **getFileList(void);
    char **getBackupFileList(void);
    int getFileListLength(void);
    static int isDirectory(const char *path);
    bool loadFileIntoBuffer(const char *file, void *buffer, size_t length);
    static size_t getFileSize(const char *file);
    
private:
    char *ldir;
    int fileListLength = 0;
    
    char **listOfDir;
};
