#include <iostream>
#include <string>

using namespace std;

class FuelTank {
    private:
        double FuelTank_Level;

    public:
        //Konstruktor
        FuelTank(double C_FuelTank_Level) 
            : FuelTank_Level (C_FuelTank_Level) {}

        //Konstruktor domyslny
        FuelTank() { FuelTank_Level = 5.0; }
        
        //Getter
        double get_FuelTank_Level() const {
            return this->FuelTank_Level;
        }

        //Setter
        void set_FuelTank_Level(double S_FuelTank_Level) {
            FuelTank_Level =S_FuelTank_Level;
        }

        //Metody
        bool Consume_Fuel(double Amount) {
            if (this->FuelTank_Level >= Amount) {
                this->FuelTank_Level -= Amount;
                return true;
            } 
            else return false;
        }
};

class Brake {
    private:
        int Brake_Power;

    public:
        //Konstruktor
        Brake(int C_Brake_Power)
            : Brake_Power(C_Brake_Power) {}

        //Konstruktor domyslny
        Brake() { Brake_Power = 30; }

        //Getter
        int get_Brake_Power() const {
            return this->Brake_Power;
        }

        //Setter
        void set_Brake_Power(int S_Brake_Power) {
            Brake_Power = S_Brake_Power;
        }

        //Metody
        /*Potrzebna metoda hamowania ale brakuje nam predkosci do ktorej metoda moze sie odniesc*/
        /*
        void Stop(int Speed) {
            while (Speed !=0)
            {
                Speed -= Brake_Power; //Uproszczona wersja - brak walidacji co sie stanie jak brake power bedzie wieksze od speed = wartosc ujemna
            }
        }
        */
};

class Transmission {

};

class Engine {
    private:
        FuelTank& FuelTank_Ref;
        double Start_Engine_Consumption;
        double Ride_Consumption;
        bool Engine_on_off;


    public:
        //Konstruktor
        Engine( FuelTank& C_FuelTank_Ref, double C_Start_Engine_Consumption = 0.2, double C_Ride_Consumption = 0.1, bool C_Engine_on_off = false)
            : FuelTank_Ref(C_FuelTank_Ref), Start_Engine_Consumption(C_Start_Engine_Consumption), Ride_Consumption(C_Ride_Consumption), Engine_on_off(C_Engine_on_off) {}

        //Getter
        double get_Start_Consumption() const {
            return this->Start_Engine_Consumption;
        }
        double get_Ride_Consumption() const {
            return this->Ride_Consumption;
        }
        bool get_Engine_on_off() const {
            return this->Engine_on_off;
        }

        //Setter
        void set_Start_Consumption(double S_Start_Consumption) {
            Start_Engine_Consumption = S_Start_Consumption;
        }
        void set_Ride_Consumption(double S_Ride_Consumption) {
            Ride_Consumption = S_Ride_Consumption;
        }
        void set_Engine_on_off(bool S_Engine_on_off) {
            Engine_on_off = S_Engine_on_off;
        }

        //Metody
        void Start_Engine () {  //Chwilowo void - pozniej prawdopodobnie zmienie na bool
            //Start silnika wykorzystuje paliwo z baku            
            if (FuelTank_Ref.Consume_Fuel(Start_Engine_Consumption)) {
                set_Engine_on_off(true);
                cout<<"Silnik zostal uruchomiony"<<endl;
            }
            else cout<<"Nie udalo sie uruchomic silnika. Byc moze brakuje paliwa"<<endl;
        }
        bool Ride () {
            //Jazda wykorzystuje paliwo
            if (Engine_on_off == true && FuelTank_Ref.Consume_Fuel(Ride_Consumption) ) return true; //Najpierw sprawdzamy czy silnik jest uruchomiony, jezeli tak to sprawdzamy drugi warunek logiczny
            else return false;
        }
};

class Dashboard {
    private:
        FuelTank& FuelTank_Ref;
        Engine& Engine_Ref;

    public:
        //Konstruktor
        Dashboard(FuelTank& C_FuelTank_Ref, Engine& C_Engine_Ref)
            : FuelTank_Ref(C_FuelTank_Ref), Engine_Ref(C_Engine_Ref) {}

        //Metody
        void Car_Information() {
            cout<<"<--------Informacje o aucie-------->"<<endl;
            //Silnik
            if(Engine_Ref.get_Engine_on_off()) cout<<"Silnik jest uruchomiony"<<endl;
            else cout<<"Silnik jest wylaczony"<<endl;
            //Paliwo
            cout<<"W baku jest: "<<FuelTank_Ref.get_FuelTank_Level()<<"L paliwa"<<endl;
            //Predkosc
            /*Brakuje metody w Car*/
        }
};

class Car {
    private:
        FuelTank Car_FuelTank;
        Brake Car_Brake;
        Engine Car_Engine;
        //Transmission Car_Transmission;
        Dashboard Car_Dashboard;

    public:
        /*//Konstruktor 
        Car(FuelTank& C_Car_FuelTank, Brake& C_Car_Brake, Engine& C_Car_Engine, Transmission Car_Transmission, Dashboard& C_Car_Dashboard) 
            :  Car_FuelTank (C_Car_FuelTank), Car_Brake (C_Car_Brake), Car_Engine (C_Car_Engine, C_Car_FuelTank), Car_Transmission (Car_Transmission), Car_Dashboard (C_Car_Dashboard, C_Car_FuelTank, C_Car_Engine) {}
        */
        //Konstruktor domyslny
        Car() 
            : Car_FuelTank (), 
              Car_Brake (), 
              Car_Engine (Car_FuelTank), 
              /*Car_Transmission (),*/ 
              Car_Dashboard (Car_FuelTank, Car_Engine) {}

        //Gettery
        Engine get_Engine() const {
            return this->Car_Engine;
        }
        FuelTank get_Car_FuelTank() const {
            return this->Car_FuelTank;
        }      
        Brake get_Car_Brake() const {
            return this->Car_Brake;
        }      
        /*Transmission get_Car_Transmission() const {
            return this->get_Car_Transmission;
        }*/      
        Dashboard get_Car_Dashboard() const {
            return this->Car_Dashboard;
        }

        //Settery
        void set_FuelTank(FuelTank S_FuelTank) {
            Car_FuelTank = S_FuelTank;
        }       
        void set_Brake(Brake S_Brake) {
            Car_Brake = S_Brake;
        }        
        /*void set_Transmission(Transmission S_Transmission) {
            Car_Transmission = S_Transmission;
        }*/        
};

const int MAX_SPEED_KMH = 200;

const int MASS_KG = 3000;

const double DT = 0.02; //Jest to liczba przykladowa i wymaga testowania

//pętla update(dt) 

int main ()
{

}