#include "Carte.h"

// intialisation de valeur et couleur à 0 et du tableau vierge pour l'affichage txt
Carte::Carte()
{
    valeur = 0;
    couleur = 0;
    dimx = 8;
    dimy = 11;
    positionEtoile = 0;
    for (int x = 0; x < dimx; ++x)
        for (int y = 0; y < dimy; ++y)
            carte[x][y] = carteVierge[x][dimy - 1 - y];
}

// initialisation de valeur, de couleur et du tableau pour l'affichage txt
Carte::Carte(const unsigned int val, const unsigned int col)
{
    assert(val <= 14);
    assert(col >= 1 && col <= 4);
    valeur = val;
    couleur = col;
    dimx = 8;
    dimy = 11;
    positionEtoile = 0;
    for (int x = 0; x < dimx; ++x)
        for (int y = 0; y < dimy; ++y)
            carte[x][y] = carteVierge[x][dimy - 1 - y];
    // On met la valeur de la carte.
    if (val >= 0 && val <= 9)
        carte[3][5] = '0' + val;
    else
        switch (val)
        {
        case 10:
            carte[3][4] = 'i';
            carte[3][5] = 'n';
            carte[3][6] = 'v';
            break;
        case 11:
            carte[3][4] = 'p';
            carte[3][5] = 'a';
            carte[3][6] = 's';
            break;
        case 12:
            carte[3][5] = '+';
            carte[3][6] = '2';
            break;
        case 13:
            carte[3][5] = '+';
            carte[3][6] = '4';
            break;
        case 14:
            carte[3][3] = 'j';
            carte[3][4] = 'o';
            carte[3][5] = 'k';
            carte[3][6] = 'e';
            carte[3][7] = 'r';
            break;
        }
    // On met la couleur de la carte.
    switch (col)
    {
    case 1:
        carte[4][3] = 'r';
        carte[4][4] = 'o';
        carte[4][5] = 'u';
        carte[4][6] = 'g';
        carte[4][7] = 'e';
        break;
    case 2:
        carte[4][4] = 'v';
        carte[4][5] = 'e';
        carte[4][6] = 'r';
        carte[4][7] = 't';
        break;
    case 3:
        carte[4][4] = 'b';
        carte[4][5] = 'l';
        carte[4][6] = 'e';
        carte[4][7] = 'u';
        break;
    case 4:
        carte[4][3] = 'j';
        carte[4][4] = 'a';
        carte[4][5] = 'u';
        carte[4][6] = 'n';
        carte[4][7] = 'e';
        break;
    }
}

Carte::~Carte() {}

// récupère la valeur de la carte
unsigned int Carte::getValeur() const
{
    return valeur;
}

// récupère la couleur de la carte
unsigned int Carte::getCouleur() const
{
    return couleur;
}

// modifie la couleur de la carte
void Carte::setCouleur(const unsigned int c)
{
    assert (c<5);
    couleur = c;
}

// modifie la valeur de la carte
void Carte::setValeur(const unsigned int val)
{
    assert (val<15);
    valeur = val;
}

// pour un tri par couleur, puis par valeur
bool Carte::operator<(const Carte &c2)
{
    return (couleur < c2.getCouleur()) || (couleur == c2.getCouleur() && valeur < c2.getValeur());
}

// valeurs et couleurs égales
bool Carte::operator==(const Carte &c)
{
    return (couleur == c.getCouleur()) && (valeur == c.getValeur());
}
void Carte::afficher(ostream &flux) const
{
    flux << "La valeur de la carte est "<< valeur << " de couleur " << couleur << endl;
}
ostream &operator<<( ostream &flux, Carte const& c)
{
    c.afficher(flux) ;
    return flux;
}