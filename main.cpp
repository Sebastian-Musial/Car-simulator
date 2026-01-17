#include <iostream>
#include <string>
#include <chrono>   //Biblioteka do obsługi czasu  potrzebne dla pomiaru czasu i jednostek czasu 
#include <thread>   //Biblioteka do obslugi wielowatkowosci i zarzadzania czasem w aplikacji - zarządza wątkami np. w sleep_for aby uspic petle
#include <cstdio>   //Biblioteka dla wypisywania danych np, printf
#include <windows.h>    //Biblioteka windows API - potrzebna do skorzystania z GetAsyncKeyState
#include "class.hpp"
#include "Physics.hpp"
#include <iomanip>

using namespace std;

extern double TEST_STOP (Car & X, const double & DT);

#ifdef _WIN32   //Wlaczenie ANSI (Virtual Terminal) - rozwiazanie problemu migotania konsoli
void EnableVTMode()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

//Instrukcje preprocesora dla kompilacji dla windowsa oraz nie windowsa np. linux
#ifdef _WIN32   //Windows
    //Stan bitu 15 sprawdza czy klawisz jest teraz wcisniety - sprawdza czy klawisz jest trzymany. Dla samego wcisnięcia sprawdzany jest bit 0 - (0x0001)
    bool Key_Throttle() { return (GetAsyncKeyState(VK_UP)    & 0x8000) != 0; }  //Test czy klawisz sztalki w gore jest wcisniety dla throttle
    bool Key_Brake()    { return (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0; }  //Test czy klawisz spacji jest wcisniety dla brake
    bool Key_Quit()     { return (GetAsyncKeyState('Q')      & 0x8000) != 0; }  //Test czy klawisz Q jest wcisniety dla Quit
    bool Key_Engine()   { return (GetAsyncKeyState('E')      & 0x0001) != 0; }  //Test czy klawisz E jest wcisniety dla Engine
    bool Key_Refuel()   { return (GetAsyncKeyState('R')      & 0x0001) != 0; }  //Test czy klawisz R jest wcisniety dla Engine

    bool Key_Consume_Normal()   { return (GetAsyncKeyState('1')      & 0x0001) != 0; }  //Test czy klawisz R jest wcisniety dla Engine
    bool Key_Consume_Eco()      { return (GetAsyncKeyState('2')      & 0x0001) != 0; }  //Test czy klawisz R jest wcisniety dla Engine
    bool Key_Consume_Sport()    { return (GetAsyncKeyState('3')      & 0x0001) != 0; }  //Test czy klawisz R jest wcisniety dla Engine

    bool Key_Gear_Up()   { return (GetAsyncKeyState('A')      & 0x0001) != 0; }  //Test czy klawisz A jest wcisniety dla zwiększenia biegu 
    bool Key_Gear_Down()   { return (GetAsyncKeyState('Z')      & 0x0001) != 0; }  //Test czy klawisz Z jest wcisniety dla zmniejszenia biegu
    bool Key_ShiftPolicy_Transmission()   { return (GetAsyncKeyState('M')      & 0x0001) != 0; }  //Test czy klawisz M jest wcisniety dla zmiany trbu manual/auto 

//#else   //Tutaj powinna byc instrukcja dla linux
#endif

double position;
int Choice; //Zmienna z ktorej bede korzystal do wyborow uzytkownika typu tak/nie
enum class Test_X {off, on, first_time};    //Zmienna typu wyliczeniowego do weryfikacji stanu danego testu
Test_X Test = Test_X::first_time;   //Nazwa zmiennej jest chwilowa poniewaz nie wykorzystuje innych testow w programie. W przyszlosci moze sie zmienic

int main ()
{
    #ifdef _WIN32
        EnableVTMode();
    #endif
    cout << "\x1b[2J\x1b[H"; // wyczyść ekran + kursor do (0,0)

    Car Audi;

        cout << "Czy chcesz uruchomic test po jakiej odleglosci auto rozpedzon do 50 km/h sie zatrzyma?\n1 - Tak / 2 - Nie" << endl;
        cout << "Dokonaj wyboru wpisujac cyfre a nastepnie naciskajac enter: ";
        cin >> Choice;
        cout << "\n";
        if (Choice == 1) Test = Test_X::on;
        else 
        {
        #ifdef _WIN32   //Czyszczenie konsoli - Windows
            system("cls");
        #else           //Czyszczenei konsoli - Nie windows, wersja linux albo mac. W obecnej chwili program dziala tylko dla windows.
            system("clear");
        #endif
        }

    while (true) {
        if (Key_Quit()) break;

        if(Test == Test_X::on) position = TEST_STOP(Audi, DT);

        if ((Test == Test_X::on) && (Audi.get_CarSpeed() == 0.0))
        {
        #ifdef _WIN32   //Czyszczenie konsoli - Windows
            system("cls");
        #else           //Czyszczenei konsoli - Nie windows, wersja linux albo mac. W obecnej chwili program dziala tylko dla windows.
            system("clear");
        #endif

            cout << "Test po jakiej odleglosci auto rozpedzone do 50 km/h sie zatrzymuje wlasnie sie zakoczyl"
                "\nAuto zatrzymalo sie po "  << position << " metrach"
                "\n\nZa 5 sekund program powroci do normalnego dzialania i bedziesz mogl kontrolowac autem" << endl;
            this_thread::sleep_for(chrono::seconds(5));

            Test = Test_X::off;

        #ifdef _WIN32
            system("cls");  //Czyszczenie konsoli - Windows
        #else
            system("clear");    //Czyszczenei konsoli - Nie windows, wersja linux albo mac. W obecnej chwili program dziala tylko dla windows.
        #endif
        } 

        bool Thr = Key_Throttle();
        bool Brk = Key_Brake();

        if(Test != Test_X::off) Audi.Speed_Update(DT, Thr, Brk);     //Sprawdzenie czy nie dziala w tle test dla sprawdzania odleglosci

        if (Key_Engine()) {
            if (Audi.get_Engine().Engine_is_On()) Audi.get_Engine().set_Engine_Off();
            else Audi.get_Engine().set_Engine_On(); 
        }

        if (Key_Refuel()) {
            Audi.get_Car_FuelTank().Refuel(5);  //Zgodnie z trescia zadania +5 L paliwa tankujemy
        }

        if (Audi.get_Engine().Engine_is_On()) {
            if (Key_Consume_Normal())   {Audi.get_Engine().set_Consumption_Normal();}
            else if (Key_Consume_Eco())      {Audi.get_Engine().set_Consumption_Eco();}
            else if (Key_Consume_Sport())    {Audi.get_Engine().set_Consumption_Sport();} 
        }

        //Obsluga skrzyni biegu

        if (!Audi.get_Car_Transmission().isAuto()) {
            if (Key_Gear_Up()) {
                Audi.get_Car_Transmission().Update_Shift(G_Shift::Up);
            }
            if (Key_Gear_Down()) {
                Audi.get_Car_Transmission().Update_Shift(G_Shift::Down);
            }
        }

        
        if (Key_ShiftPolicy_Transmission()) {
            Audi.get_Car_Transmission().change_ShiftPolicy();
        }

        //Wypisanie tekstu z informacjami w czasie rzeczywistym
        /*printf("\rspeed=%6.2f km/h   throttle=%.2f   brake=%.2f   fuel=%.2f L   engine=%s   ",
                    Audi.get_CarSpeed() * 3.6 , Audi.get_CatThrottle(), Audi.get_CarBrake(),    //Wystepuje tutaj mnozenie przez 3.6 w celu zamiany jednostki m/s na km/h
                    Audi.get_Car_FuelTank().get_FuelTank_Level(),
                    Audi.get_Engine().Engine_is_On() ? "ON" : "OFF",
                    "Momentary Fuel Consumption: ", Audi.get_Trip_Computer().get_Momentary_Fuel_Consumption_100KM(),
                    "Average Fuel Consumption: ", Audi.get_Trip_Computer().get_Average_Fuel_Consumption(),
                    "Distance: ", Audi.get_Trip_Computer().get_Distance(),
                    "Engine Work time: ", Audi.get_Trip_Computer().get_Work_Time(),
                    "Consumption Fuel Model: ", %6s Audi.get_Engine().Check_Consumption());*/
        /*#ifdef _WIN32
        system("cls");
        #else           //Czyszczenei konsoli - Nie windows, wersja linux albo mac. W obecnej chwili program dziala tylko dla windows.
        system("clear");
        #endif */  
        
        //czyszczenie ekranu bez miogotania i bez pozostawiania blednych liter na koncu wyrazu
        cout << "\x1b[" << 14 << "A";
        for(int i=0;i<13;i++) cout << "\x1b[2K\n";
        cout << "\x1b[" << 14 << "A";

        cout << fixed << setprecision(2);
        cout << "===CAR INFORMATION===\n"
            << "\nUP = throttle, SPACE = brake, Q = quit, E = Engine ON/OFF, R - Refuel 1/2/3 - Consumption model Normal/Eco/Sport\n"
            << "\nA - GearUp, Z - GearDown, M - ShiftPolicy[Manual/Auto]\n"
            << "\nSpeed:" << Audi.get_CarSpeed() * 3.6 << " km/h " << "  Throttle: " << Audi.get_CatThrottle() << "  Brake= " << Audi.get_CarBrake()
            << " Engine: " << setw(4) <<(Audi.get_Engine().Engine_is_On() ? "ON" : "OFF")
            << "\nFuel: " << Audi.get_Car_FuelTank().get_FuelTank_Level() << " Consumption Fuel Model: "<< setw(7) << Audi.get_Engine().Check_Consumption()
            << "\nEngine Work time: " << Audi.get_Trip_Computer().get_Work_Time()
            << "\nMomentary Fuel Consumption: " << Audi.get_Trip_Computer().get_Momentary_Fuel_Consumption_100KM()
            << "\nAverage Fuel Consumption: " << Audi.get_Trip_Computer().get_Average_Fuel_Consumption()
            << "\nDistance: " << Audi.get_Trip_Computer().get_Distance()
            << "\nCurrent gear: " << Audi.get_Car_Transmission().get_Current_Gear()
            << "\nRPM: " << Audi.get_Car_Transmission().get_RPM()
            << "\nShiftPolicy transmission: " << setw(7) << Audi.get_Car_Transmission().Check_ShiftPolicy();
            //<< flush;

        this_thread::sleep_for(chrono::milliseconds(16));
    }
    return 0;
}
