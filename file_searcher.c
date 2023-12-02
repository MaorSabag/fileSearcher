#include <windows.h>
#include "beacon.h"

WINBASEAPI HANDLE WINAPI KERNEL32$FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
WINBASEAPI BOOL WINAPI KERNEL32$FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);
WINBASEAPI BOOL WINAPI KERNEL32$FindClose(HANDLE hFindFile);
WINBASEAPI DWORD WINAPI KERNEL32$GetLastError(void);
DECLSPEC_IMPORT void* __cdecl MSVCRT$malloc(size_t _Size);
DECLSPEC_IMPORT void __cdecl MSVCRT$free(void* _Block);
DECLSPEC_IMPORT size_t __cdecl MSVCRT$strlen(const char* str);
DECLSPEC_IMPORT int __cdecl MSVCRT$sprintf(char *buffer, const char *format, ...);
DECLSPEC_IMPORT int __cdecl MSVCRT$strcmp(const char* string1, const char* string2);


BOOL isFileInList(char* fileName, char** listOfFiles, int listSize) {
    for (int i = 0; i < listSize; i++) {
        char* fileInList = listOfFiles[i];
        while (*fileName && (*fileName == *fileInList)) {
            fileName++;
            fileInList++;
        }
        if (*fileName == '\0' && *fileInList == '\0') {
            return TRUE;
        }
    }
    return FALSE;
}

void findFileByName(char* fileName, char* path, char** listOfFiles, int* listSize, int* fileCounter) {
    WIN32_FIND_DATAA findData;
    char newPath[MAX_PATH];
    if (path[MSVCRT$strlen(path) - 1] == '\\') {
        MSVCRT$sprintf(newPath, "%s*", path);
    } else {
        MSVCRT$sprintf(newPath, "%s\\*", path);
    }

    HANDLE hFind = KERNEL32$FindFirstFileA(newPath, &findData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (MSVCRT$strcmp(findData.cFileName, ".") != 0 && MSVCRT$strcmp(findData.cFileName, "..") != 0) {
                    char newPath[MAX_PATH];
                    MSVCRT$sprintf(newPath, "%s\\%s", path, findData.cFileName);
                    findFileByName(fileName, newPath, listOfFiles, listSize, fileCounter);
                }
            }
            else {
                if (MSVCRT$strcmp(findData.cFileName, fileName) == 0) {
                    char potentialFile[MAX_PATH];
                    MSVCRT$sprintf(potentialFile, "%s\\%s", path, findData.cFileName);

                    if(!isFileInList(potentialFile, listOfFiles, *listSize)) {
                        BeaconPrintf(CALLBACK_OUTPUT, "File found: %s", potentialFile);
                        listOfFiles[*listSize] = MSVCRT$malloc(MSVCRT$strlen(potentialFile) + 1);
                        MSVCRT$sprintf(listOfFiles[*listSize], "%s", potentialFile);
                        (*listSize)++;
                    }
                }
                (*fileCounter)++;
                if (*fileCounter % 100000 == 0) {
                    BeaconPrintf(CALLBACK_OUTPUT, "Processed %d files", *fileCounter);
                }
            }
        } while (KERNEL32$FindNextFileA(hFind, &findData));
        KERNEL32$FindClose(hFind);
    } else {
        DWORD errorCode = KERNEL32$GetLastError();
        //BeaconPrintf(CALLBACK_ERROR, "Failed to search in directory: \"%s\", error code: %lu", path, errorCode);
    }
}

void go(char* args, int argLen) {
    datap parser;
    char* fileName;

    BeaconDataParse(&parser, args, argLen);
    fileName = BeaconDataExtract(&parser, NULL);

    char** listOfFiles = (char**) MSVCRT$malloc(1024 * sizeof(char*));
    int listSize = 0;
    int fileCounter = 0;

    findFileByName(fileName, "C:\\", listOfFiles, &listSize, &fileCounter);

    if (listSize == 0) {
        BeaconPrintf(CALLBACK_OUTPUT, "File not found");
    }
    else {
        BeaconPrintf(CALLBACK_OUTPUT, "Found %d files", listSize);
    }
    
    MSVCRT$free(listOfFiles);
}