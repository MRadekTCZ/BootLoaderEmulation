#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include <dlfcn.h>
#include <sys/stat.h>
#define MAX_PATH 260 // Definicja maksymalnej długości ścieżki
#define PATH_MAX 260 // Definicja maksymalnej długości ścieżki
#define N_APPS 3
#include <dlfcn.h>
int CopyDLLWithCopySuffix(wchar_t *fileName) {
    if (access(fileName, F_OK) != -1) {
        char copiedFileName[FILENAME_MAX];
        strcpy(copiedFileName, fileName);
        char* dotPosition = strrchr(copiedFileName, '.');
        if (dotPosition != NULL) {
            strcpy(dotPosition, "_copy.so");
        } else {
            strcat(copiedFileName, "_copy.so");
        }

        if (rename(fileName, copiedFileName) == 0) {
            return 0x0;
        } else {
            // Handle error while renaming
            return 0xF1;
        }
    } else {
        // File does not exist
        return 0xF3;
    }
}
int checkFilesExistence(int n) {
    for (int i = 1; i <= n; ++i) {
        wchar_t filename[PATH_MAX];
        snprintf(filename, sizeof(filename), "app%d.so", i);

        FILE* file = fopen(filename, "r");
        if (file == NULL) {
            return 0xF1; // Zwrócenie false jeśli plik nie istnieje
        } else {
            fclose(file);
        }
    }

    return 0x1; // Zwrócenie true jeśli wszystkie pliki istnieją
}
int checkCopiedFilesExistence(int n) {
    int result = 0;

    for (int i = 1; i <= n; ++i) {
        wchar_t filename[MAX_PATH];
        snprintf(filename, sizeof(filename), "app%d_copy.so", i);

        FILE* file = fopen(filename, "r");
        if (file == NULL) {
            result = 0xF1; // Ustawienie wyniku na 0xF1 jeśli plik nie istnieje
            break; // Przerwanie pętli, gdy pojawi się pierwszy nieistniejący plik
        } else {
            fclose(file);
        }
    }

    return result;
}

int APP3(int* iCycleCount) {
    
    typedef void (*MyFunction)(); // Definicja typu wskaźnika na funkcję

    const char* soPath = "./OS3.so"; // Ścieżka do pliku .so, możesz dostosować nazwę i ścieżkę

    void* handle = dlopen(soPath, RTLD_LAZY);
    if (handle != NULL) {
        // printf("SO loaded successfully.\n");

        // Pobranie adresu funkcji z biblioteki
        MyFunction myFunc = (MyFunction)dlsym(handle, "os3");
        if (myFunc != NULL) {
            // Wywołanie funkcji z biblioteki
            myFunc();
        } else {
            printf("Error getting the function address: %s\n", dlerror());
        }

        dlclose(handle); // Zamknięcie biblioteki po zakończeniu pracy
    } else {
        printf("Error loading the SO: %s\n", dlerror());
    }
    
    *iCycleCount = *iCycleCount + 1;
    return *iCycleCount;
    
       printf("Trzecia \n");
    return *iCycleCount;
}

int APP1(int* iCycleCount)
{
    printf("Pierwsza \n");
    return *iCycleCount;
}
int APP2(int* iCycleCount)
{   
    printf("Druga \n");
    return *iCycleCount;
}
int FBL(int *iCycleCount)
{
    
    int result_error = 8;
    
    for (int i = 1; i <= N_APPS; ++i) {
        wchar_t dllFileName[MAX_PATH];
        snprintf(dllFileName, sizeof(dllFileName), "app%d.so", i); // Tworzenie nazwy pliku DLL: app1.dll, app2.dll, ..., appn.dll
        result_error = CopyDLLWithCopySuffix(dllFileName);
        if (result_error) break;
    }
    
    //*iCycleCount = *iCycleCount + 1;
    return result_error;
}

int ixCycleCount = 0;
typedef int (*FunctionStackPtr)(int*);
int main()
{
    
    int iErrorCode1;
    FunctionStackPtr actual_function_ptr[N_APPS + 1] = { FBL, APP1, APP2, APP3 };
    printf("Boot Start \n");
    //Beep(2000, 500);
    iErrorCode1 = FBL(&ixCycleCount);
    
    while (1)
    {
       
        int checkResult_copies = checkCopiedFilesExistence(N_APPS);
        int checkResult_origin = checkFilesExistence(N_APPS);
        printf("Kopie: %d, Oryginaly: %d", checkResult_copies, checkResult_origin);
        
        if (!checkResult_copies && checkResult_origin)
        {
            for (int a = 1; a <= N_APPS; a++)
            {
                actual_function_ptr[a]((int*)&ixCycleCount);
            }
            printf("APP mode \n");
        }
        else
        {
            //iErrorCode1 = FBL(&ixCycleCount);
            iErrorCode1 = actual_function_ptr[0]((int*)&ixCycleCount);
            printf("FBL mode \n");
        }

        


        printf("Liczba wykonan programu: %d \n", ixCycleCount);
        
        sleep(5);
    }

    return 0;
}
