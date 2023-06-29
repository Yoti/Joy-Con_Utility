#include "file.h"
#include <cstdlib>
#include <malloc.h>
#include <dirent.h>

File::File(void){
    //constructor
    setFileDirectory(strdup("."));
}

File::File(char *dir){
    //constructor with argument
    setFileDirectory(dir);
}

File::~File(void){
    //destructor
    if(fileListLength != 0){
        for(int i=0;i<fileListLength;i++){
            char *r = listOfDir[i];
            free(r);
        }
        free(listOfDir);
    }
}

bool File::setFileDirectory(char *dir){
    ldir = dir;
    return true;
}

char **File::getFileList(void){
    if(fileListLength > 0){
        for(int i=0;i<fileListLength;i++){
            char *r = listOfDir[i];
            free(r);
        }
        free(listOfDir);
    }
    listOfDir = (char **)malloc(99 * sizeof(char*));
    fileListLength = 0;

    struct dirent *curdir;
    DIR *dr = opendir(ldir);

    if(dr == NULL){
        return NULL;
    }

    while ((curdir = readdir(dr)) != NULL){
        if(strcmp(curdir->d_name, ".") == 0){
            continue;
        }
        else if(strcmp(curdir->d_name, "..") == 0){
            continue;
        }
        if(isDirectory(curdir->d_name)){
            continue;
        }

        if(fileListLength == 99){
            break;
        }

        if(curdir->d_name[0] == '.'){
            continue;
        }

        int nameLength = strlen(curdir->d_name);
        if(nameLength > 50){
            continue;
        }

        char *r = (char *)malloc(nameLength+1);
        char *filename = strtok(curdir->d_name, ".");
        char *fileext = strtok(NULL, ".");

        if(strcmp(fileext, "bin") != 0){
            continue;
        }

        sprintf(r, "%s.%s", filename, fileext);

        listOfDir[fileListLength] = r;
        fileListLength++;
    }
    closedir(dr);

    if(fileListLength == 0){
        char EMPTY[] = "EMPTY\0";
        char *r = (char *)malloc(strlen(EMPTY)+1);
        strcpy(r, EMPTY);
        listOfDir[fileListLength] = r;
        fileListLength++;
    }

    return listOfDir;
}

char **File::getBackupFileList(void){
    if(fileListLength > 0){
        for(int i=0;i<fileListLength;i++){
            char *r = listOfDir[i];
            free(r);
        }
        free(listOfDir);
    }
    listOfDir = (char **)malloc(99 * sizeof(char*));
    fileListLength = 0;

    struct dirent *curdir;
    DIR *dr = opendir(ldir);

    if(dr == NULL){
        return NULL;
    }
    char NEW[] = "[NEW FILE]\0";
    char *p = (char *)malloc(strlen(NEW));
    strcpy(p, NEW);

    listOfDir[fileListLength] = p;
    fileListLength++;

    while ((curdir = readdir(dr)) != NULL){
        if(strcmp(curdir->d_name, ".") == 0){
            continue;
        }
        else if(strcmp(curdir->d_name, "..") == 0){
            continue;
        }
        if(isDirectory(curdir->d_name)){
            continue;
        }

        if(fileListLength == 99){
            break;
        }

        if(curdir->d_name[0] == '.'){
            continue;
        }

        int nameLength = strlen(curdir->d_name);
        if(nameLength > 50){
            continue;
        }

        char *r = (char *)malloc(nameLength+1);
        char *filename = strtok(curdir->d_name, ".");
        char *fileext = strtok(NULL, ".");

        if(strcmp(fileext, "bin") != 0){
            continue;
        }

        sprintf(r, "%s.%s", filename, fileext);

        listOfDir[fileListLength] = r;
        fileListLength++;
    }
    closedir(dr);

    return listOfDir;
}

int File::getFileListLength(){
    return fileListLength;
}

int File::isDirectory(const char *path) {
   struct stat statBuffer;
    if (stat(path, &statBuffer) != 0){
        return 0;
    }
   return S_ISDIR(statBuffer.st_mode);
}

bool File::loadFileIntoBuffer(const char *file, void *buffer, size_t length){
    length = getFileSize(file);
    buffer = memalign(0x1000, length);
    FILE *fd = fopen(file, "rb");
    if(fd == NULL){
        free(buffer);
        length = -1;
        return false;
    }
    size_t read = fread(buffer, 1, length, fd);
    if(read != length){
        fclose(fd);
        free(buffer);
        length = -1;
        return false;
    }
    fclose(fd);
    return true;
}

size_t File::getFileSize(const char *file){
    if(strcmp(file, "[NEW FILE]") == 0){
        return 0;
    }

    FILE *fd = fopen(file, "rb");
    if(fd == NULL){
        return -1;
    }

    fseek(fd, 0, SEEK_END);
    size_t ret = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    fclose(fd);

    return ret;
}
