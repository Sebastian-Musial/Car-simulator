#include <memory> //Wymagana do uzytego wzorca state dla stanu silnika

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