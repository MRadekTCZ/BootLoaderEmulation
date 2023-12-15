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
            //wprintf(L"File copied successfully as: %s\n", copiedFileName);

            if (DeleteFileW(fileName)) {
                //wprintf(L"Original file deleted: %s\n", fileName);
                return 0x0;
            }
            else {
                DWORD error = GetLastError();
                //wprintf(L"Error deleting the original file: %lu\n", error);
                return 0xF1;
            }
        }
        else {
            DWORD error = GetLastError();
            //wprintf(L"Error copying the file: %lu\n", error);
            return 0xF2;
        }
    }
    else {
        //wprintf(L"The file does not exist.\n");
        return 0xF3;
    }
}
int checkFilesExistence(int n) {
    int result = 0;

    for (int i = 1; i <= n; ++i) {
        wchar_t filename[MAX_PATH];
        swprintf(filename, MAX_PATH, L"app%d.dll", i);

        FILE* file;
        if (_wfopen_s(&file, filename, L"r") != 0) {
            result = 0xF1; // Ustawienie wyniku na 0xF1 jeśli plik nie istnieje
            break; // Przerwanie pętli, gdy pojawi się pierwszy nieistniejący plik
        }
        else {
            fclose(file);
        }
    }

    return result;
}
int checkCopiedFilesExistence(int n) {
    int result = 0;

    for (int i = 1; i <= n; ++i) {
        wchar_t filename[MAX_PATH];
        swprintf(filename, MAX_PATH, L"app%d_copy.dll", i);

        FILE* file;
        if (_wfopen_s(&file, filename, L"r") != 0) {
            result = 0xF1; // Ustawienie wyniku na 0xF1 jeśli plik nie istnieje
            break; // Przerwanie pętli, gdy pojawi się pierwszy nieistniejący plik
        }
        else {
            fclose(file);
        }
    }

    return result;
}
int APP1(int *iCycleCount)
{
    printf("Pierwsza \n");
    return *iCycleCount;
}
int APP2(int* iCycleCount)
{   
    printf("Druga \n");
    return *iCycleCount;
}
int APP3(int* iCycleCount)
{
    typedef void (*MyFunction)(); // Define function pointer type
    LPCWSTR dllPath = L"OS3.dll"; // Replace with the path to your DLL

    HMODULE hDLL = LoadLibraryW(dllPath);
    if (hDLL != NULL) {
       // wprintf(L"DLL loaded successfully.\n");

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
    return result_error;

}
typedef int (*FunctionStackPtr)(int);
int main()
{
    int ixCycleCount = 0;
    int iErrorCode1;
    FunctionStackPtr actual_function_ptr[N_APPS + 1] = { FBL, APP1, APP2, APP3 };
    printf("Boot Start \n");
    //Beep(2000, 500);
    iErrorCode1 = FBL(&ixCycleCount);
    
    while (1)
    {
       
        int checkResult_copies = checkCopiedFilesExistence(N_APPS);
        int checkResult_origin = checkFilesExistence(N_APPS);
        //printf("Kopie: %d, Oryginaly: %d", checkResult_copies, checkResult_origin);
        if (!checkResult_copies && checkResult_origin)
        {
            for (int a = 1; a <= N_APPS; a++)
            {
                actual_function_ptr[a](&ixCycleCount);
            }
            printf("APP mode \n");
        }
        else
        {
            //iErrorCode1 = FBL(&ixCycleCount);
            iErrorCode1 = actual_function_ptr[0](&ixCycleCount);
            printf("FBL mode \n");
        }

        


        printf("Liczba wykonan programu: %d \n", ixCycleCount);
        
        Sleep(5000);
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