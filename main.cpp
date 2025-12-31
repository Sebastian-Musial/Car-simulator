#include <iostream>
#include <string>
#include <chrono>   //Biblioteka do obsługi czasu  potrzebne dla pomiaru czasu i jednostek czasu 
#include <thread>   //Biblioteka do obslugi wielowatkowosci i zarzadzania czasem w aplikacji - zarządza wątkami np. w sleep_for aby uspic petle
#include <cstdio>   //Biblioteka dla wypisywania danych np, printf
#include <windows.h>    //Biblioteka windows API - potrzebna do skorzystania z GetAsyncKeyState
#include "class.hpp"

using namespace std;

extern double TEST_STOP (Car & X, const double & DT);

const double DT = 0.02; //Jest to liczba przykladowa i wymaga testowania
//pętla update(dt) 

//Instrukcje preprocesora dla kompilacji dla windowsa oraz nie windowsa np. linux
#ifdef _WIN32   //Windows
    //Stan bitu 15 sprawdza czy klawisz jest teraz wcisniety - sprawdza czy klawisz jest trzymany. Dla samego wcisnięcia sprawdzany jest bit 0 - (0x0001)
    bool Key_Throttle() { return (GetAsyncKeyState(VK_UP)    & 0x8000) != 0; }  //Test czy klawisz sztalki w gore jest wcisniety dla throttle
    bool Key_Brake()    { return (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0; }  //Test czy klawisz spacji jest wcisniety dla brake
    bool Key_Quit()     { return (GetAsyncKeyState('Q')      & 0x8000) != 0; }  //Test czy klawisz Q jest wcisniety dla Quit
    bool Key_Engine()   { return (GetAsyncKeyState('E')      & 0x0001) != 0; }  //Test czy klawisz E jest wcisniety dla Engine
    bool Key_Refuel()   { return (GetAsyncKeyState('R')      & 0x0001) != 0; }  //Test czy klawisz R jest wcisniety dla Engine

//#else   //Tutaj powinna byc instrukcja dla linux
#endif

double position;
int Choice; //Zmienna z ktorej bede korzystal do wyborow uzytkownika typu tak/nie
enum class Test_X {off, on, first_time};    //Zmienna typu wyliczeniowego do weryfikacji stanu danego testu
Test_X Test = Test_X::first_time;   //Nazwa zmiennej jest chwilowa poniewaz nie wykorzystuje innych testow w programie. W przyszlosci moze sie zmienic

int main ()
{
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

        if ((Test == Test_X::on) & (Audi.get_CarSpeed() == 0.0))
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
            cout<<"UP = throttle, SPACE = brake, Q = quit, E = Engine ON/OFF"<<endl;
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

        //Wypisanie tekstu z informacjami w czasie rzeczywistym
        printf("\rspeed=%6.2f km/h   throttle=%.2f   brake=%.2f   fuel=%.2f L   engine=%s   ",
                    Audi.get_CarSpeed() * 3.6 , Audi.get_CatThrottle(), Audi.get_CarBrake(),    //Wystepuje tutaj mnozenie przez 3.6 w celu zamiany jednostki m/s na km/h
                    Audi.get_Car_FuelTank().get_FuelTank_Level(),
                    Audi.get_Engine().Engine_is_On() ? "ON" : "OFF");
        fflush(stdout);

        this_thread::sleep_for(chrono::milliseconds(16));
    }
    return 0;
}
