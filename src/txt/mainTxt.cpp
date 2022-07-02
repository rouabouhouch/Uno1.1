#include "AffichageTxt.h"
#include "Fenetre.h"
#include "SalleAttente.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    termClear();

    SalleAttente Attente;
    int choixJeu = txtBoucleDebut(Attente);
    sleep(1);
    switch (choixJeu)
    {
    case 1:
    {
        Jeu jeu(Attente.nombreJoueurs, Attente.nombreIA);
        txtBoucle(jeu);
    }
    case 2:
    {
        VarianteCumul jeu(Attente.nombreJoueurs, Attente.nombreIA);
        txtBoucle(jeu);
        break;
    }
    case 3:
    {
        VarianteDoublon jeu(Attente.nombreJoueurs, Attente.nombreIA);
        txtBoucle(jeu);
        break;
    }
    case 4:
    {
        VarianteEchange jeu(Attente.nombreJoueurs, Attente.nombreIA);
        txtBoucle(jeu);
        break;
    }
    case 5:
    {
        VarianteSuite jeu(Attente.nombreJoueurs, Attente.nombreIA);
        txtBoucle(jeu);
        break;
    }
    case 6:
    {
        VarianteTourne jeu(Attente.nombreJoueurs,Attente.nombreIA);
        txtBoucle(jeu);
        break;
    }
    default:
        break;
    }
    termClear();
    return 0;
}
