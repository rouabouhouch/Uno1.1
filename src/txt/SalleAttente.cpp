#include "SalleAttente.h"

using namespace std;

const int largeur = 77;

const int hauteur = 17;

const char salleVierge[hauteur][largeur + 1] = {
    "                    UNO                                                     ",
    "                                                                            ",
    "                Salle d'attente                                             ",
    "                                                                            ",
    "     #####################################                                  ",
    "     #                                   #                                  ",
    "     #                                   #                                  ",
    "     #                                   #                                  ",
    "     #                                   #                                  ",
    "     #                                   #                                  ",
    "     #                                   #                                  ",
    "     #                                   #       * Choix du jeu             ",
    "     #                                   #         Nombre de joueurs        ",
    "     #                                   #         Nombre d'ordinateurs     ",
    "     #                                   #         Démarrer                 ",
    "     #####################################                                  ",
    "                                                                            ",
};

const char cadreAccueil[8][25] = {
    "Jeu: classique         ",
    "                       ",
    "                       ",
    "Nombre de joueurs: 2   ",
    "                       ",
    "Nombre d'ordinateurs: 0",
    "                       ",
    "                       "
};

const char cadreRegles[8][25]={
    "1- Jeu classique       ",
    "2- Variante Cumul      ",
    "3- Variante Doublon    ",
    "4- Variante Echange    ",
    "5- Variante Suite      ",
    "6- Variante Tourne     ",
    "                       ",
    "R pour voir les regles "
};

SalleAttente::SalleAttente()
{
    larg = largeur;
    haut = hauteur;
    nombreJoueurs = 2;
    nombreIA = 0;
    variante = 1;
    etoile = 11;
    //initialisation de la salle à partir de la salle vierge
    for (unsigned int x = 0; x < haut; x++)
        for (unsigned int y = 0; y < larg; y++)
            salle[x][y] = salleVierge[x][y];
    insererContenuCadre('a');
}

SalleAttente::~SalleAttente() {}

// Complète le tableau de caractères pour le choix des règles
void SalleAttente::fenetreSalleRegles()
{
    insererContenuCadre('r');
}

// Insère l'intérieur du cadre, a pour accueil, r pour règles
void SalleAttente::insererContenuCadre(char cadre)
{
    assert(cadre == 'a' || cadre == 'r');
    if (cadre == 'a')
        for(int i=0; i<8; i++)
            for(int j=0; j<23; j++)
                salle[i+6][j+13]=cadreAccueil[i][j];
    else
        for(int i=0; i<8; i++)
            for(int j=0; j<23; j++)
                salle[i+6][j+13]=cadreRegles[i][j];
}

// Met à jour le tableau de caractères avec nombreJoueurs, nombreIA, variante
void SalleAttente::MaJFenetreSalle()
{
    // cas où on a l'affichage du choix des règles
    if (salle[6][13] == '1') insererContenuCadre('a');

    // màj de l'étoile
    salle[etoile][49] = '*';
    // màj du nombre de joueurs
    salle[9][32]='0'+ nombreJoueurs;
    // màj du nombre d'IA
    salle[11][35]='0'+nombreIA;
    // màj de la variante
    string reponse;
    switch (variante)
    {
        case 1: reponse="classique";
                break;
        case 2: reponse="variante cumul";
                break;
        case 3: reponse="variante doublon";
                break;
        case 4: reponse="variante echange";
                break;
        case 5: reponse="variante suite";
                break;
        case 6: reponse="variante tourne";
                break;
        default: break;
    }
    int i = 0;
    while (reponse[i] != '\0')
    {
        salle[6][18+i] = reponse[i];
        i++;
    }
}

// Demande le nombre de joueurs et met à jour le tableau de caractères.
void SalleAttente::choixNombreJoueurs()
{
    cout << "Combien de joueurs? (1 à 8) ";
    cin >> nombreJoueurs;
    cout << endl;
    MaJFenetreSalle();
}

// Demande le nombre d'ordinateurs et met à jour le tableau de caractères.
void SalleAttente::choixNombreIA()
{
    cout << "Combien d'ordinateurs? (1 ou 2) ";
    cin >> nombreIA;
    cout << endl;
    MaJFenetreSalle();
}		

// Demande la variante et met à jour le tableau de caractères.
void SalleAttente::choixJeu()
{
    fenetreSalleRegles();
    cout << "Quel jeu? (1 à 6) ";
    cin >> variante;
    cout << endl;
    MaJFenetreSalle();
}
		