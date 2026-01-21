#include <iostream>
#include <string>
#include <chrono>   //Biblioteka do obsługi czasu  potrzebne dla pomiaru czasu i jednostek czasu 
#include <thread>   //Biblioteka do obslugi wielowatkowosci i zarzadzania czasem w aplikacji - zarządza wątkami np. w sleep_for aby uspic petle
#include <cstdio>   //Biblioteka dla wypisywania danych np, printf
#include <windows.h>    //Biblioteka windows API - potrzebna do skorzystania z GetAsyncKeyState
#include "class.hpp"
#include "Physics.hpp"
#include "Test.cpp"
#include <iomanip>

using namespace std;

//extern double TEST_STOP (Car & X, const double & DT);
TEST Test;

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

    bool Key_Consume_Normal()   { return (GetAsyncKeyState('8')      & 0x0001) != 0; }  //Test czy klawisz 8 jest wcisniety dla modelu spalania normal
    bool Key_Consume_Eco()      { return (GetAsyncKeyState('9')      & 0x0001) != 0; }  //Test czy klawisz 9 jest wcisniety dla modelu spalania eco
    bool Key_Consume_Sport()    { return (GetAsyncKeyState('0')      & 0x0001) != 0; }  //Test czy klawisz 0 jest wcisniety dla modelu spalania sport

    bool Key_Gear_Up()   { return (GetAsyncKeyState('A')      & 0x0001) != 0; }  //Test czy klawisz A jest wcisniety dla zwiększenia biegu 
    bool Key_Gear_Down()   { return (GetAsyncKeyState('Z')      & 0x0001) != 0; }  //Test czy klawisz Z jest wcisniety dla zmniejszenia biegu
    bool Key_ShiftPolicy_Transmission()   { return (GetAsyncKeyState('M')      & 0x0001) != 0; }  //Test czy klawisz M jest wcisniety dla zmiany trbu manual/auto 

    bool Key_ABS_Enable()   { return (GetAsyncKeyState('B')      & 0x0001) != 0; }  //Test czy klawisz B jest wcisniety dla dostępności ABS w aucie
    bool Key_Asphalt_Road()   { return (GetAsyncKeyState('1')      & 0x0001) != 0; }  //Test czy klawisz 1 jest wcisniety dla zmiany typu drogi 
    bool Key_Gravel_Road()      { return (GetAsyncKeyState('2')      & 0x0001) != 0; }  //Test czy klawisz 2 jest wcisniety dla zmiany typu drogi 
    bool Key_Ice_Road()    { return (GetAsyncKeyState('3')      & 0x0001) != 0; }  //Test czy klawisz 3 jest wcisniety dla zmiany typu drogi 

    bool Key_Test_Reset()    { return (GetAsyncKeyState('J')      & 0x0001) != 0; }  //Test czy klawisz J jest wcisniety dla resetu testu
    bool Key_Test_ON_OFF()    { return (GetAsyncKeyState('K')      & 0x0001) != 0; }  //Test czy klawisz K jest wcisniety dla ON/OFF testu
     
    bool Key_Grade_Up()    { return (GetAsyncKeyState(VK_OEM_4)      & 0x0001) != 0; }  //Test czy klawisz [ jest wcisniety dla dodania grade
    bool Key_Grade_Down()    { return (GetAsyncKeyState(VK_OEM_6)      & 0x0001) != 0; }  //Test czy klawisz ] jest wcisniety dla odjęcia grade

//#else   //Tutaj powinna byc instrukcja dla linux
#endif

double position;
int Choice; //Zmienna z ktorej bede korzystal do wyborow uzytkownika typu tak/nie
//enum class Test_X {off, on, first_time};    //Zmienna typu wyliczeniowego do weryfikacji stanu danego testu
//Test_X Test = Test_X::first_time;   //Nazwa zmiennej jest chwilowa poniewaz nie wykorzystuje innych testow w programie. W przyszlosci moze sie zmienic

enum class Road {Normal, Water, Snow};
Road Actual_Road = Road::Normal;
string Road_Name = "Normal"; //Zamierzony zamiar trzymania drogi w main.cpp

int main ()
{
    #ifdef _WIN32
        EnableVTMode();
    #endif
    cout << "\x1b[2J\x1b[H"; // wyczyść ekran + kursor do (0,0)

    Car Audi;

    /*    cout << "Czy chcesz uruchomic test po jakiej odleglosci auto rozpedzon do 50 km/h sie zatrzyma?\n1 - Tak / 2 - Nie" << endl;
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
    */
    while (true) {
        if (Key_Quit()) break;

        //if(Test == Test_X::on) position = TEST_STOP(Audi, DT);

        /*if ((Test == Test_X::on) && (Audi.get_CarSpeed() == 0.0))
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
        }*/

        bool Thr = Key_Throttle();
        bool Brk = Key_Brake();

        //if(Test != Test_X::off) 
        Audi.Speed_Update(DT, Thr, Brk);     //Sprawdzenie czy nie dziala w tle test dla sprawdzania odleglosci

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

        //Zmiana typu drogi - Wymagane dla testu ABC TCS #NIE AKTUALNE - STARY KOD#
        /*if (Key_Normal_Road()) {
            mu = 0.9;
            Actual_Road = Road::Normal;
            Road_Name = "Normal";
        } 
        if (Key_Water_Road()) {
            mu = 0.6;
            Actual_Road = Road::Water;
            Road_Name = "Water";
        }
        if (Key_Snow_Road()) {
            mu = 0.2;
            Actual_Road = Road::Snow;
            Road_Name = "Snow";
        }*/
        if (Key_Asphalt_Road()) {Audi.get_Environment().set_Surface(std::make_unique<AsphaltSurface>());}
        if (Key_Gravel_Road()) {Audi.get_Environment().set_Surface(std::make_unique<GravelSurface>());}
        if (Key_Ice_Road()) {Audi.get_Environment().set_Surface(std::make_unique<IceSurface>());}


        //Przełącznik do uruchamiania ABS
        if (Key_ABS_Enable()) Audi.ABS_enabled_changer();

        if (Key_Test_Reset()) Test.Reset();
        if (Key_Test_ON_OFF()) Test.Test_ON_OFF();
        if (Test.get_Test_Start())Test.Start_Test_Time(Audi.get_CarSpeed());

        //Nachylenie
        if (Key_Grade_Up()) Audi.get_Environment().Add_Grade_Percent(5.0);
        if (Key_Grade_Down()) Audi.get_Environment().Add_Grade_Percent(-5.0);

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
        //Potencjalny BUG jeżeli tekstu będzie więcej niż wielkość startowego okna - trzeba uważać
        cout << "\x1b[" << 27 << "A";
        for(int i=0;i<27;i++) cout << "\x1b[2K\n";
        cout << "\x1b[" << 27 << "A";

        cout << fixed << setprecision(2);
        cout << "===CAR AND ROAD CONTROL==="
            << "\nUP = throttle, SPACE = brake, Q = quit, E = Engine ON/OFF, R - Refuel 8/9/0 - Consumption model Normal/Eco/Sport"
            << "\nA - GearUp, Z - GearDown, M - ShiftPolicy[Manual/Auto]"
            << "\n1 - Asphalt Road, 2 - Gravel Road, 3 - Ice Road, B - ABS ON/OFF"
            << "\n[ - Grade Up, ] - Grade Down, Min grade = -30/Max grade = 30"
            << "\n\n===CAR INFORMATION==="
            << "\nSpeed:" << Audi.get_CarSpeed() * 3.6 << " km/h " << "  Throttle: " << Audi.get_CatThrottle() << "  Brake= " << Audi.get_CarBrake()
            << " Engine: " << setw(4) <<(Audi.get_Engine().Engine_is_On() ? "ON" : "OFF")
            << "\nFuel: " << Audi.get_Car_FuelTank().get_FuelTank_Level() << " Consumption Fuel Model: "<< setw(7) << Audi.get_Engine().Check_Consumption()
            << "\n\nEngine Work time: " << Audi.get_Trip_Computer().get_Work_Time()
            << "\nMomentary Fuel Consumption: " << Audi.get_Trip_Computer().get_Momentary_Fuel_Consumption_100KM()
            << "\nAverage Fuel Consumption: " << Audi.get_Trip_Computer().get_Average_Fuel_Consumption()
            << "\nDistance: " << Audi.get_Trip_Computer().get_Distance()
            << "\n\nCurrent gear: " << Audi.get_Car_Transmission().get_Current_Gear()
            << "\nRPM: " << Audi.get_Car_Transmission().get_RPM()
            << "\nShiftPolicy transmission: " << setw(7) << Audi.get_Car_Transmission().Check_ShiftPolicy()
            << "\n\nABS: " << setw(4) << Audi.ABS_info() << " TCS: " << setw(4) << Audi.TCS_info();
        if (Audi.get_ABS_Enable()) cout<<"\nABS is Enable";
        else cout<<"\nABS is Unable";
        cout <<"\nActual road: " << setw(7) << Audi.get_Environment().get_surface().name() << " , Actual grade: "<< Audi.get_Environment().get_grade_Percent()
            << " , wind: " << Audi.get_Environment().get_wind();
            //<< flush;


        cout << "\n\n===TEST INFORMATION===" 
            << "\nJ - Test reset, K - Start/Stop test";

        if (Test.get_Test_Start()) cout << "\nTest START";
        else cout << "\nTest STOP";

        cout << "\nTest time: " << Test.get_Test_Time()
            << "\nTest Distance: " << Test.get_Test_Distance() << " M";

        this_thread::sleep_for(chrono::milliseconds(16));
    }
    return 0;
}
