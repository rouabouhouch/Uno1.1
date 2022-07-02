#include <Bot.h>

using namespace std;

Bot::Bot()
{
    numeroBot = 0;
    nbCarteRouge = 0;
    nbCarteVert = 0;
    nbCarteBleu = 0;
    nbCarteJaune = 0;
    indCarteJoker = -1;
    indCartePlus4 = -1;
    nom = "";
}

Bot::Bot(const unsigned int num)
{
    numeroBot = num;
    nbCarteRouge = 0;
    nbCarteVert = 0;
    nbCarteBleu = 0;
    nbCarteJaune = 0;
    indCarteJoker = -1;
    indCartePlus4 = -1;
    nom = "Bot ";
    nom += to_string(numeroBot);
}
// Trie les cartes du joueur par couleur puis par valeur
void Bot::trierMain()
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

// Compte le nombre de cartes qui ont la couleur du talon
int Bot::carteMemeCouleurTalon(const Jeu &jeu) const
{
    // Selon la couleur de la carte du talon on retourne les variables.
    switch (jeu.talon.back().getCouleur())
    {
    case 1:
        return nbCarteRouge;
        break;
    case 2:
        return nbCarteVert;
        break;
    case 3:
        return nbCarteBleu;
        break;
    case 4:
        return nbCarteJaune;
        break;

    default:
        break;
    }
    return -1; // Erreur.
}
int Bot::cartePourCumul(const Jeu &jeu, int &c, int &indiceCarte)
{
    int indexBot = numeroBot - 1;

    unsigned int i = 0;
    bool aTrouveValeur = false;
    do
    {
        if ((jeu.talon.back().getValeur() == 12) && (jeu.joueursBot[indexBot].main[i].getValeur() == 13 || jeu.joueursBot[indexBot].main[i].getValeur() == 12))
        { // On regarde si on est en train de jouer une variante Cumul et si la valeur du talon est donc un +2 ou +4.
            cout << "Le bot doit jouer un +2 ou +4" << endl;
            aTrouveValeur = true;
            indiceCarte = i;
            c = rand() % 3 + 1;
            return 100; // C'est une valeur max pour dire que ce sera elle qui sera joué en prio.
        }
        else if ((jeu.casPart % 2 == 0) && (jeu.talon.back().getValeur() == 13) && (jeu.joueursBot[indexBot].main[i].getValeur() == 13))
        { // Si la carte du talon est un +4 on doit obligatoirement jouer un +4.
            cout << "Le bot doit jouer un +4" << endl;
            aTrouveValeur = true;
            indCartePlus4 = i;
            indiceCarte = indCartePlus4;
            c = rand() % 3 + 1;
            return 100; // C'est une valeur max pour dire que ce sera elle qui sera joué en prio.
        }
        else
        {
            i++;
        }
    } while (i < jeu.joueursBot[indexBot].main.size() || aTrouveValeur);
    if (aTrouveValeur == false)
    {
        cout << "On a pas de carte +2 ou +4." << endl;
        return 0;
    }
    return -1;
}

// Compte le nombre de cartes de même valeur que le talon
int Bot::carteMemeValeurTalon(const Jeu &jeu, int &c, int &indiceCarte)
{
    //cout << "Fonction Carte meme valeur talon " << endl;
    // variable contenant le nombre de Carte avec la couleur de la carte qui a la meme valeur que celle du talon.
    int nbCarteMemeCouleur = 0;
    // On créer une nouvelle variable pour mettre le bonne indice du bot que l'on regarde.
    int indexBot = numeroBot - 1;
    //cout << " CARTE MEME VALEUR ########### L'actuel carte du talon est : " << jeu.talon.back().getValeur() << " et sa couleur est : " << (jeu.talon.back()).getCouleur() << endl;
    if ((jeu.casPart % 2 == 0 && jeu.casPart != 0) && (jeu.talon.back().getValeur() == 13 || jeu.talon.back().getValeur() == 12))
    {
        int resCumul = cartePourCumul(jeu, c, indiceCarte);
        assert(resCumul >= 0);
        return resCumul;
    }
    for (unsigned int i = 0; i < jeu.joueursBot[indexBot].main.size(); i++)
    {
        cout << "On visite la main du Bot" << endl;
        if (jeu.joueursBot[indexBot].main[i].getValeur() == 13)
        { // On regarde si la carte est un +4 et si on a pas déjà un autre +4 dans la main.
            // edit : Si on a déjà un +4 on en prend pas un autre.
            indCartePlus4 = i;
        }
        else if (jeu.joueursBot[indexBot].main[i].getValeur() == 14)
        { // On regarde si la carte est un Joker et si on a pas déjà un autre joker dans la main.
            // edit : Si on a déjà un joker on en prend pas un autre.
            indCarteJoker = i;
        }
        if (jeu.joueursBot[indexBot].main[i].getValeur() == jeu.talon.back().getValeur() && jeu.joueursBot[indexBot].main[i].getCouleur() != jeu.talon.back().getCouleur())
        {

            // On récupère la Couleur de la carte qui a la même valeur que talon
            int couleurCarte = jeu.joueursBot[indexBot].main[i].getCouleur();
            // Variable pour boucle tant que.
            unsigned int k = 0;
            // variable temp qui permet de compter le nombre de carte que l'on comparera si jamais au nombre de carte déjà trouvé dans d'autre couleur.
            int compareValeur = 0;
            switch (couleurCarte)
            {
            case 1:
                k = 0;
                while (jeu.joueursBot[indexBot].main[k].getCouleur() == 1) // Si on est dans le cas d'une valeur rouge.
                {
                    // Si on a des cartes spéciales dans cette couleur alors on met une valeur plus élevé à jouer cette couleur.
                    if (jeu.joueursBot[indexBot].main[k].getValeur() == 10)
                    {
                        compareValeur = compareValeur + 2;
                        k++;
                    }
                    else if (jeu.joueursBot[indexBot].main[k].getValeur() == 11 ||
                             jeu.joueursBot[indexBot].main[k].getValeur() == 12)
                    {
                        compareValeur = compareValeur + 5;
                        k++;
                    }
                    // On gère le cas des cartes spéciales +4 et joker qui ne représentent pas une valeur pour la couleur.
                    else if (jeu.joueursBot[indexBot].main[k].getValeur() == 13 ||
                             jeu.joueursBot[indexBot].main[k].getValeur() == 14)
                        k++;
                    else
                    {

                        compareValeur++;
                        k++;
                    }
                }
                // Si le nombre de carte est supérieur au nombre de carte déjà trouvé alors on garde la plus grande.
                if (compareValeur > nbCarteMemeCouleur)
                {
                    nbCarteMemeCouleur = compareValeur;
                    c = 1;
                    indiceCarte = i;
                }

                break;
            case 2:
                k = nbCarteRouge;
                while (jeu.joueursBot[indexBot].main[k].getCouleur() == 2) // Si on est dans le cas d'une valeur rouge.
                {

                    // Si on a des cartes spéciales dans cette couleur alors on met une valeur plus élevé à jouer cette couleur.
                    if (jeu.joueursBot[indexBot].main[k].getValeur() == 10)
                    {
                        compareValeur = compareValeur + 2;
                        k++;
                    }
                    else if (jeu.joueursBot[indexBot].main[k].getValeur() == 11 ||
                             jeu.joueursBot[indexBot].main[k].getValeur() == 12)
                    {
                        compareValeur = compareValeur + 5;
                        k++;
                    }
                    // On gère le cas des cartes spéciales +4 et joker qui ne représentent pas une valeur pour la couleur.
                    else if (jeu.joueursBot[indexBot].main[k].getValeur() == 13 ||
                             jeu.joueursBot[indexBot].main[k].getValeur() == 14)
                        k++;
                    else
                    {
                        compareValeur++;
                        k++;
                    }
                }
                if (compareValeur > nbCarteMemeCouleur)
                {
                    nbCarteMemeCouleur = compareValeur;
                    c = 2;
                    indiceCarte = i;
                }

                break;
            case 3:
                k = nbCarteRouge + nbCarteVert;
                while (jeu.joueursBot[indexBot].main[k].getCouleur() == 3 && k < jeu.joueursBot[indexBot].main.size()) // Si on est dans le cas d'une valeur rouge.
                {

                    // Si on a des cartes spéciales dans cette couleur alors on met une valeur plus élevé à jouer cette couleur.
                    if (jeu.joueursBot[indexBot].main[k].getValeur() == 10)
                    {
                        compareValeur = compareValeur + 2;
                        k++;
                    }
                    else if (jeu.joueursBot[indexBot].main[k].getValeur() == 11 ||
                             jeu.joueursBot[indexBot].main[k].getValeur() == 12)
                    {
                        compareValeur = compareValeur + 5;
                        k++;
                    }
                    // On gère le cas des cartes spéciales +4 et joker qui ne représentent pas une valeur pour la couleur.
                    else if (jeu.joueursBot[indexBot].main[k].getValeur() == 13 ||
                             jeu.joueursBot[indexBot].main[k].getValeur() == 14)
                        k++;
                    else
                    {
                        compareValeur++;
                        k++;
                    }
                }
                if (compareValeur > nbCarteMemeCouleur)
                {
                    nbCarteMemeCouleur = compareValeur;
                    c = 3;
                    indiceCarte = i;
                }

                break;
            case 4:
                k = jeu.joueursBot[indexBot].main.size() - 1;
                while (jeu.joueursBot[indexBot].main[k].getCouleur() == 4) // Si on est dans le cas d'une valeur Jaune.
                {
                    // Si on a des cartes spéciales dans cette couleur alors on met une valeur plus élevé à jouer cette couleur.
                    if (jeu.joueursBot[indexBot].main[k].getValeur() == 10)
                    {
                        compareValeur = compareValeur + 2;
                        k--;
                    }
                    else if (jeu.joueursBot[indexBot].main[k].getValeur() == 11 ||
                             jeu.joueursBot[indexBot].main[k].getValeur() == 12)
                    {
                        compareValeur = compareValeur + 5;
                        k--;
                    }
                    // On gère le cas des cartes spéciales +4 et joker qui ne représentent pas une valeur pour la couleur.
                    else if (jeu.joueursBot[indexBot].main[k].getValeur() == 13 ||
                             jeu.joueursBot[indexBot].main[k].getValeur() == 14)
                        k--;
                    else
                    {
                        compareValeur++;
                        k--;
                    }
                }
                if (compareValeur > nbCarteMemeCouleur)
                {
                    nbCarteMemeCouleur = compareValeur;
                    c = 4;
                    indiceCarte = i;
                }

                break;

            default:
                break;
            }
        }
    }
    return nbCarteMemeCouleur;
}

void Bot::joueCouleurSelonEntier(Jeu &jeu, int couleur, int carteSpeciale)
{
    string er;
    int randomNumber;
    srand(time(NULL));
    bool estJokerPlus = false;
    if ((carteSpeciale >= 0) && (main[carteSpeciale].getValeur() == 13 || main[carteSpeciale].getValeur() == 14)) // On change la couleur du joker ou du plus 4 avec la meilleur couleur a jouer.
    {
        main[carteSpeciale].setCouleur(couleur);
        estJokerPlus = true;
    }
    int max; // Variable temporaire servant à définir dans quelle intervalle on génère val aléa.
    int min;
    switch (couleur) // On joue en fonction de la carte du talon.
    {
    case 1:
        cout << "JoueCouleurSelonEntier Rouge" << endl;
        if (carteSpeciale >= 0 && estJokerPlus)
        {
            jeu.poserCarte(carteSpeciale, er);
        }
        else if (carteSpeciale >= 0) // Pas nécessairement un joker.
        {
            changeIndiceCarteSpeciale(carteSpeciale);
            if (nbCarteRouge == 0)
            {
                nbCarteRouge = 0;
            }
            else
                nbCarteRouge--;
            jeu.poserCarte(carteSpeciale, er);
        }
        else
        {
            cout << "Le nombre de carte Rouge est " << nbCarteRouge << endl;
            if (nbCarteRouge == 1)
            {
                changeIndiceCarteSpeciale(0);
                if (nbCarteRouge == 0)
                {
                    nbCarteRouge = 0;
                }
                else
                    nbCarteRouge--;
                jeu.poserCarte(0, er);
            }
            else
            {
                max = rand() % nbCarteRouge;
                changeIndiceCarteSpeciale(max);
                if (nbCarteRouge == 0)
                {
                    nbCarteRouge = 0;
                }
                else
                    nbCarteRouge--;
                jeu.poserCarte(max, er);
            }
        }
        break;
    case 2:
        cout << "JoueCouleurSelonEntier Vert" << endl;
        //cout << "Nombre carte Rouge " << nbCarteVert << endl;
        if (carteSpeciale >= 0 && estJokerPlus)
        {
            jeu.poserCarte(carteSpeciale, er);
        }
        else if (carteSpeciale >= 0)
        {
            if (nbCarteVert == 0)
            {
                nbCarteVert = 0;
            }
            else
                nbCarteVert--;
            changeIndiceCarteSpeciale(carteSpeciale);
            jeu.poserCarte(carteSpeciale, er);
        }
        else
        {
            max = nbCarteRouge + nbCarteVert - 1;
            min = nbCarteRouge;
            cout << "Le nombre de carte Rouge est " << nbCarteRouge << endl;
            cout << "Le nombre de carte Verte est " << nbCarteVert << endl;
            cout << "Le max est " << max << endl;
            cout << "Le min est " << min << endl;
            if (min == max)
            {
                changeIndiceCarteSpeciale(max);
                if (nbCarteVert == 0)
                {
                    nbCarteVert = 0;
                }
                else
                    nbCarteVert--;
                jeu.poserCarte(max, er);
            }
            else
            {
                randomNumber = (rand() % (max - min) + min);
                changeIndiceCarteSpeciale(randomNumber);
                if (nbCarteVert == 0)
                {
                    nbCarteVert = 0;
                }
                else
                    nbCarteVert--;
                jeu.poserCarte(randomNumber, er);
            }
        }
        break;
    case 3:
        cout << "JoueCouleurSelonEntier Bleu" << endl;
        //cout <<  "Nombre carte Rouge " << nbCarteBleu << endl;
        if (carteSpeciale >= 0 && estJokerPlus)
        {
            jeu.poserCarte(carteSpeciale, er);
        }
        else if (carteSpeciale >= 0)
        {
            changeIndiceCarteSpeciale(carteSpeciale);
            if (nbCarteBleu == 0)
            {
                nbCarteBleu = 0;
            }
            else
                nbCarteBleu--;
            jeu.poserCarte(carteSpeciale, er);
        }
        else
        {
            max = nbCarteRouge + nbCarteVert + nbCarteBleu - 1;
            min = nbCarteRouge + nbCarteVert;
            cout << "Le nombre de carte Rouge est " << nbCarteRouge << endl;
            cout << "Le nombre de carte Verte est " << nbCarteVert << endl;
            cout << "Le nombre de carte Bleu est " << nbCarteBleu << endl;
            cout << "Le max est " << max << endl;
            cout << "Le min est " << min << endl;
            if (min == max)
            {
                changeIndiceCarteSpeciale(max);
                if (nbCarteBleu == 0)
                {
                    nbCarteBleu = 0;
                }
                else
                    nbCarteBleu--;
                jeu.poserCarte(max, er);
            }
            else
            {
                randomNumber = (rand() % (max - min) + min);
                changeIndiceCarteSpeciale(randomNumber);
                if (nbCarteBleu == 0)
                {
                    nbCarteBleu = 0;
                }
                else
                    nbCarteBleu--;
                jeu.poserCarte(randomNumber, er);
            }
        }
        break;
    case 4:

        cout << "JoueCouleurSelonEntier Jaune" << endl;
        //cout <<  "Nombre carte Rouge " << nbCarteJaune << endl;
        if (carteSpeciale >= 0)
        {
            if (nbCarteJaune == 0)
            {
                nbCarteJaune = 0;
            }
            else
                nbCarteJaune--;
            jeu.poserCarte(carteSpeciale, er);
        }
        else
        {
            max = nbCarteRouge + nbCarteVert + nbCarteBleu + nbCarteJaune - 1;
            min = nbCarteRouge + nbCarteVert + nbCarteBleu;
            cout << "Le nombre de carte Rouge est " << nbCarteRouge << endl;
            cout << "Le nombre de carte Verte est " << nbCarteVert << endl;
            cout << "Le nombre de carte Bleu est " << nbCarteBleu << endl;
            cout << "Le nombre de carte Jaune est " << nbCarteJaune << endl;
            cout << "Le max est " << max << endl;
            cout << "Le min est " << min << endl;
            if (min == max)
            {
                changeIndiceCarteSpeciale(max);
                if (nbCarteJaune == 0)
                {
                    nbCarteJaune = 0;
                }
                else
                    nbCarteJaune--;
                jeu.poserCarte(max, er);
            }
            else
            {
                randomNumber = (rand() % (max - min) + min);
                changeIndiceCarteSpeciale(randomNumber);
                if (nbCarteJaune == 0)
                {
                    nbCarteJaune = 0;
                }
                else
                    nbCarteJaune--;
                jeu.poserCarte(randomNumber, er);
            }
        }
        break;

    default:
        break;
    }
}
void Bot::changeIndiceCarteSpeciale(int indiceCarte)
{
    // Comme les indices des jokers sont absolues car on les récups lors de la distrib des cartes,
    // On doit les modifier pour ne pas avoir un segfault.
    if ((indCarteJoker > 0 && indCartePlus4 == -1))
    {
        if (indCarteJoker < indiceCarte)
            return;
        indCarteJoker--;
    }

    else if (indCarteJoker == -1 && indCartePlus4 > 0)
    {
        if (indCartePlus4 < indiceCarte)
            return;
        indCartePlus4--;
    }
    else if (indCarteJoker > 0 && indCartePlus4 > 0)
    {
        if (indCarteJoker < indiceCarte && indCartePlus4 < indiceCarte)
            return;
        else if (indCarteJoker < indiceCarte && indCartePlus4 > indiceCarte)
        {
            indCartePlus4--;
            return;
        }
        else if (indCartePlus4 < indiceCarte && indCarteJoker > indiceCarte)
        {
            indCarteJoker--;
            return;
        }
        else
        {
            indCartePlus4--;
            indCarteJoker--;
            return;
        }
    }
}

void Bot::choixJeu(Jeu &jeu)
{
    //VarianteCumul cumul; // Ne sert pas à grand chose à part comparer avec jeu.
    cout << "La taille de la main du Bot est : " << main.size() << endl;
    // Affiche les cartes dans la main du bot.
    int nbCarteCouleur, nbCarteValeur;
    int maxNbCouleur = couleurAvecPlusDeCarte(); // Renvoie la couleur avec le plus de carte.
    nbCarteCouleur = carteMemeCouleurTalon(jeu); // Nombre de carte meme couleur que talon.
    int couleur;                                 // Entier qui permettra de savoir quelle couleur a le plus de carte.
    int indiceCarteMemeValeur;
    nbCarteValeur = carteMemeValeurTalon(jeu, couleur, indiceCarteMemeValeur);
    for (auto &i : main)
    {
        cout << "La valeur est " << i.getValeur() << " et la couleur " << i.getCouleur() << endl;
    }
    cout << "L'indice de la carte du joker est : " << indCarteJoker << endl;
    cout << "L'indice de la carte du Plus 4 est : " << indCartePlus4 << endl;
    assert(nbCarteCouleur >= 0 || nbCarteValeur >= 0);
    // On compare les deux en prenant en compte que c'est mieux de finir une couleur avant d'en changer
    if ((jeu.casPart % 2 == 0) && (jeu.talon.back().getValeur() == 12 || jeu.talon.back().getValeur() == 13))
    {
        if (nbCarteValeur == 0)
        { // On est dans le cas où le bot ne peut pas jouer car il n'a pas de carte +2 ou +4 ou parce qu'il n'a pas de +4.
            jeu.piocherCarte();
            jeu.termineTour();
        }
        else
        {                                                                // On peut jouer dans un des deux cas cités dans la fonction CarteMemeValeurTalon.
            joueCouleurSelonEntier(jeu, couleur, indiceCarteMemeValeur); // Ce n'est pas vraiment la même valeur à proprement parler mais ça permetera de jouer le cumul.
        }
    }
    else if (!(nbCarteCouleur == 0) && nbCarteValeur == 0) // Cas où on peut jouer que les cartes mêmes couleurs.
    {
        cout << "Joue carte meme couleur" << endl;
        joueCouleurSelonEntier(jeu, jeu.talon.back().getCouleur(), -1);
    }
    else if (!(nbCarteCouleur == 0) && !(nbCarteValeur == 0)) // Cas où on peut jouer Carte Couleur.
    {
        if (nbCarteCouleur > nbCarteValeur || nbCarteCouleur == 1) // Cas où c'est mieux de jouer meme couleur.
        {
            cout << "Joue meme couleur quand on peut jouer meme valeur" << endl;
            joueCouleurSelonEntier(jeu, jeu.talon.back().getCouleur(), -1);
        }
        else if (nbCarteCouleur == 2 && nbCarteValeur == 1)
        { // Si on a que 1 de différence mais la possibilité de finir la couleur on fini.
            joueCouleurSelonEntier(jeu, jeu.talon.back().getCouleur(), -1);
        }
        else if (nbCarteValeur == 2 && nbCarteCouleur == 1)
        { // Si on a que 1 de différence mais la possibilité de finir la couleur on fini.
            joueCouleurSelonEntier(jeu, couleur, -1);
        }
        else // On a plus de carte de la couleur de celle qui a la meme valeur que talon.
        {
            cout << "Joue meme valeur quand on a quand meme couleur" << endl;
            joueCouleurSelonEntier(jeu, couleur, indiceCarteMemeValeur);
        }
    }
    else if (!(nbCarteValeur == 0) && nbCarteCouleur == 0) // On ne peut jouer que la carte avec la même valeur.
    {
        // On joue la carte qui a la même valeur.
        cout << "On joue la carte qui a la meme valeur car on peut pas jouer autre chose" << endl;
        joueCouleurSelonEntier(jeu, couleur, indiceCarteMemeValeur);
    }
    else if ((nbCarteValeur == 0 && nbCarteCouleur == 0) && indCarteJoker >= 0) // Change couleur.
    {                                                                           // Si on peut pas jouer de carte meme valeur ou couleur talon.
        // changerCouleurCarte(jeu, maxCouleur);
        cout << "Indice du joker est : " << indCarteJoker << endl;
        cout << "La valeur du Joker doit être " << main[indCarteJoker].getValeur() << " la couleur est " << main[indCartePlus4].getCouleur() << endl;
        cout << "On joue le joker car pas d'autres options avec indice : " << indCarteJoker << endl;
        joueCouleurSelonEntier(jeu, maxNbCouleur, indCarteJoker);
        indCarteJoker = -1;
        cout << "suppression Joker" << endl;
    }
    else if ((nbCarteValeur == 0 && nbCarteCouleur == 0) && indCartePlus4 >= 0) // +4
    {
        cout << "Indice du +4 est : " << indCartePlus4 << endl;
        cout << "La valeur du +4 doit être " << main[indCartePlus4].getValeur() << " la couleur est " << main[indCartePlus4].getCouleur() << endl;
        cout << "On joue le joker +4 car pas d'autres options avec indice : " << indCartePlus4 << endl;
        joueCouleurSelonEntier(jeu, maxNbCouleur, indCartePlus4);
        indCartePlus4 = -1;
        cout << "suppression +4" << endl;
    }
    else
    {
        cout << "On pioche" << endl;
        // On pioche car on ne peut pas jouer.
        jeu.piocherCarte();
        jeu.termineTour();
    }
}

int Bot::couleurAvecPlusDeCarte() const
{
    int tabNbCarte[] = {nbCarteRouge, nbCarteVert, nbCarteBleu, nbCarteJaune};
    int max = tabNbCarte[0];
    int couleur = 0;
    int length = sizeof tabNbCarte / sizeof tabNbCarte[0];
    for (int n = 1; n < length; n++)
    {
        if (tabNbCarte[n] > max && tabNbCarte[n] > 0)
        {
            max = tabNbCarte[n]; // On met à jour le max pour continuer à comparer.
            couleur = n;         // On stock quelle couleur est la plus grande.
        }
    }
    if (couleur == 0)
    {
        couleur = 1;
    }
    return couleur;
}
void Bot::setCarteRouge()
{
    nbCarteRouge++;
}
void Bot::setCarteVert()
{
    nbCarteVert++;
}
void Bot::setCarteBleu()
{
    nbCarteBleu++;
}
void Bot::setCarteJaune()
{
    nbCarteJaune++;
}
void Bot::setCarteJoker(int indiceJoker)
{
    indCarteJoker = indiceJoker;
}
void Bot::setCartePlus4(int indicePlus4)
{
    indCartePlus4 = indicePlus4;
}
int Bot::getCarteRouge()
{
    return nbCarteRouge;
}
int Bot::getCarteVert()
{
    return nbCarteVert;
}
int Bot::getCarteBleu()
{
    return nbCarteBleu;
}
int Bot::getCarteJaune()
{
    return nbCarteJaune;
}
int Bot::getCarteJoker()
{
    //assert(indiceJoker >= 0);
    return indCarteJoker;
}
int Bot::getCartePlus4()
{
    //assert(indicePlus4 >= 0);
    return indCartePlus4;
}
void Bot::remetNbCarteZero()
{
    nbCarteRouge = 0;
    nbCarteVert = 0;
    nbCarteBleu = 0;
    nbCarteJaune = 0;
}
void Bot::copieNbCarte(Bot &bot)
{
    nbCarteRouge = bot.nbCarteRouge;
    nbCarteVert = bot.nbCarteVert;
    nbCarteBleu = bot.nbCarteBleu;
    nbCarteJaune = bot.nbCarteJaune;
}

void Bot::testRegression(Jeu &jeu)
{
    assert(main.size() >= 7);
    assert(numeroBot >= 0);
    assert(!nom.empty());
    trierMain();
    // Affiche les cartes dans la main du bot.
    for (auto &i : main)
    {
        cout << "La valeur est " << i.getValeur() << " et la couleur " << i.getCouleur() << endl;
    }

    int CouleurTalon = carteMemeCouleurTalon(jeu);
    int c;
    int indv;
    int nbValeur = carteMemeValeurTalon(jeu, c, indv);

    switch (c)
    {
    case 1:
        cout << "La couleur est Rouge" << endl;
        break;
    case 2:
        cout << "La couleur est Vert" << endl;
        break;
    case 3:
        cout << "La couleur est Bleu" << endl;
        break;
    case 4:
        cout << "La couleur est Jaune" << endl;
        break;
    }
    // Test sur le choixJeu du bot.
    choixJeu(jeu);
}
