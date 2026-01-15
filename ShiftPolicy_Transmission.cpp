#include "class.hpp" 

//Implementacja metod klas ShiftPolicy z pliku .hpp

void ManualPolicy::Update_Gear(Transmission& tr, G_Shift gs) {
    if (gs == G_Shift::Up)   tr.Gear_up();
    if (gs == G_Shift::Down) tr.Gear_down();
}

void AutoPolicy::Update_Gear(Transmission& tr, G_Shift /*gs*/) {
    // GearUp -> 6200 RPM
    // GearDown -> 1600 RPM
    if ((tr.get_RPM() >= 6200.0) && (tr.get_Current_Gear() < tr.get_Max_gear())) {
        tr.Gear_up();
    }
    else if ((tr.get_RPM() < 1600.0) && (tr.get_Current_Gear() > 1)) {
        tr.Gear_down();
    }
}