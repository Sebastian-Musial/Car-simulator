#pragma once

#include <cmath>
#include <memory>
#include "SurfaceModel.hpp"


double mu = 0.9;                
const double g = 9.81;
const double DT = 0.02;

class Environment {
    private:
        double grade_percent = 0.0;
        std::unique_ptr<SurfaceModel> surface_mu = std::make_unique<AsphaltSurface>();
        double wind_ms = 0.0;
    public:
        //Konstruktor
            Environment() : 
                grade_percent(0.0),
                wind_ms(0.0),
                surface_mu(std::make_unique<AsphaltSurface>()) {}

        //Get
        double gradePercent() const {return grade_percent;}

        //Set
        void set_Surface(std::unique_ptr<SurfaceModel> new_s) {surface_mu = std::move(new_s);} //Wymagane przerzucanie jakiegoś make_unique z nową drogą 
        void setGradePercent(double grade) {grade_percent = grade;}


        //Metody
        void Surface_changer(int key) { //Przesył klawiszy do tej metody zamiast w main 
            if (key == '1') set_Surface(std::make_unique<AsphaltSurface>());
            if (key == '2') set_Surface(std::make_unique<GravelSurface>());
            if (key == '3') set_Surface(std::make_unique<IceSurface>());

            mu = surface_mu->mu();
        }

        void Add_Grade_Percent(double N_Percent) {grade_percent += N_Percent;}

        double alphaRad() const {
            return std::atan(grade_percent / 100.0);
        }
};