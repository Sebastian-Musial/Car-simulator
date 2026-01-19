#include <memory> //Wymagana do uzytego wzorca state dla stanu silnika

#pragma once
extern const double DT;

using namespace std;

/*==============================*/
/*Model strategy dla modelu spalania paliwa w trybie eco, normal i sport*/
class ConsumptionModel {
    public:
    virtual double Fuel_Flow_Lps (double CarThrottle, double CarSpeed) = 0; //CarSpeed - M/S. Metoda ta jest wirtualna i nalezy do niej zdefiniowac implementacje w klasach ktore beda ja dziedziczyc
    virtual ~ConsumptionModel() = default; //Automatyczny Destruktor - dziala na na pochodnej klasie (dzieki virtual) a nastepnie na bazowej np. ConsumptionModel_Eco -> ConsumptionModel
};

class ConsumptionModel_Eco : public ConsumptionModel {
    public:
        double Fuel_Flow_Lps (double CarThrottle, double CarSpeed) override {   //Nadpisywanie wirtualnej metody z klasy bazowej [Fuel_Flow_Lps]
            return 0.00013 * CarThrottle * CarSpeed;    //13 L
        }
};

class ConsumptionModel_Normal : public ConsumptionModel {
    public:
        double Fuel_Flow_Lps (double CarThrottle, double CarSpeed) override {   //Nadpisywanie wirtualnej metody z klasy bazowej [Fuel_Flow_Lps]
            return 0.00020 * CarThrottle * CarSpeed;    //20 L
        }
};

class ConsumptionModel_Sport : public ConsumptionModel {
    public:
        double Fuel_Flow_Lps (double CarThrottle, double CarSpeed) override {   //Nadpisywanie wirtualnej metody z klasy bazowej [Fuel_Flow_Lps]
            return 0.00030 * CarThrottle * CarSpeed;    //30 L
        }
};

/*==============================*/
/*Model state dla silnika on/off */
class EngineState {
    public:
        virtual double Real_Throttle (double R_Throttle) = 0;
        virtual bool Check_Engine_On () = 0;
        virtual ~EngineState() = default; //Destruktor 
};

class Engine_on : public EngineState {      //Throttle nie ingonowany
    public:
        double Real_Throttle( double R_Throttle) override {
            return R_Throttle;
        }

        bool Check_Engine_On () override {
            return true;
        };
};

class Engine_off : public EngineState {     //Throttle ingonowany
    public:
        double Real_Throttle( double R_Throttle) override {
            return 0.0;
        }

        bool Check_Engine_On () override {
            return false;
        };
};

/*==============================*/
/*Model observer dla tripComputer na silniku */
class TripComputer {
    private:
        double Momentary_Fuel_Consumption_100KM = 0.0;
        double Average_Fuel_Consumption = 0.0;
        double Distance = 0.0;
        double Work_Time = 0.0; //Liczy sekundy
        double Consume_Fuel = 0.0;
        double DT_Work_Time = 0.0;  //Liczy kroki czasu/FPS

    public:
        //Konstruktor domyslny
        TripComputer() :
            Momentary_Fuel_Consumption_100KM(),
            Average_Fuel_Consumption(),
            Distance(),
            Work_Time(),
            Consume_Fuel(),
            DT_Work_Time() {}

        //Gettery
        const double get_Average_Fuel_Consumption() const {
            return Average_Fuel_Consumption;
        }

        const double get_Momentary_Fuel_Consumption_100KM() const {
            return Momentary_Fuel_Consumption_100KM;
        }

        const double get_Distance() const {
            return Distance;
        }

        const double get_Work_Time() const {
            return Work_Time;
        }

        //Settery
        void set_Average_Fuel_Consumption(double S_Average_Fuel_Consumption) {
            Average_Fuel_Consumption = S_Average_Fuel_Consumption;
        }

        void set_Distance(double S_Distance) {
            Distance = S_Distance;
        }

        void set_Work_Time(double S_Work_Time) {
            Work_Time = S_Work_Time;
        }

        //Metody
        void Count_Distance (double Car_Speed) {
            Distance += Car_Speed * DT;
        }

        void Count_Fuel_Consumption(double DT_C_Fuel, double Car_Speed) {
            Consume_Fuel += DT_C_Fuel;
            double DT_Distance = Car_Speed * DT;
            if (Distance > 0.0) Average_Fuel_Consumption = Consume_Fuel / Distance;  //Spalanie srednie
            else Average_Fuel_Consumption = 0.0;
            if ((DT_C_Fuel > 0.0) && (DT_Distance > 0.0)) //Zalozenie liczenia wymaga przeplywu paliwa. Predkosc samochodu moze byc utrzymywana przez pewien czas bez spalania samochodu.
                Momentary_Fuel_Consumption_100KM = (DT_C_Fuel / DT_Distance) * 100000;    //Spalanie na 100KM
            else Momentary_Fuel_Consumption_100KM = 0.0; //Nie ma spalania - nie ma chwilowego spalania na 100km. Zalozenie: w takim przypadku ma pokazywac chwilowe na 0
        }

        void Count_Work_Time() {
                Work_Time += DT;
        }

        void Update(double Car_Speed, double DT_C_Fuel) {
            Count_Distance(Car_Speed);
            Count_Fuel_Consumption(DT_C_Fuel, Car_Speed);
            Count_Work_Time();
        }
};

