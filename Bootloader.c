// Bootloader.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#define N_APPS 3
int CopyDLLWithCopySuffix(LPCWSTR fileName) {
    if (GetFileAttributesW(fileName) != INVALID_FILE_ATTRIBUTES) {
        wchar_t copiedFileName[MAX_PATH];
        wcscpy_s(copiedFileName, MAX_PATH, fileName);
        wchar_t* dotPosition = wcsrchr(copiedFileName, L'.');
        if (dotPosition != NULL) {
            wcscpy_s(dotPosition, MAX_PATH - (dotPosition - copiedFileName), L"_copy.dll");
        }
        else {
            wcscat_s(copiedFileName, MAX_PATH, L"_copy.dll");
        }

        if (CopyFileW(fileName, copiedFileName, FALSE)) {
            wprintf(L"File copied successfully as: %s\n", copiedFileName);

            if (DeleteFileW(fileName)) {
                wprintf(L"Original file deleted: %s\n", fileName);
                return 0x0;
            }
            else {
                DWORD error = GetLastError();
                wprintf(L"Error deleting the original file: %lu\n", error);
                return 0xF1;
            }
        }
        else {
            DWORD error = GetLastError();
            wprintf(L"Error copying the file: %lu\n", error);
            return 0xF2;
        }
    }
    else {
        wprintf(L"The file does not exist.\n");
        return 0xF3;
    }
}

int OS1(int *iCycleCount)
{
    const char* originalFileName = "OS3.exe";
    const char* copiedFileName = "copy.exe";

    if (CopyFile(originalFileName, copiedFileName, FALSE)) {
        printf("File copied successfully.\n");
    }

        
    system("copy.exe");
    *iCycleCount = *iCycleCount + 1;
    return *iCycleCount;
}
int OS2(int* iCycleCount)
{   
    LPCWSTR originalFileName = L"OS4.exe";
    LPCWSTR copiedFileName = L"OS4_copy.exe";

    if (CopyFileW(originalFileName, copiedFileName, FALSE)) {
        wprintf(L"File copied successfully.\n");
    }

    system("OS4_copy.exe");
    *iCycleCount = *iCycleCount + 1;
    return *iCycleCount;
}
int OS3(int* iCycleCount)
{
    typedef void (*MyFunction)(); // Define function pointer type
    LPCWSTR dllPath = L"OS3.dll"; // Replace with the path to your DLL

    HMODULE hDLL = LoadLibraryW(dllPath);
    if (hDLL != NULL) {
        wprintf(L"DLL loaded successfully.\n");

        // Get the address of the function from the DLL
        MyFunction myFunc = (MyFunction)GetProcAddress(hDLL, "os3");
        if (myFunc != NULL) {
            // Call the function from the DLL
            myFunc();
        }
        else {
            wprintf(L"Error getting the function address.\n");
        }

        FreeLibrary(hDLL); // Unload the DLL when done using it
    }
    else {
        DWORD error = GetLastError();
        wprintf(L"Error loading the DLL: %lu\n", error);
    }
    *iCycleCount = *iCycleCount + 1;
    return *iCycleCount;
}
enum Bootprog { BM, APP }bootprog;

int FBL(int* iCycleCount)
{
    
    int result_error;
    for (int i = 1; i <= N_APPS; ++i) {
        wchar_t dllFileName[MAX_PATH];
        swprintf_s(dllFileName, MAX_PATH, L"app%d.dll", i); // Tworzenie nazwy pliku DLL: app1.dll, app2.dll, ..., appn.dll
        result_error = CopyDLLWithCopySuffix(dllFileName);
        if (result_error) break;
    }
    *iCycleCount = *iCycleCount + 1;
    printf("Liczba wykonan programu: %d \n", *iCycleCount);
    return result_error;

}

int main()
{
    int ixCycleCount = 0;
    int iErrorCode1;
    while (1)
    {
        iErrorCode1 = FBL(&ixCycleCount);
        printf("ErrorCode: %d \n", iErrorCode1);
        Sleep(2000);
    }

    return 0;
}

/*tutaj jest jak usunac plik*/
/*
    LPCWSTR fileNameToDelete = L"fileToDelete.txt"; // Replace with the file you want to delete

    if (DeleteFileW(fileNameToDelete)) {
        wprintf(L"File deleted successfully.\n");
    } else {
        DWORD error = GetLastError();
        wprintf(L"Error deleting the file: %lu\n", error);
    }

    return 0;
    */