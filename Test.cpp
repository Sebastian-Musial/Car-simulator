#include <iostream>
#include "class.hpp"

double TEST_STOP (Car & X, const double & DT) {
    static int Once = 0;
    static double Ods = 0.0; 
    if (Once == 0) {
        X.set_CarBrake(1.0);
        X.set_CarSpeed(14.0);
    }
    Ods += X.get_CarSpeed();
    Once++;
    X.Speed_Update (DT, false, true);
    return Ods;
}