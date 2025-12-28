#include <iostream>
#include <algorithm> //Wymagane dla std::clamp(value, min, max); + std::max(Value_A, Value_B);#
#include <memory> //Wymagana do uzytego wzorca state dla stanu silnika

using namespace std;

class EngineState;
class Engine_off;
class Engine_on;

class FuelTank {
    private:
        double FuelTank_Level;
        double FuelTank_Capacity;

    public:
        //Konstruktor
        FuelTank(double C_FuelTank_Level, double C_FuelTank_Capacity) 
            : FuelTank_Level (C_FuelTank_Level), FuelTank_Capacity (C_FuelTank_Capacity) {}

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

        void Refuel(int Amount) {
            if (this->FuelTank_Level + Amount >= FuelTank_Capacity) return;
            else this ->FuelTank_Level += Amount;
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
    enum class ConsumptionStatus { Eco, Normal, Sport, Off};
    private:
        FuelTank& FuelTank_Ref;
        unique_ptr<EngineState> State;
        unique_ptr<ConsumptionModel> Consumption_Fuel;
        ConsumptionStatus Consumption_Status;

    public:
        //Konstruktor
        Engine( FuelTank& C_FuelTank_Ref, ConsumptionStatus C_Consumption_Beginning = ConsumptionStatus::Off)
            : FuelTank_Ref(C_FuelTank_Ref), State(make_unique<Engine_off>()), Consumption_Fuel(make_unique<ConsumptionModel_Normal>()), Consumption_Status(C_Consumption_Beginning) {}

        //Getter
        bool Engine_is_On() const {
            return State->Check_Engine_On();
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
            Consumption_Fuel = make_unique<ConsumptionModel_Normal>();
            Consumption_Status = ConsumptionStatus::Normal;
        }
        void set_Consumption_Eco() {
            Consumption_Fuel = make_unique<ConsumptionModel_Eco>();
            Consumption_Status = ConsumptionStatus::Eco;
        }
        void set_Consumption_Sport() {
            Consumption_Fuel = make_unique<ConsumptionModel_Sport>();
            Consumption_Status = ConsumptionStatus::Sport;
        }
        //Metody
        void Check_Consumption(ConsumptionStatus Consumption_Status) {
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
            : Car_FuelTank (), 
              Car_Brake (), 
              Car_Engine (Car_FuelTank), 
              /*Car_Transmission (),*/ 
              Car_Dashboard (Car_FuelTank, Car_Engine) {}

        //Gettery klas
        const Engine& get_Engine() const {
            return Car_Engine;
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
            CarThrottle = Rate_Limiter(CarThrottle, Click_Throttle ? 1.0:0.0, Slew ,DT);
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
        }
}; 

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
            return 0.7 * CarThrottle * CarSpeed;
        }
};

class ConsumptionModel_Normal : public ConsumptionModel {
    public:
        double Fuel_Flow_Lps (double CarThrottle, double CarSpeed) override {   //Nadpisywanie wirtualnej metody z klasy bazowej [Fuel_Flow_Lps]
            return 1 * CarThrottle * CarSpeed;
        }
};

class ConsumptionModel_Sport : public ConsumptionModel {
    public:
        double Fuel_Flow_Lps (double CarThrottle, double CarSpeed) override {   //Nadpisywanie wirtualnej metody z klasy bazowej [Fuel_Flow_Lps]
            return 1.5 * CarThrottle * CarSpeed;
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