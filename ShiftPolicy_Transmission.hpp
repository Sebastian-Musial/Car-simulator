#pragma once

class Transmission;

enum class G_Shift { Up, Down}; //Dla sprawdzania klawiszy dla manulpolicy

//<Tylko deklaracje implementacja w .cpp>
/*Wzorzec strategy dla ShiftPolicy*/
class ShiftPolicy {
    public:
        virtual ~ShiftPolicy() = default; //Destruktor
        virtual void Update_Gear(Transmission& tr, G_Shift gs) = 0;
};

class ManualPolicy : public ShiftPolicy {
    public:
        virtual void Update_Gear(Transmission& tr, G_Shift gs) override;
};

class AutoPolicy : public ShiftPolicy {
    //GearUp -> 6200 RPM
    //GearDown -> 1600 RPM
    public:
        virtual void Update_Gear(Transmission& tr, G_Shift /*gs*/) override;
};