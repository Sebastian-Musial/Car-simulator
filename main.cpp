#include <iostream>
#include <string>

using namespace std;

class Engine {

};

class FuelTank {

};

class Brake {

};

class Transmission {

}

class Dashboard {

}

class Car {
    private:
        Engine Car_Engine;
        FuelTank Car_FuelTank;
        Brake Car_Brake;
        //Transmission Car_Transmission;
        Dashboard Car_Dashboard;

    public:
        //Konstruktor 
        Car(Engine C_Car_Engine, FuelTank C_Car_FuelTank, Brake C_Car_Brake, /*Transmission Car_Transmission*/, Dashboard C_Car_Dashboard) 
            : Car_Engine (C_Car_Engine), Car_FuelTank (C_Car_FuelTank), Car_Brake (C_Car_Brake), /*Car_Transmission (Car_Transmission)*/, Car_Dashboard (C_Car_Dashboard) {}

        //Konstruktor domyslny
        Car() 
            : Car_Engine (), 
              Car_FuelTank (), 
              Car_Brake (), 
              /*Car_Transmission ()*/, 
              Car_Dashboard () {}

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
        void set_Engine(Engine S_Engine) {
            Car_Engine = S_Engine;
        }
        void set_FuelTank(FuelTank S_FuelTank) {
            Car_FuelTank = S_FuelTank;
        }       
        void set_Brake(Break S_Brake) {
            Car_Brake = S_Brake;
        }        
        /*void set_Transmission(Transmission S_Transmission) {
            Car_Transmission = S_Transmission;
        }*/        
        void set_Dashborad(Dashboard S_Dashboard) {
            Car_Dashboard = S_Dashboard;
        }
};

const MAX_SPEED_KMH;

const MASS_KG;

const DT;

//pętla update(dt) 

int main ()
{

}