#include <iostream>

#include "../core/calc/mathEnigne.hpp"

int main(){
    std::cout<<"Debug"<<std::endl;

    MathEngine mathEngine;

    mathEngine.calcolaFunzione3D("(x^4 - 6x^2y^2+y^4) / (x^2+y^2 +1)^3");
}