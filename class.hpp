#pragma once

#include <iostream>
#include <algorithm> //Wymagane dla std::clamp(value, min, max); + std::max(Value_A, Value_B);#
#include <memory> //Wymagana do uzytego wzorca state dla stanu silnika
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include "Engine_mechanics.hpp"
#include "ShiftPolicy_Transmission.hpp"
#include "Physics.hpp"


using namespace std;

class Car;

//Wzorzec OBSERVER
struct CarState
{
    double speedKmh = 0.0;
    double throttle = 0.0;
    double brake = 0.0;

    double fuelL = 0.0;
    double fuelCapL = 1.0;

    bool engineOn = false;
    string consumptionMode; // Normal/Eco/Sport/Off

    int gear = 0;
    double rpm = 0.0;
    string shiftPolicy;

    bool absEnable = false;
    bool absActive = false;
    bool tcsActive = false;

    string road;
    double gradePct = 0.0;
    double wind = 0.0;

    //Trip PC
    double workTime = 0.0;
    double consMoment = 0.0;
    double consAvg = 0.0;
    double distance = 0.0;
};

class ICarObserver
{
    public:
        virtual ~ICarObserver() = default;
        virtual void State_Changed(const CarState& New) = 0;
};

//Klasy samochodu
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
        double get_FuelTank_Capacity() const {
            return this->FuelTank_Capacity;
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
        //int Brake_Power;
        //ABS
        double abs_Timer = 0.0;          //[s]
        double abs_ReleaseK = 0.6;       //Ile limitu przyczepności wykorzystuje w momencie odpuszczania hamowania aby koło się nie blokowało - 60%
        double abs_Period = 0.06;        //[s] okres pulsacji (60 ms)
        double abs_DutyRelease = 0.5;    //Ustawienie jaka część cyklu jest przekazywana na odpuszczenie - w tym przypadku jest to 50% czyli połowa cyklu odpuszcza a połowa dociska hamulec 

        //TCS
        double tcsK = 0.95;             //Ile limitu przyczepności wykorzystuje w momencie ruszania - limit przyczepności jaki jest przekazywany do napędu to 95% limitu

        Environment& env;

    public:
        //Konstruktor
        Brake(Environment& C_env)
            : env(C_env) {}
        //Konstruktor domyslny
        /*Brake() { Brake_Power = 30; }*/

        //Getter
        /*int get_Brake_Power() const {
            return this->Brake_Power;
        }*/

        //Setter
        /*void set_Brake_Power(int S_Brake_Power) {
            Brake_Power = S_Brake_Power;
        }*/

        //Metody
        void TCS(double& F_max, double& F_Eng_cmd, double& F_Eng , Car& TCS_Car);

        void  ABS(Car& ABS_Car, double& F_Brake, double& F_Brake_cmd, double& F_max, double DT);
        
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
    enum class E_ShiftPolicy {Manual, Auto};
    private:
        vector<double> Gears_Ratio = {4.2, 3.6, 2.9, 2.1, 1.7 }; //Biegi od 1 do 5 na potrzeby zadania 
        const double Final_drive = 3.1; //Stały mnożnik dla każdego biegu. Potrzebny do total ration i wyliczania RPM
        const double R_Wheel = 0.3; //[M] - Promień koła, dla liczenia RPM
        int Current_Gear;    
        double RPM;  //RPM ~ wheel_speed * gear_ratio
        static constexpr double Pi = 3.14159265358979323846;
        unique_ptr<ShiftPolicy> Shift_Transmission;
        E_ShiftPolicy Curent_ShiftPolicy = E_ShiftPolicy::Manual;

    public:
        Transmission( int C_Current_Gear = 1, double C_RPM = 0)
            : Current_Gear(C_Current_Gear), RPM(C_RPM), Curent_ShiftPolicy(E_ShiftPolicy::Manual), Shift_Transmission(make_unique<ManualPolicy>()) {}

        //Get
        int get_Current_Gear() const {
            return this -> Current_Gear;
        }
        
        double get_RPM() const {
            return this -> RPM;
        }

        int get_Max_gear() const {
            return Gears_Ratio.size();
        }

        double get_Wheel_Radius() const {
            return this -> R_Wheel;
        }

        //Set
        void set_ShiftPolicy_Manual() {
            Curent_ShiftPolicy = E_ShiftPolicy::Manual;
            Shift_Transmission = make_unique<ManualPolicy>();
        }

        void set_ShiftPolicy_Auto() {
            Curent_ShiftPolicy = E_ShiftPolicy::Auto;
            Shift_Transmission = make_unique<AutoPolicy>();
        }

        //Metody
        bool isAuto() const { return Curent_ShiftPolicy == E_ShiftPolicy::Auto; }

        void change_ShiftPolicy() {
            if (Curent_ShiftPolicy == E_ShiftPolicy::Manual) set_ShiftPolicy_Auto();
            else if (Curent_ShiftPolicy == E_ShiftPolicy::Auto) set_ShiftPolicy_Manual();
        }
        void Gear_up() {
            if (Current_Gear < Gears_Ratio.size()) Current_Gear++;
        }

        void Gear_down() {
            if (Current_Gear > 1) Current_Gear--;
        }

        string Check_ShiftPolicy() {
            if (Curent_ShiftPolicy == E_ShiftPolicy::Manual) return "Manual";
            if (Curent_ShiftPolicy == E_ShiftPolicy::Auto) return "Auto";
            return "Unknown";
        }

        double Total_Ratio() const {  //Całkowite przełożenie
            int Max_Index = static_cast<int>(Gears_Ratio.size()) - 1;
            int Gear_Index = clamp(Current_Gear - 1, 0, Max_Index); 
            return Gears_Ratio[Gear_Index] * Final_drive;
        }

        void Count_RPM(const Car& T_Car); //Deklaracja tutaj a na dole pliku będzie implementacja metody

        void Update_Shift(G_Shift gs) {
            if (Shift_Transmission) Shift_Transmission->Update_Gear(*this, gs);
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

        double Engine_Moment(double RPM, double Car_Throttle) {
            const double IDLE = 900.0; //Min obrotów dla pracy silnika
            const double REDLINE = 7000.0; //Max obrotów dla pracy silnika
            const double Max_Torque= 280.0; //Maksymalny moment obrotowy silnika

            RPM = clamp(RPM, IDLE, REDLINE);

            double RPMFactor = 1.0 - (RPM - IDLE) / (REDLINE - IDLE); //Dla IDLE 1.0 dla REDLINE 0.0

            return Max_Torque * RPMFactor * Car_Throttle;
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

class Dashboard : public ICarObserver {
    private:
        CarState State{};

    public:
        Dashboard() = default;

        //Metody
        void State_Changed(const CarState& New) override {
            State = New;
        }

        string FuelBar(double fuel, double cap, int width = 20) {
            if (cap <= 0) cap = 1;
            double ratio = std::clamp(fuel / cap, 0.0, 1.0);
            int filled = (int)std::lround(ratio * width);
            filled = std::clamp(filled, 0, width);
            return "[" + std::string(filled, '#') + std::string(width - filled, '.') + "]";
        }
        void screen() {
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
                << "\nSpeed:" << State.speedKmh /* * 3.6*/ << " km/h " << "  Throttle: " << State.throttle << "  Brake= " << State.brake
                << " Engine: " << setw(4) <<(State.engineOn ? "ON" : "OFF")
                
                << "\nFuel: " << State.fuelL << " L " <<  FuelBar(State.fuelL, State.fuelCapL, 24) << " " << static_cast<int>(std::lround(std::clamp(State.fuelL / std::max(1.0, State.fuelCapL), 0.0, 1.0) * 100.0)) << "%"
                << " Consumption Fuel Model: "<< setw(7) << State.consumptionMode

                << "\n\nEngine Work time: " << State.workTime
                << "\nMomentary Fuel Consumption: " << State.consMoment
                << "\nAverage Fuel Consumption: " << State.consAvg
                << "\nDistance: " << State.distance
                << "\n\nCurrent gear: " << State.gear
                << "\nRPM: " << State.rpm
                << "\nShiftPolicy transmission: " << setw(7) << State.shiftPolicy

                << "\n\nABS: " << setw(4) << (State.absActive ? "ON" : "OFF") << " TCS: " << setw(4) << (State.tcsActive ? "ON" : "OFF");

            if (State.absEnable) cout<<"\nABS is Enable";
            else cout<<"\nABS is Unable";

            cout <<"\nActual road: " << setw(7) << State.road << " , Actual grade: "<< State.gradePct
                << " , wind: " << State.wind;
                //<< flush;
        }

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
        Environment env;
        TripComputer Car_TripComputer;
        FuelTank Car_FuelTank;
        Brake Car_Brake;
        Engine Car_Engine;
        Transmission Car_Transmission;
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

        bool TCS_Active = false;
        bool ABS_Active = false;
        bool ABS_Enable = true;
        
        //Ogranicznik szybkosci zmian wartosci
        static double Rate_Limiter(double Current_Value, double Target_Value, double Max_Value_For_Rate, double DT) { //DT - Krok czasu, Max_Value_For_Tate = maksymalna wartosc/predkosc na sekunde/klatke
            double Target = Target_Value - Current_Value; //Ile brakuje do celu - do wartosci max dla np. throttle - Przedział od 0.0 do 1.0
            double MAX_One_DT = Max_Value_For_Rate * DT; //Maksymalny krok jaki może być wykonany na 1 krok czasu   
            if (Target > MAX_One_DT) Current_Value += MAX_One_DT;   //Gdy wartosc do celu jest wieksza od max wartosci na klatke np. do max predkosci brakuje 4 km/h, max przyspieszenie na klatke 3km/h 
            else if (Target < -MAX_One_DT) Current_Value -= MAX_One_DT;  //Gdy wartosc do celu jest mniejsza od max wartosci na klatke np. do max predkosci brakuje 2 km/h, max przyszpieczenie na klatke 3km/h
            else Current_Value = Target_Value;
            return clamp(Current_Value, 0.0, 1.0);  //Przycina wartosci do przedzialu [0.0, 1.0] 
        } 

        CarState Update_State() {
            CarState s;
            s.speedKmh = CarSpeed * 3.6;
            s.throttle = CarThrottle;
            s.brake    = CarBrake;

            s.fuelL    = Car_FuelTank.get_FuelTank_Level();
            s.fuelCapL = Car_FuelTank.get_FuelTank_Capacity();

            s.engineOn = Car_Engine.Engine_is_On();
            s.consumptionMode = Car_Engine.Check_Consumption();

            s.gear = Car_Transmission.get_Current_Gear();
            s.rpm  = Car_Transmission.get_RPM();
            s.shiftPolicy = Car_Transmission.Check_ShiftPolicy();

            s.absEnable = ABS_Enable;
            s.absActive = ABS_Active;
            s.tcsActive = TCS_Active;

            s.road = env.get_surface().name();
            s.gradePct = env.get_grade_Percent();
            s.wind = env.get_wind();

            s.workTime = Car_TripComputer.get_Work_Time();
            s.consMoment = Car_TripComputer.get_Momentary_Fuel_Consumption_100KM();
            s.consAvg = Car_TripComputer.get_Average_Fuel_Consumption();
            s.distance = Car_TripComputer.get_Distance();

            return s;
        }

    public:
        /*//Konstruktor 
        Car(FuelTank& C_Car_FuelTank, Brake& C_Car_Brake, Engine& C_Car_Engine, Transmission Car_Transmission, Dashboard& C_Car_Dashboard) 
            :  Car_FuelTank (C_Car_FuelTank), Car_Brake (C_Car_Brake), Car_Engine (C_Car_Engine, C_Car_FuelTank), Car_Transmission (Car_Transmission), Car_Dashboard (C_Car_Dashboard, C_Car_FuelTank, C_Car_Engine) {}
        */
        //Konstruktor domyslny
        Car() 
            : env (),
              Car_TripComputer(),
              Car_FuelTank (), 
              Car_Brake (env), 
              Car_Engine (Car_FuelTank, Car_TripComputer), 
              Car_Transmission (),
              Car_Dashboard ()
            {}

        //OBSERVER
        void UpdateObserver() {
            Car_Dashboard.State_Changed(Update_State());
        }

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
        const Brake& get_Car_Brake() const {
             return Car_Brake; 
        }
        Brake& get_Car_Brake() {
            return Car_Brake;
        }      
        const Transmission& get_Car_Transmission() const {
            return Car_Transmission;
        }    
        Transmission& get_Car_Transmission() {
            return Car_Transmission;
        }    
        Dashboard& get_Car_Dashboard() {
            return Car_Dashboard;
        }
        const Dashboard& get_Car_Dashboard() const { 
            return Car_Dashboard; 
        }
        const TripComputer& get_Trip_Computer() const {
            return Car_TripComputer;
        }
        Environment& get_Environment() {
            return env;
        }
        const Environment& get_Environment() const {
            return env;
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
        double get_MASS_KG() const {
            return this->MASS_KG;
        }
        bool get_TCS_Active() const {
            return this->TCS_Active;
        }
        bool get_ABS_Active() const {
            return this->ABS_Active;
        }
        bool get_ABS_Enable() const {
            return this->ABS_Enable;
        }

        //Settery klas
        void set_FuelTank(FuelTank S_FuelTank) {
            Car_FuelTank = S_FuelTank;
        }       
        /*void set_Brake(Brake S_Brake) {
            Car_Brake = S_Brake;
        }*/        
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
        void set_TCS_Active(bool S_TCS)  {
            TCS_Active = S_TCS;
        }
        void set_ABS_Active(bool S_ABS)  {
            ABS_Active = S_ABS;
        }
       
        //Metody
        string ABS_info() {
            if (ABS_Active) return "ON";
            else return "OFF";
        }
        string TCS_info() {
            if (TCS_Active) return "ON";
            else return "OFF";
        }
        void ABS_enabled_changer() {
            ABS_Enable = !ABS_Enable;
        }

        void Speed_Update (double DT, bool Click_Throttle, bool Click_Brake) {
            //Click_Throttle = false;   TEST DLA NOWEJ MECHANIKI SPRAWDANIA STANU SILNIKA
            //CarThrottle = Rate_Limiter(CarThrottle, Click_Throttle ? 1.0:0.0, Slew ,DT);
            CarThrottle = Car_Engine.Real_Throttle(Rate_Limiter(CarThrottle, Click_Throttle ? 1.0:0.0, Slew ,DT)); //Gdy engine off - throttle igonorowany
            CarBrake = Rate_Limiter(CarBrake, Click_Brake ? 1.0:0.0, Slew ,DT);
            
            //Dla nowej mechaniki ze skrzynią biegów dokonujemy dodatkowych obliczeń
            Car_Transmission.Count_RPM(*this); //Liczy RPM i wewnątrz obektu Transmission wstawia mu wartość
            if (Car_Transmission.isAuto()) Car_Transmission.Update_Shift(G_Shift::Up); //Ignorowanie G_Shift bo automat
            double Actual_Engine_Moment= Car_Engine.Engine_Moment(Car_Transmission.get_RPM(), CarThrottle);

            //Limit przyczepnosci - dla ABS i TCS wraz z aktualizacją o kąt nachylenia drogi
            // F_max = mu * m * g * cos(alpha)
            //Maksymalna siła = współczynnik przyczepności * masa auta * grawitacja * cos(alpha)
            double F_max = std::max(0.0, env.get_surface().mu()) * MASS_KG * g * cos(env.alphaRad());

            //Mechanika przyspieszenia:  Przyspieszenie = Throttle - Brake
            double Acceleration = 0.0;

            double F_Eng = 0.0;         //[N] - siła wpływająca na przyśpieszenie
            double F_Eng_cmd = 0.0;     //[N] - siła żądana 
            double F_Brake = 0.0;       //[N] - siłą wpływająca na przyśpieszenie
            double F_Brake_cmd = 0.0;   //[N] - siła żądana
            double F_Drag = c_drag * (CarSpeed - env.get_wind()) * std::abs(CarSpeed - env.get_wind()); //[N] !!Nie mylić z hamulcem ABS - Tutaj abs to funkcja z biblioteki standardowej!!
            double F_Roll = c_roll;    //[N]
            double F_Grade = MASS_KG * g * sin(env.alphaRad());

            //F_Eng = CarThrottle * F_Eng_MAX;  //Tworzymy moc napedu
            double wheel_Torque = Actual_Engine_Moment * Car_Transmission.Total_Ratio();    //Zamiana momentu sinlinka na siłę napędową. wheel_Torque - moment obrotowy kola
            F_Eng_cmd = wheel_Torque / Car_Transmission.get_Wheel_Radius();  //Tworzymy !żądaną! moc napedu - nowa mechanika na podstawie obrotów i skrzynki biegów. Wartość 0.3 jako stała wartość promienia koła 
            F_Brake_cmd = CarBrake * F_Brake_MAX;   //Tworzymy !żądaną! moc hamulca

            //Ograniczenie żądanej mocy hamulca do możliwej przyczepności
            F_Brake = min(F_Brake_cmd, F_max);
            
            //Reset flag jeżeli wcześniej były użyte te mechanizmy
            ABS_Active = false;
            TCS_Active = false;

            if (!ABS_Enable) {
                //ABS niedostępny - możliwy poślizg
                if(F_Brake_cmd > F_max) {
                    F_Brake = 0.5 * F_max; //Poślizg 
                } else {
                    F_Brake = F_Brake_cmd;
                }
            }
            else {
                //ABS - dostępny
                get_Car_Brake().ABS(*this, F_Brake, F_Brake_cmd, F_max, DT);
            }

            F_Eng = F_Eng_cmd; //Wymaga testu
            get_Car_Brake().TCS(F_max,F_Eng_cmd ,F_Eng, *this);
            
            Acceleration = ((F_Eng - F_Brake - F_Drag - F_Roll - F_Grade) / MASS_KG); //[m/s^2]

            /* Poprzednia mechanika zmiany predkosci
            CarSpeed = max(0.0, CarSpeed + (Acceleration * DT));    //Funkcja wybierajaca wieksza wartosc. Zabezpieczenie przed ujemna predkoscia
            if (CarSpeed >= MAX_SPEED) CarSpeed = MAX_SPEED;    //Ogranicznik maksymalnej predkosci */

            CarSpeed = clamp( (CarSpeed + Acceleration * DT), 0.0, MAX_SPEED_MS);  //Ogranicznik spelniajacy zalozenia: 0 <= V <= Vmax. [m/s]
            Car_Engine.Consume_Fuel(get_CatThrottle(), get_CarSpeed(), DT); //Spalanie paliwa na koniec po okresleniu nowej predkosci
        }
}; 



//Definicje metod z - Transmission
inline void Transmission::Count_RPM(const Car& T_Car) {
    RPM = ( (T_Car.get_CarSpeed() / R_Wheel) * Total_Ratio() ) * 60.0 / (2.0 * Pi);
    }
            /*
            RPM - ilość obrotów w ciągu minuty
            RPM = ( wheel_speed [V / R] * total_ratio [gear * final drive] ) * 60 / (2 * Pi)]
            [rad/x] * [ile obrótów zrobi silnik w momencie gdy koło robi 1 obrót];

            [rad/s]
            V - prędkość
            R - Promień koła <-- na potrzeby zadania będzie to stała wartość
            */


//Definicje metod z - BRAKE
inline void Brake::TCS(double& F_max, double& F_Eng_cmd, double& F_Eng , Car& TCS_Car) {
    //TCS – limit napędu gdy podczas ruszania wchodzimy w poślizg
    //Sprawdzenie: Czy żądana moc silnika nie przekracza maksymalnej przyczepności
    if (F_max > 0.0 && F_Eng_cmd > F_max) {
    TCS_Car.set_TCS_Active(true); //= true;
    F_Eng = tcsK * F_max;   //TCS - włączony więc ograniczamy siłę napędu do 95% zgodnie z tcsK
    }
}

inline void  Brake::ABS(Car& ABS_Car, double& F_Brake, double& F_Brake_cmd, double& F_max, double DT) {
    //ABS
    //Decel to w fizyce opóźnienie, w łatwiejszym tłumaczeniu ujemne przyśpieszenie czyli tempo zmniejszania prędkości w czasie [m/s^2]
    //decel = |F_brake| / m
    double decel = (ABS_Car.get_MASS_KG() > 0.0) ? (abs(F_Brake) / ABS_Car.get_MASS_KG()) : 0.0; //Walidacja sprawdzajaca czy auto cos waży

    //Decel_lock to maksymalne możliwe opóźnienie [Decel]. Jeżeli opóźnienie [Decel] będzie większe od Decel_lock to auto może wpaść w poslizg - ryzyko blokady kół
    //Wartość 0.9 bierze się z tego że chcemy dać 10% marginesu bezpieczeńśtwa. ABS reaguje wcześniej zanim osiągniemy maksymalną wartość limitu tarcia i wpadniemy w poślizg
    const double decel_lock = 0.9 * max(0.0, env.get_surface().mu()) * g;

    //NearLimit zapobiega aktywacji ABS przy lekkim hamowaniu. Sprawdzany jest warunek czy żądana siła hamowania jest wyższa od (maksymalnej siły hamowania [granicy przyczepności] - 5%)
    const bool nearLimit = (F_max > 0.0) ? (F_Brake_cmd > 0.95 * F_max) : false;

    /*Warunki uruchomienia ABS
    1.Auto w ruchu - predkość powyżej 0.5
    2.Warunek nearlimit spełniony = true. Żądana siła hamowania jest wyższa od 95% granicy przyczepności
    3.Aktualne opóźnienie przekracza próg decel_lock, czyli jesteśmy blisko granicy przyczepności i grozi blokowanie kół
    4.F_max > 0.0
    */
    if (ABS_Car.get_CarSpeed() > 0.5 && nearLimit && decel > decel_lock && F_max > 0.0) {
    ABS_Car.set_ABS_Active(true); //= true;

    abs_Timer += DT;
    const double period = max(0.001, abs_Period); //Zabezpieczenie przed dzieleniem przez zero w phase - uniknięcie błędów lub NaN
    const double phase = fmod(abs_Timer, period) / period; //fmod to odpowiednik modulo (%) dla liczb rzeczywistych. Modulo obsługuje tylko int a fmod jest w stanie obsłużyć double bez utraty danych po kropkce

    const bool releasePhase = (phase < clamp(abs_DutyRelease, 0.0, 1.0));
    if (releasePhase) {
    F_Brake = abs_ReleaseK * F_max;   //set
    } else {
    F_Brake = F_max;                 //set
    }
    } else {
    abs_Timer = 0.0;
    }
}