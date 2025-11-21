#include <iostream>
#include <algorithm> //Wymagane dla std::clamp(value, min, max); + std::max(Value_A, Value_B);

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