#pragma once

//Deklaracja klasy oraz metod potrzebnych dodziałania poniższego kodu
//Są to tylko deklaracje a definicje w innym pliku - wymagane do poprawnej kompilacji
/*Obejścia używać ostrożnie - najlepiej kopiuj i wklej z prawdziwej definicji*/
/*Propozycja na późniejsze rozwiązanie - przebudowa klas poprzez rozbicie na większą ilość pików wtedy takie obejście nie będzie wymagane*/
class Transmission {
    public:
        double get_RPM() const;
        int get_Max_gear() const;
        int get_Current_Gear() const;
        void Gear_up();
        void Gear_down();
};

enum class G_Shift { Up, Down}; //Dla sprawdzania klawiszy dla manulpolicy

/*Wzorzec strategy dla ShiftPolicy*/
class ShiftPolicy {
    public:
        virtual ~ShiftPolicy() = default; //Destruktor
        virtual void Update_Gear(Transmission& tr, G_Shift gs) = 0;
};

class ManualPolicy : public ShiftPolicy {
    public:
        virtual void Update_Gear(Transmission& tr, G_Shift gs) override {
            if (gs == G_Shift::Up) tr.Gear_up();
            if (gs == G_Shift::Down) tr.Gear_down();
        }
};

class AutoPolicy : public ShiftPolicy {
    //GearUp -> 6200 RPM
    //GearDown -> 1600 RPM
    public:
        virtual void Update_Gear(Transmission& tr, G_Shift /*gs*/) override {
            if ( (tr.get_RPM() >= 6200.0) && (tr.get_Current_Gear() < tr.get_Max_gear()) ) {
                tr.Gear_up();
            }
            else if ( (tr.get_RPM() < 1600.0) && (tr.get_Current_Gear() > 1 )) {
                tr.Gear_down();
            }
        }
};