#include <iostream>
#include "class.hpp"

extern const double DT;

/*double TEST_STOP (Car & X, const double & DT) {
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
}*/

//TEST_Time - liczy czas
//TEST_DT_time - liczy kroki czasu/FPS
class TEST {
    private:
        double TEST_DT_time = 0.0;
        double TEST_time = 0.0;
        double TEST_Distance = 0.0;
        bool TEST_START = false;
    public:
        TEST() 
            : TEST_DT_time(),
              TEST_time(),
              TEST_Distance() {}

        //Get
        double get_Test_Time() const {
            return TEST_time;
        }
        double get_Test_Distance() const {
            return TEST_Distance;
        }
        bool get_Test_Start() const {
            return TEST_START;
        }

        //Metody
        void Start_Test_Time(double Car_Speed) {
            TEST_time += DT;
            TEST_Distance += Car_Speed * DT;
        }

        void Test_ON_OFF() {
            if (TEST_START) TEST_START = false;
            else TEST_START = true;
        }

        void Reset() {
            TEST_DT_time = 0.0;
            TEST_time = 0.0;
            TEST_Distance = 0.0;
        }
};