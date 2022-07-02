#include <Joueur.h>

const int largeur = 180;

const int hauteur = 38;

const char tableVierge[hauteur][largeur + 1] = {
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                     ###########                    ###########                                                                     ",
    "                                                                     #         #                    #         #                                                                     ",
    "                                             ############            #  Pioche #    ############    #  Talon  #            ############                                             ",
    "                                             #   UNO    #            #         #    #  Passer  #    #         #            #Contre UNO#                                             ",
    "                                             # touche U #            #         #    # touche P #    #         #            # touche C #                                             ",
    "                                             ############            #  cartes #    ############    #         #            ############                                             ",
    "                                                                     #         #                    #         #                                                                     ",
    "                                                                     ###########                    ###########                                                                     ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    ",
    "                                                                                                                                                                                    "};

Joueur::Joueur()
{
    numeroJoueur = 1;
    nom = "";
    larg = largeur;
    haut = hauteur;
    //initialisation de la table du Joueur à partir de la table vierge
    for (unsigned int x = 0; x < haut; x++)
        for (unsigned int y = 0; y < larg; y++)
            tableJoueur[x][y] = tableVierge[x][y];
}

// Affecte num à numeroJoueur et crée la table du joueur avec pioche et talon
Joueur::Joueur(const unsigned int num)
{
    numeroJoueur = num;
    larg = largeur;
    haut = hauteur;
    nom = "Joueur ";
    nom += to_string(numeroJoueur);
    indiceEtoile = 0;
    //initialisation de la table du Joueur à partir de la table vierge
    for (unsigned int x = 0; x < haut; x++)
        for (unsigned int y = 0; y < larg; y++)
            tableJoueur[x][y] = tableVierge[x][y];
}

Joueur::~Joueur() {}

// Insère une carte dans tableJoueur pour version txt
void Joueur::insererCartePositionIJ(const unsigned int indi, const unsigned int indj, Carte &c, const bool aEtoile)
{
    assert(indi < 38);
    assert(indj < 180);
    for (unsigned i = indi; i < indi + 8; i++)
    {
        for (unsigned int j = indj; j < indj + 11; j++)
        {
            tableJoueur[i][j] = c.carte[i - indi][j - indj];
            if (aEtoile)
                c.carte[1][5] = '*';
            else
                c.carte[1][5] = ' ';
        }
    }
}

// Insère une carte adversaire, sans le nombre de cartes, dans tableJoueur pour version txt
void Joueur::insererCarteAdversairePositionJ(const unsigned int indj, const Joueur & j)
{
    assert(indj<180);
    int i = 0;
    // nom du joueur
    while (j.nom[i]!='\0') 
    {
        tableJoueur[0][indj+i]=j.nom[i];
        i++;
    }
    
    //contour de la carte
    for (unsigned int i = 1; i < 9; i++)
        for (unsigned int j = indj; j < indj + 11; j++)
            tableJoueur[i][j] = carteVierge[i - 1][j - indj];

    tableJoueur[5][indj + 2] = 'c';
    tableJoueur[5][indj + 3] = 'a';
    tableJoueur[5][indj + 4] = 'r';
    tableJoueur[5][indj + 5] = 't';
    tableJoueur[5][indj + 6] = 'e';
    tableJoueur[5][indj + 7] = 's';
}

// Trie les cartes du joueur par couleur puis par valeur
void Joueur::trierMain()
{
    Carte cMin;
    int indMin;
    for (unsigned int i = 0; i < main.size(); i++)
    {
        cMin = main[i];
        indMin = i;
        for (unsigned int j = i + 1; j < main.size(); j++)
            if (main[j] < cMin)
            {
                cMin = main[j];
                indMin = j;
            }
        main.insert(main.begin() + i, cMin);
        main.erase(main.begin() + indMin + 1);
    }
}

// met des " " dans tableJoueur dans toute la zone de la main en bas pour la version txt
void Joueur::effacerMainTxt()
{
    for (unsigned int i = 22; i < haut; i++)
        for (unsigned int j = 0; j < larg; j++)
            tableJoueur[i][j] = ' ';
}

// Dessine toutes les cartes de la main dans tableJoueur pour la version txt
void Joueur::dessinerMainTxt()
{
    int ind = 0;
    // le nom
    while (nom[ind]!='\0') 
    {
        tableJoueur[21][0+ind]=nom[ind];
        ind++;
    }
    // les cartes
    for (unsigned int i = 0; i < main.size(); i++)
    {
        if (indiceEtoile == i) // On affiche le curseur dans la carte à l'indice i.
        {
            insererCartePositionIJ(22 + 9 * (main.size() / 15), 12 * (i % 15), main[i], true);
        }
        else
        {
            insererCartePositionIJ(22 + 9 * (main.size() / 15), 12 * (i % 15), main[i], false);
        }
    }
}

// Pour la version txt
void Joueur::modifMainTxt()
{
    effacerMainTxt(); // On efface la du joueur.
    trierMain();      // On trie la main du joueur (ordre couleur et dans les couleurs ordres val cartes.)
    dessinerMainTxt();
}

// écrit la carte du talon et le nombre de cartes de la pioche dans tableJoueur de chaque joueur
// t talon, p pioche
void Joueur::modifTalonPiocheTxt(const queue<Carte> &t, const stack<Carte> &p)
{
    // écrit la carte du talon dans tableJoueur
    unsigned int v = t.back().getValeur();
    unsigned int c = t.back().getCouleur();
    // écrit la valeur numérique de la carte
    for (int i = 0; i < 5; i++)
        tableJoueur[15][103 + i] = ' ';
    if (v >= 0 && v <= 9)
        tableJoueur[15][105] = '0' + v;
    else // écrit le symbole de la carte
        switch (v)
        {
        case 10:
            tableJoueur[15][104] = 'i';
            tableJoueur[15][105] = 'n';
            tableJoueur[15][106] = 'v';
            break;
        case 11:
            tableJoueur[15][104] = 'p';
            tableJoueur[15][105] = 'a';
            tableJoueur[15][106] = 's';
            break;
        case 12:
            tableJoueur[15][105] = '+';
            tableJoueur[15][106] = '2';
            break;
        case 13:
            tableJoueur[15][105] = '+';
            tableJoueur[15][106] = '4';
            break;
        case 14:
            tableJoueur[15][103] = 'j';
            tableJoueur[15][104] = 'o';
            tableJoueur[15][105] = 'k';
            tableJoueur[15][106] = 'e';
            tableJoueur[15][107] = 'r';
            break;
        }
    // écrit la couleur
    for (int i = 0; i < 5; i++)
        tableJoueur[16][103 + i] = ' ';
    switch (c)
    {
    case 1:
        tableJoueur[16][103] = 'r';
        tableJoueur[16][104] = 'o';
        tableJoueur[16][105] = 'u';
        tableJoueur[16][106] = 'g';
        tableJoueur[16][107] = 'e';
        break;
    case 2:
        tableJoueur[16][104] = 'v';
        tableJoueur[16][105] = 'e';
        tableJoueur[16][106] = 'r';
        tableJoueur[16][107] = 't';
        break;
    case 3:
        tableJoueur[16][104] = 'b';
        tableJoueur[16][105] = 'l';
        tableJoueur[16][106] = 'e';
        tableJoueur[16][107] = 'u';
        break;
    case 4:
        tableJoueur[16][103] = 'j';
        tableJoueur[16][104] = 'a';
        tableJoueur[16][105] = 'u';
        tableJoueur[16][106] = 'n';
        tableJoueur[16][107] = 'e';
        break;
    }

    // écrit le nombre de cartes de la pioche dans tableJoueur
    unsigned int taille = p.size();
    if (taille >= 100)
    {
        tableJoueur[15][73] = '1';
        tableJoueur[15][74] = '0' + (taille - 100) / 10;
        tableJoueur[15][75] = '0' + taille % 10;
    }
    else if (taille >= 10)
    {
        tableJoueur[15][74] = '0' + taille / 10;
        tableJoueur[15][75] = '0' + taille % 10;
    }
    else
        tableJoueur[15][74] = '0' + taille;
}

// gagnant si la main est vide
bool Joueur::gagnant()
{
    return main.empty();
}

// récupère la largeur pour version txt
unsigned int Joueur::getLarg() const { return larg; }

// récupère la hauteur pour la version txt
unsigned int Joueur::getHaut() const { return haut; }

// récupère le caractère à la position x,y pour la version txt
char Joueur::getXY(const int x, const int y) const
{
    assert(x >= 0);
    assert(y >= 0);
    assert(x < hauteur);
    assert(y < largeur);
    return tableJoueur[x][y];
}
