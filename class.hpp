#include <iostream>
#include <algorithm> //Wymagane dla std::clamp(value, min, max); + std::max(Value_A, Value_B);#
#include <memory> //Wymagana do uzytego wzorca state dla stanu silnika
#include <vector>
#include "Engine_mechanics.hpp"

using namespace std;

class Car;

class FuelTank {
    private:
        double FuelTank_Level;
        double FuelTank_Capacity;

    public:
        //Konstruktor - ///Nie pozwala na wartosc baku ponizej zera oraz powyzej pojemnosci - Capacity. Mozliwe w uzyciu poniewaz wartosc C_FuelTank_Capacity jest incjalizowana przez usera lub przez wartosc domyslna
        FuelTank(double C_FuelTank_Level = 0.02, double C_FuelTank_Capacity = 120.0) 
            : FuelTank_Level (clamp(C_FuelTank_Level, 0.0, C_FuelTank_Capacity)), FuelTank_Capacity (C_FuelTank_Capacity) {}     

        //Konstruktor
        //FuelTank() : FuelTank_Level(5.0), FuelTank_Capacity(120.0) {}
        
        //Getter
        double get_FuelTank_Level() const {
            return this->FuelTank_Level;
        }

        //Setter
        void set_FuelTank_Level(double S_FuelTank_Level) {
            FuelTank_Level = clamp(S_FuelTank_Level, 0.0, FuelTank_Capacity);   //Nie pozwala na wartosc baku ponizej zera oraz powyzej pojemnosci - Capacity
        }

        //Metody
        /*bool Consume_Fuel(double Amount) {
            if (this->FuelTank_Level >= Amount) {
                this->FuelTank_Level -= Amount;
                return true;
            } 
            else return false;
        }*/

        void Refuel(int Amount) {
            FuelTank_Level = clamp(FuelTank_Level + Amount, 0.0, FuelTank_Capacity); //Nie pozwala na wartosc baku ponizej zera oraz powyzej pojemnosci - Capacity
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
    private:
        vector<double> Gears_Ratio = {4.2, 3.6, 2.9, 2.1, 1.7 }; //Biegi od 1 do 5 na potrzeby zadania 
        const double Final_drive = 3.1; //Stały mnożnik dla każdego biegu. Potrzebny do total ration i wyliczania RPM
        const double R_Wheel = 0.3; //[M] - Promień koła, dla liczenia RPM
        int Current_Gear;    
        double RPM;  //RPM ~ wheel_speed * gear_ratio
        static constexpr double Pi = 3.14159265358979323846;

    public:
        //Brakuje konstruktora ale najpierw wymagane bedzie stworzenie klas dla manual i automat ze wzorcem strategy
        //Get
        int get_Current_Gear() const {
            return this -> Current_Gear;
        }
        
        double get_RPM() const {
            return this -> RPM;
        }

        //Metody
        void Gear_up() {
            if (Current_Gear < Gears_Ratio.size()) Current_Gear++;
        }

        void Gear_down() {
            if (Current_Gear > 1) Current_Gear--;
        }

        double Total_Ratio() const {  //Całkowite przełożenie
            int Max_Index = static_cast<int>(Gears_Ratio.size()) - 1;
            int Gear_Index = clamp(Current_Gear - 1, 0, Max_Index); 
            return Gears_Ratio[Gear_Index] * Final_drive;
        }

        void Count_RPM(const Car& T_Car) {
            RPM = ( (T_Car.get_CarSpeed() / R_Wheel) * Total_Ratio() ) * 60.0 / (2.0 * Pi);
            /*
            RPM - ilość obrotów w ciągu minuty
            RPM = ( wheel_speed [V / R] * total_ratio [gear * final drive] ) * 60 / (2 * Pi)]
            [rad/x] * [ile obrótów zrobi silnik w momencie gdy koło robi 1 obrót];

            [rad/s]
            V - prędkość
            R - Promień koła <-- na potrzeby zadania będzie to stała wartość
            */
        }
};

class Engine {
    enum class ConsumptionStatus { Eco, Normal, Sport, Off};
    private:
        FuelTank& FuelTank_Ref;
        TripComputer& TripComputer_Ref;
        unique_ptr<EngineState> State;
        unique_ptr<ConsumptionModel> Consumption_Fuel_Model;
        ConsumptionStatus Consumption_Status;

    public:
        //Konstruktor
        Engine( FuelTank& C_FuelTank_Ref, TripComputer& C_TripComputer,ConsumptionStatus C_Consumption_Beginning = ConsumptionStatus::Off)
            : FuelTank_Ref(C_FuelTank_Ref), TripComputer_Ref(C_TripComputer),State(make_unique<Engine_off>()), Consumption_Fuel_Model(make_unique<ConsumptionModel_Normal>()), Consumption_Status(C_Consumption_Beginning) {}

        //Getter
        bool Engine_is_On() const {
            return State->Check_Engine_On();
        }
        string Check_Consumption() {
            switch (Consumption_Status) {
            case ConsumptionStatus::Off:
                return "Off";
                break;
            case ConsumptionStatus::Normal:
                return "Normal";
                break;
            case ConsumptionStatus::Eco:
                return "Eco";
                break;
            case ConsumptionStatus::Sport:
                return "Sport";
                break;
            }
            return "Error";
        }

        //Setter
        void set_Engine_On() {
            State = make_unique<Engine_on>();
            set_Consumption_Normal(); //Kazde uruchomienie silnika ma ustawiac spalanie na normal
            Consumption_Status = ConsumptionStatus::Normal;
        }
        void set_Engine_Off() {
            State = make_unique<Engine_off>();
            Consumption_Status = ConsumptionStatus::Off;
        }
        void set_Consumption_Normal() {
            Consumption_Fuel_Model = make_unique<ConsumptionModel_Normal>();
            Consumption_Status = ConsumptionStatus::Normal;
        }
        void set_Consumption_Eco() {
            Consumption_Fuel_Model = make_unique<ConsumptionModel_Eco>();
            Consumption_Status = ConsumptionStatus::Eco;
        }
        void set_Consumption_Sport() {
            Consumption_Fuel_Model = make_unique<ConsumptionModel_Sport>();
            Consumption_Status = ConsumptionStatus::Sport;
        }
        //Metody
        /*void Check_Consumption() {
            switch (Consumption_Status) {
            case ConsumptionStatus::Off:
                cout<<"Off"<<endl;
                break;
            case ConsumptionStatus::Normal:
                cout<<"Normal"<<endl;
                break;
            case ConsumptionStatus::Eco:
                cout<<"Eco"<<endl;
                break;
            case ConsumptionStatus::Sport:
                cout<<"Sport"<<endl;
                break;
            }
        }*/

        double Real_Throttle(double Car_Throttle) {
            return State->Real_Throttle(Car_Throttle);
        }

        void Consume_Fuel(double CarThrottle, double CarSpeed, double DT) {
            if (Engine_is_On()) {
                double Requirement_Fuel = Consumption_Fuel_Model->Fuel_Flow_Lps(CarThrottle, CarSpeed);
                double New_Fuel_Level = FuelTank_Ref.get_FuelTank_Level() - Requirement_Fuel * DT;

                TripComputer_Ref.Update(CarSpeed, Requirement_Fuel * DT);
                FuelTank_Ref.set_FuelTank_Level(max(0.0, New_Fuel_Level)); //Nie pozwala na wartosci ponizej zera - wybiera wieksza wartosc
            }
            if (Engine_is_On() && (FuelTank_Ref.get_FuelTank_Level() <= 0)) set_Engine_Off(); //Pusty bank wylacza silnik
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
        /*void Car_Information() {
            cout<<"<--------Informacje o aucie-------->"<<endl;
            //Silnik
            if(Engine_Ref.get_Engine_on_off()) cout<<"Silnik jest uruchomiony"<<endl;
            else cout<<"Silnik jest wylaczony"<<endl;
            //Paliwo
            cout<<"W baku jest: "<<FuelTank_Ref.get_FuelTank_Level()<<"L paliwa"<<endl;
            //Predkosc
            /*Brakuje metody w Car*/
        //}
};

class Car {
    private:
        TripComputer Car_TripComputer;
        FuelTank Car_FuelTank;
        Brake Car_Brake;
        Engine Car_Engine;
        //Transmission Car_Transmission;
        Dashboard Car_Dashboard;


        const double MAX_SPEED_MS = 50.0; //[M/S] -> 50 m/s * 3.6 = 180 km/h. Mnozenie przez 3.6 w celu uzyskania km/h
        const int MASS_KG = 1000;
        const double F_Eng_MAX = 4000.0;    //Sila w [N] na potrzebny nowej mechaniki  sumy wszystkich sil
        const double F_Brake_MAX = 6500.0;      //Sila w [N] na potrzebny nowej mechaniki  sumy wszystkich sil

        double CarSpeed = 0.0;  //[M/S]
        double CarThrottle = 0.0;
        double CarBrake = 0.0;  //Zapisanie zmiennych double CarNazwa z powodu problemu nazw double Brake i class Brake 
        double Slew = 3; //O ile jednostek na sekundę moze zostac dokonana zmiana wartosci. W tym przypadku 3 jednostki na sekunde

        double c_roll = 75.0;   //Opor toczenia  [N]
        double c_drag = 0.5;    //Opor powietrza [N / (m/s)^2]
        
        //Ogranicznik szybkosci zmian wartosci
        static double Rate_Limiter(double Current_Value, double Target_Value, double Max_Value_For_Rate, double DT) { //DT - Krok czasu, Max_Value_For_Tate = maksymalna wartosc/predkosc na sekunde/klatke
            double Target = Target_Value - Current_Value; //Ile brakuje do celu - do wartosci max dla np. throttle - Przedział od 0.0 do 1.0
            double MAX_One_DT = Max_Value_For_Rate * DT; //Maksymalny krok jaki może być wykonany na 1 krok czasu   
            if (Target > MAX_One_DT) Current_Value += MAX_One_DT;   //Gdy wartosc do celu jest wieksza od max wartosci na klatke np. do max predkosci brakuje 4 km/h, max przyspieszenie na klatke 3km/h 
            else if (Target < -MAX_One_DT) Current_Value -= MAX_One_DT;  //Gdy wartosc do celu jest mniejsza od max wartosci na klatke np. do max predkosci brakuje 2 km/h, max przyszpieczenie na klatke 3km/h
            else Current_Value = Target_Value;
            return clamp(Current_Value, 0.0, 1.0);  //Przycina wartosci do przedzialu [0.0, 1.0] 
        } 

    public:
        /*//Konstruktor 
        Car(FuelTank& C_Car_FuelTank, Brake& C_Car_Brake, Engine& C_Car_Engine, Transmission Car_Transmission, Dashboard& C_Car_Dashboard) 
            :  Car_FuelTank (C_Car_FuelTank), Car_Brake (C_Car_Brake), Car_Engine (C_Car_Engine, C_Car_FuelTank), Car_Transmission (Car_Transmission), Car_Dashboard (C_Car_Dashboard, C_Car_FuelTank, C_Car_Engine) {}
        */
        //Konstruktor domyslny
        Car() 
            : Car_TripComputer(),
              Car_FuelTank (), 
              Car_Brake (), 
              Car_Engine (Car_FuelTank, Car_TripComputer), 
              /*Car_Transmission (),*/ 
              Car_Dashboard (Car_FuelTank, Car_Engine) {}

        //Gettery klas
        const Engine& get_Engine() const {
            return Car_Engine;
        }
        Engine& get_Engine() {  //Przeciazenie do modyfikacji przez getter
            return Car_Engine;
        }
        const FuelTank& get_Car_FuelTank() const {
            return Car_FuelTank;
        }
        FuelTank& get_Car_FuelTank() {   //Przeciazenie do modyfikacji przez getter
            return Car_FuelTank;
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
        const TripComputer& get_Trip_Computer() const {
            return Car_TripComputer;
        }

        //Gettery zmiennych
        double get_CarSpeed() const {   //Zwracana jednostka to [m/s], w celu uzyskania [km/h] wymagane mnozenie przez 3.6
            return this->CarSpeed;
        }
        double get_CatThrottle() const {
            return this->CarThrottle;
        }
        double get_CarBrake() const {
            return this->CarBrake;
        }

        //Settery klas
        void set_FuelTank(FuelTank S_FuelTank) {
            Car_FuelTank = S_FuelTank;
        }       
        void set_Brake(Brake S_Brake) {
            Car_Brake = S_Brake;
        }        
        /*void set_Transmission(Transmission S_Transmission) {
            Car_Transmission = S_Transmission;
        }*/   
        
        //Settery zmiennych
        void set_CarBrake(double S_CarBrake) {
            CarBrake = S_CarBrake;
        }
        void set_CarSpeed(double S_CarSpeed) {
            CarSpeed = S_CarSpeed;
        }
       
        //Metody
        void Speed_Update (double DT, bool Click_Throttle, bool Click_Brake) {
            //Click_Throttle = false;   TEST DLA NOWEJ MECHANIKI SPRAWDANIA STANU SILNIKA
            //CarThrottle = Rate_Limiter(CarThrottle, Click_Throttle ? 1.0:0.0, Slew ,DT);
            CarThrottle = Car_Engine.Real_Throttle(Rate_Limiter(CarThrottle, Click_Throttle ? 1.0:0.0, Slew ,DT)); //Gdy engine off - throttle igonorowany
            CarBrake = Rate_Limiter(CarBrake, Click_Brake ? 1.0:0.0, Slew ,DT);


            //Mechanika przyspieszenia:  Przyspieszenie = Throttle - Brake
            double Acceleration = 0.0;

            double F_Eng = 0.0;     //[N]
            double F_Brake = 0.0;   //[N]
            double F_Drag = c_drag * (CarSpeed * CarSpeed); //[N] 
            double F_Roll = c_roll;    //[N]

            F_Eng = CarThrottle * F_Eng_MAX;  //Tworzymy moc napedu
            F_Brake = CarBrake * F_Brake_MAX;   //Tworzymy moc hamulca
            
            Acceleration = ((F_Eng - F_Brake - F_Drag - F_Roll) / MASS_KG); //[m/s^2]

            /* Poprzednia mechanika zmiany predkosci
            CarSpeed = max(0.0, CarSpeed + (Acceleration * DT));    //Funkcja wybierajaca wieksza wartosc. Zabezpieczenie przed ujemna predkoscia
            if (CarSpeed >= MAX_SPEED) CarSpeed = MAX_SPEED;    //Ogranicznik maksymalnej predkosci */

            CarSpeed = clamp( (CarSpeed + Acceleration * DT), 0.0, MAX_SPEED_MS);  //Ogranicznik spelniajacy zalozenia: 0 <= V <= Vmax. [m/s]
            Car_Engine.Consume_Fuel(get_CatThrottle(), get_CarSpeed(), DT); //Spalanie paliwa na koniec po okresleniu nowej predkosci
        }
}; 