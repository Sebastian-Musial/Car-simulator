#pragma once

#include <cmath>
#include <memory>
#include "Physics.hpp"
#include "SurfaceModel.hpp"


double mu = 0.9;                
const double g = 9.81;
const double DT = 0.02;

class Environment {
    private:
        double grade_percent;
        std::unique_ptr<SurfaceModel> surface_mu;
        double wind_ms;
    public:
        //Get
        double gradePercent() const {return grade_percent;}

        //Set
        void set_Surface(std::unique_ptr<SurfaceModel> new_s) {surface_mu = std::move(new_s);} //Wymagane przerzucanie jakiegoś make_unique z nową drogą 
        void setGradePercent(double grade) {grade_percent = grade;}


        //Metody
        void Surface_changer(Environment& env, int key) { //Przesył klawiszy do tej metody zamiast w main 
            if (key == '1') env.set_Surface(std::make_unique<AsphaltSurface>());
            if (key == '2') env.set_Surface(std::make_unique<GravelSurface>());
            if (key == '3') env.set_Surface(std::make_unique<IceSurface>());

            mu = surface_mu->mu();
        }

        void Add_Grade_Percent(double N_Percent) {grade_percent += N_Percent;}

        double alphaRad() const {
            return std::atan(grade_percent / 100.0);
        }
};