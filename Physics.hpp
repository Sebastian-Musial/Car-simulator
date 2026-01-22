#pragma once

#include <algorithm>
#include <cmath>
#include <memory>
#include "SurfaceModel.hpp"

//extern double mu;
extern double g;
extern const double DT;

class Environment {
    private:
        double grade_percent = 0.0;
        std::unique_ptr<SurfaceModel> surface_mu = std::make_unique<AsphaltSurface>();
        double wind_ms = 2.0;
    public:
        //Konstruktor
            Environment() : 
                grade_percent(0.0),
                wind_ms(2.0),
                surface_mu(std::make_unique<AsphaltSurface>()) {}

        //Get
        double get_grade_Percent() const {return grade_percent;}
        const SurfaceModel& get_surface() const {return *surface_mu;}
        double get_wind() const {return wind_ms;}


        //Set
        void set_Surface(std::unique_ptr<SurfaceModel> new_s) {surface_mu = std::move(new_s);} //Wymagane przerzucanie jakiegoś make_unique z nową drogą 
        void set_Grade_Percent(double grade) {grade_percent = std::clamp(grade, -30.0, 30.0);}


        //Metody
        void Add_Grade_Percent(double N_Percent) {set_Grade_Percent(grade_percent + N_Percent);}

        double alphaRad() const {
            return std::atan(grade_percent / 100.0);
        }
};