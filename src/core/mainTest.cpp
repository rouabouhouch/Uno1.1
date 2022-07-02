#include "Jeu.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{

    Jeu jeu(2,1);
    jeu.testRegression();
    jeu.joueursBot[0].testRegression(jeu);
    
    return 0;
}
