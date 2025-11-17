#include <iostream>
#include <string>
#include <chrono>   //Biblioteka do obsługi czasu  potrzebne dla pomiaru czasu i jednostek czasu 
#include <thread>   //Biblioteka do obslugi wielowatkowosci i zarzadzania czasem w aplikacji - zarządza wątkami np. w sleep_for aby uspic petle
#include <cstdio>   //Biblioteka dla wypisywania danych np, printf
#include <windows.h>    //Biblioteka windows API - potrzebna do skorzystania z GetAsyncKeyState
#include "class.hpp"

using namespace std;

const double DT = 0.02; //Jest to liczba przykladowa i wymaga testowania
//pętla update(dt) 

//Instrukcje preprocesora dla kompilacji dla windowsa oraz nie windowsa np. linux
#ifdef _WIN32   //Windows
    //Stan bitu 15 sprawdza czy klawisz jest teraz wcisniety - sprawdza czy klawisz jest trzymany. Dla samego wcisnięcia sprawdzany jest bit 0 - (0x0001)
    bool Key_Throttle() { return (GetAsyncKeyState(VK_UP)    & 0x8000) != 0; }  //Test czy klawisz sztalki w gore jest wcisniety dla throttle
    bool Key_Brake()    { return (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0; }  //Test czy klawisz spacji jest wcisniety dla brake
    bool Key_Quit()     { return (GetAsyncKeyState('Q')      & 0x8000) != 0; }  //Test czy klawisz Q jest wcisniety dla Quit
    //Proces dzialania
    /*
    Funkcja: SHORT GetAsyncKeyState(int vKey);
        Pozwala na zwracanie wartosci bitowych stanu danego klawisza i sprawdzenie go z bitami odpowiadającymi za wcisniecie klawisza
        A dokladnie
        Funkcja GetAsyncKeyState(int vKey) zwraca wartość 16-bitową,
        w której poszczególne bity informują o stanie klawisza —
        pozwala sprawdzić, czy dany klawisz jest aktualnie wciśnięty (bit 15)
        lub czy został naciśnięty od ostatniego wywołania (bit 0).
    */
//#else   //Tutaj powinna byc instrukcja dla linux
#endif


int main ()
{
    cout<<"UP = throttle, SPACE = brake, Q = quit"<<endl;

    Car Audi;

    while (true) {
        if (Key_Quit()) break;

        bool Thr = Key_Throttle();
        bool Brk = Key_Brake();

        Audi.Speed_Update(DT, Thr, Brk);

        //Wypisanie tekstu z informacjami w czasie rzeczywistym
        printf("\rspeed=%6.2f km/h   throttle=%.2f   brake=%.2f   fuel=%.2f L   engine=%s   ",
                    Audi.get_CarSpeed(), Audi.get_CatThrottle(), Audi.get_CarBrake(),
                    Audi.get_Car_FuelTank().get_FuelTank_Level(),
                    Audi.get_Engine().get_Engine_on_off() ? "ON" : "OFF");
        fflush(stdout);
        //Dzialanie
        /*
        printf jest buforem danych ktory wypisuje caly czas ta sama linie tekstu
        Znak (\r) powoduje cofanie sie kursora na poczatek tej samej liunii
        Skorzystanie z tych 2 opcji pozwala nadpisywac tekst i symulowac aktualizacje danych

        fflush(stdout) wymusza oproznienie bufora i pokazanie aktualnej zawartosci z printf natychmiast
        to znaczy nowo przygotowany tekst

        W srodku mamy zawartosc tekstowa z formatowaniem tekstu oraz gettery klasy Car
        w celu przekazania informacji o aktualnym statusie auta
        */

        this_thread::sleep_for(chrono::milliseconds(16));
        //Dzialanie
        /*
        this_thread - funcja obslugujaca aktualny watek programu
        sleep_for - usypia/zatrzymuje dzialanie biezace watku na okreslony czat, w tym przypadku 16 millisekund
        chrono::milliseconds(16) - chrono obsluguje czas w programie a zapis miliseconds(16) okresla obiekt czasu - 16 milisekund

        calosc dzialania powoduje zatrzymanie watku programu - dzialania programu na 16 milisekund
        czyli petla ma przerwy co 16 ms
        uzyte jest to w celu bardziej rzeczywistej symulacji dzialnia programu - w przyblizeniu 60 razy na sekunde czyli 60 FPS  
        */
    }
    return 0;
}
