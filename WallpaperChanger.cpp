#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <ctype.h>
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>

const int TOTAL_FILES_AMOUNT = 6;

std::string wallpapers[4];                           // список обоев
std::string path = "Wallpapers";                     // путь к файлу
std::string allFiles[6];                             // список всех файлов программы
std::string error = "Не найдены следующие файлы:\n"; // текст ошибки
std::string currentDate;                             // текущая дата
std::string currentTime;                             // текущее время
std::string currentWallpaper;                        // текущие обои
std::string currentPath;                             // текущий путь к папке программы
std::string wallpaperPath;                           // путь к обоям
std::string thisExe;                                 // путь к ехешнику программы

std::fstream logFile; // файл логов
std::fstream config;  // файл конфигурации программы

int day;              // текущий день
int month;            // текущий месяц
int year;             // текущий год
int hour;             // текущий час
int minute;           // текущая минута
int second;           // текущая секунда

//Получение текущего пути
std::string GetCurrentPath(char* argv)
{
    std::string currPath = argv;

    bool gotSlash = false;
    std::string reversedPath = "";

    for (int i = currPath.length() - 1; i >= 0; i--)
    {
        if (currPath[i] == '\\') gotSlash = true;
        if (gotSlash) reversedPath += currPath[i];
    }
    std::string path = reversedPath;
    for (int i = 0, j = reversedPath.length() - 1; i < reversedPath.length(); i++, j--)
        path[j] = reversedPath[i];

    return path;
}


//Получение текущих даты и времени
void getCurrentDateTime()
{
    time_t now = time(0);
    struct tm tstruct;
    tstruct = *localtime(&now);

    day =         tstruct.tm_mday;
    month =       tstruct.tm_mon + 1;
    year =        tstruct.tm_year + 1900;
    hour =        tstruct.tm_hour;
    minute =      tstruct.tm_min;
    second =      tstruct.tm_sec;

    std::string hourStr   = (hour < 10)   ? ("0" + std::to_string(hour))   : std::to_string(hour);
    std::string minuteStr = (minute < 10) ? ("0" + std::to_string(minute)) : std::to_string(minute);
    std::string secondStr = (second < 10) ? ("0" + std::to_string(second)) : std::to_string(second);
    std::string dayStr =    (day < 10)    ? ("0" + std::to_string(day))    : std::to_string(day);
    std::string monthStr =  (month < 10)  ? ("0" + std::to_string(month))  : std::to_string(month);
    std::string yearStr =   std::to_string(year);

    currentDate = dayStr +  "." + monthStr +  "." + yearStr;   // текущая дата
    currentTime = hourStr + ":" + minuteStr + ":" + secondStr; // текущее время
}


//Проверка на наличие файла
bool fileExists(std::string filePath)
{
    std::ifstream check(filePath);
    bool result = check.good();
    check.close();
    return result;
}


//Проверка файла и создание логов
void checkFiles(std::string currentPath)
{
    bool anyFilesNotFound = false;
    allFiles[0] = "config.txt";     // файл конфигурации программы
    allFiles[1] = "igcmdWin10.exe"; // программа заменяет обои на экране блокировки

    for (int i = 2; i < TOTAL_FILES_AMOUNT; i++)
    {
        allFiles[i] = path +"\\" + wallpapers[i - 2];
    }
                                                                    
    std::string desktop;                                                    // путь к рабочему столу
    desktop = std::string(getenv("USERPROFILE")) + "\\Desktop";             // путь к рабочему столу
    bool logFileOpened = false;                                             // файл логов открыт
    getCurrentDateTime();                                                   // обновить значения даты и времени

    //Создание файла логов и запись в него ошибки
    std::string logFilePath = desktop + "\\WallpaperChanger error log.txt"; // путь к файлу логов
    
    error = "====================[" + currentDate + " " + currentTime + "]====================\nНе найдены следующие файлы: \n";
    
    if (fileExists(logFilePath))
        error = "\n\n" + error;
    
    int count = 1;
    std::string missingFile;

    for (int i = 0; i < TOTAL_FILES_AMOUNT; i++) //проверка главных файлов
    {
        if (i < 2)
        {
            if (!fileExists((currentPath + allFiles[i]).c_str()))
            {
                missingFile = allFiles[i];
                error += std::to_string(count) + ") " + missingFile + "\n"; // Текст ошибки
                count++;
                error += "Проверьте корневую папку программы: " + currentPath + "\n\n";
                logFile.open(logFilePath.c_str(), std::ios::app);
                logFile << error;
                logFile.close();
                anyFilesNotFound = true;
            }
            return;
        }
        else
        {
            missingFile = " (" + currentPath + allFiles[i] + ")";
            if (i == 2) missingFile = "Утренние обои" + missingFile;
            else if (i == 3) missingFile = "Дневные обои" + missingFile;
            else if (i == 4) missingFile = "Вечерние обои" + missingFile;
            else if (i == 5) missingFile = "Ночные обои" + missingFile;
            error += std::to_string(count) + ") " + missingFile + "\n"; // Текст ошибки
            count++;
            anyFilesNotFound = true;
        }
    }
    if (anyFilesNotFound)
    {
        logFile.open(logFilePath.c_str(), std::ios::app);
        std::cout << error << std::endl;
        error += "Проверьте корневую папку программы: " + currentPath + "\n\n";
        logFile << error;
        logFile.close();
        logFileOpened = false;
        system("pause");
    }
}


//Чтение конфига
void readConfig(std::string currentPath)
{
    short index = 0;
    config.open(currentPath+"config.txt");
    while (!config.eof())
    {
        std::string line;
        getline(config, line);
        
        if (line[0] != '#' && line[0] != NULL)
        {
            if (index >= 0 && index <= 3)
            {
                wallpapers[index] = line;
            }
            index++;
        }
    }
    config.close();
}


//Установка обоев
void setWP(std::string wallpaperPath)
{
    getCurrentDateTime();

    if      (hour < 6)                currentWallpaper = wallpapers[3]; // ночь
    else if (hour >= 6  && hour < 12) currentWallpaper = wallpapers[0]; // утро
    else if (hour >= 12 && hour < 18) currentWallpaper = wallpapers[1]; // день
    else if (hour >= 18)              currentWallpaper = wallpapers[2]; // вечер
    std::string wp = wallpaperPath + "\\" + currentWallpaper;
    //std::cout << hour << ":" << minute << " " << wp << std::endl;

    //установка обоев на рабочий стол
    SystemParametersInfoA(20, 0, (PVOID)wp.c_str(), 3); 

    //установка обоев на экран блокировки
    std::string command = "igcmdWin10.exe setlockimage \"" + wp + "\"";
    system(command.c_str());
}


int main(int argc, char* argv[])
{
    ShowWindow(GetConsoleWindow(), SW_HIDE); //окно консоли не открывается
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    currentPath = GetCurrentPath(argv[0]);

    addToAutoStart(currentPath, argv[0]);

    wallpaperPath = currentPath + path;

    const int MINUTES = 60 * 1000; //60 секунд

    getCurrentDateTime();

    readConfig(currentPath);
    setWP(wallpaperPath);

    Sleep(60 - second);
    
REFRESH_WALLPAPER:
    readConfig(currentPath);
    checkFiles(currentPath);
    setWP(wallpaperPath);
    Sleep(2 * MINUTES);
    goto REFRESH_WALLPAPER;

    return 0;
}
