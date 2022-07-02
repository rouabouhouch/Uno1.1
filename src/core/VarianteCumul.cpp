#include "VarianteCumul.h"

VarianteCumul::VarianteCumul() : Jeu() {}

VarianteCumul::VarianteCumul(const unsigned int nbjoueurs, const unsigned int nbIA = 0) : Jeu(nbjoueurs, nbIA)
{
    casPart = 0;
}

VarianteCumul::~VarianteCumul()
{
    delete[] joueurs;
    joueurs = NULL;

    delete[] joueursBot;
    joueursBot = NULL;

    sensJeu = 1;
    joueurActif = 0;
    nombreJoueurs = 0;
    nombreIA = 0;
}
// On compare la carte que l'on a passé en paramètre à celle qui est actuellement retourné sur le talon.
bool VarianteCumul::carteValide(const Carte c) const
{
    bool chercheCouleur = false;
    return (c.getValeur() == talon.back().getValeur()) ||
           (c.getCouleur() == talon.back().getCouleur() && (talon.back().getValeur() != 12 || casPart % 2 != 0 || casPart <= 0)) || // bloquer le cas du +2 sur le talon
           (c.getValeur() == 12 && (talon.back().getValeur() != 13 && casPart % 2 != 0)) ||                                         // bloquer le +2 sur +4
           (c.getValeur() == 14 && talon.back().getValeur() != 13 && talon.back().getValeur() != 12) ||                             // le joker ne peut pas se poser sur un +2 ou un +4
           (c.getValeur() == 13 && talon.back().getValeur() != 13 && (talon.back().getValeur() == 12 || chercheCouleur == false));  //on peut jouer +4 sur n'importe quelle carte si on n'a pas la couleur ou sur +2
}

// pose une carte et gère les cartes spéciales
void VarianteCumul::poserCarte(const unsigned int &indiceCarte, string &messageErreur)
{
    assert(indiceCarte >= 0);
    if (joueurActif >= nombreJoueurs) // bot
    {
        int indexBot = joueurActif - nombreJoueurs;
        if (carteValide(joueursBot[indexBot].main[indiceCarte]))
        {

            talon.push(joueursBot[indexBot].main[indiceCarte]); // On pousse la carte que le joueur voulait jouer.
            joueursBot[indexBot].main.erase(joueursBot[indexBot].main.begin() + indiceCarte);
            cout << "La nouvelle carte du talon est : " << talon.back().getValeur() << " et sa couleur est : " << (talon.back()).getCouleur() << endl;
            if (testUno() == false) // Si on est pas dans le cas du Uno
            {
                int newIndice = -1;
                // gestion des cartes spéciales
                switch ((talon.back()).getValeur())
                {
                case 10: // inverse
                    if (sensJeu == 1)
                        sensJeu = 0;
                    else
                        sensJeu = 1;
                    break;
                case 11: // passe
                    if (joueurActif == nombreJoueurs + nombreIA - 1 && sensJeu == 1) // Si On passe le tour du dernier joueur on revient au premier.
                        joueurActif = 0;
                    else if (joueurActif == nombreJoueurs + nombreIA - 1 && sensJeu == 0)
                    {
                        joueurActif--;
                    }
                    else if (joueurActif == 0 && sensJeu == 0)
                        joueurActif = nombreIA + nombreJoueurs - 1;
                    joueurActif++;

                    termineTour();
                    break;
                case 12: // +2
                    termineTour();
                    // on augmente le compteur de 2 pour les cartes à piocher
                    casPart += 2;
                    termineTour();
                    break;
                case 13:
                    joueursBot[indexBot].setCartePlus4(newIndice);
                    // on augmente le compteur de 2 pour les cartes à piocher
                    casPart += 4;
                    termineTour();

                    break;
                case 14: // joker
                    joueursBot[indexBot].setCarteJoker(newIndice);
                    termineTour();
                    cout << "Bot pose joker dans poserCarte Cumul" << endl;
                    break;
                }
                termineTour();
                if (joueursBot[indexBot].main.size() == 0)
                    annonceGagnant();
                return;
            }
        }
        else
        {
            messageErreur = "Cette carte ne peut pas être déposée.";
        }
    }
    else // joueur humain
    {
        if (carteValide(joueurs[joueurActif].main[indiceCarte]))
        {                                                       // La carte qu'il veut poser est valide
            talon.push(joueurs[joueurActif].main[indiceCarte]); // On pousse la carte que le joueur voulait jouer.
            joueurs[joueurActif].main.erase(joueurs[joueurActif].main.begin() + indiceCarte);

            // On appelle la procédure qui efface le cadre de la carte et le texte.
            joueurs[joueurActif].modifMainTxt();
            // On appelle la procédure qui met à jour la carte sur laquelle on joue.
            joueurs[joueurActif].modifTalonPiocheTxt(talon, pioche);

            // gestion des cartes spéciales
            switch ((talon.back()).getValeur())
            {
            case 10: // inverse
                if (sensJeu == 1)
                    sensJeu = 0;
                else
                    sensJeu = 1;
                break;
            case 11: // passe
                if (joueurActif == nombreJoueurs + nombreIA - 1 && sensJeu == 1) // Si on passe le tour du dernier joueur on revient au premier.
                    joueurActif = 0;
                else if (joueurActif == nombreJoueurs + nombreIA - 1 && sensJeu == 0)
                {
                    joueurActif--;
                }
                else if (joueurActif == 0 && sensJeu == 0)
                    joueurActif = nombreIA + nombreJoueurs - 1;
                joueurActif++;

                termineTour();
                break;
            case 12:
                casPart += 2;

                cout << casPart << "après +2" << endl;
                break;
            case 13:
                casPart += 4;
                cout << casPart << "après +4" << endl;

                break;
            case 14:
                break;
            }
            termineTour();
        }
        else
        {

            messageErreur = "Cette carte ne peut pas être déposée.";
            // Voir si on ajoute d'autre message.
            cout << messageErreur << endl;
        }
    }
}

// Fait piocher une ou des cartes selon le compteur
void VarianteCumul::piocherCarte()
{
    if (casPart == 0) // cas classique, 1 carte à piocher
    {
        if (joueurActif >= nombreJoueurs) // bot
        {
            int indexBot = joueurActif - nombreJoueurs;
            unsigned int indCarte;
            joueursBot[indexBot].main.push_back(pioche.top());
            switch (pioche.top().getCouleur())
            {
            case 1:
                joueursBot[indexBot].setCarteRouge();

                if (pioche.top().getValeur() == 14) // On ajoute l'indice du joker.
                {
                    indCarte = joueursBot[indexBot].getCarteRouge() - 1;
                    joueursBot[indexBot].setCarteJoker(indCarte);
                }

                else if (pioche.top().getValeur() == 13)
                {
                    indCarte = joueursBot[indexBot].getCarteRouge() - 1;
                    joueursBot[indexBot].setCartePlus4(indCarte);
                }
                break;
            case 2:
                joueursBot[indexBot].setCarteVert();

                if (pioche.top().getValeur() == 14) // On ajoute l'indice du joker.
                {
                    indCarte = joueursBot[indexBot].getCarteVert() - 1;
                    joueursBot[indexBot].setCarteJoker(indCarte);
                }

                else if (pioche.top().getValeur() == 13)
                {
                    indCarte = joueursBot[indexBot].getCarteVert() - 1;
                    joueursBot[indexBot].setCartePlus4(indCarte);
                }
                break;
            case 3:
                joueursBot[indexBot].setCarteBleu();

                if (pioche.top().getValeur() == 14) // On ajoute l'indice du joker.
                {
                    indCarte = joueursBot[indexBot].getCarteBleu() - 1;
                    joueursBot[indexBot].setCarteJoker(indCarte);
                }

                else if (pioche.top().getValeur() == 13)
                {
                    indCarte = joueursBot[indexBot].getCarteBleu() - 1;
                    joueursBot[indexBot].setCartePlus4(indCarte);
                }
                break;
            case 4:
                joueursBot[indexBot].setCarteJaune();

                if (pioche.top().getValeur() == 14) // On ajoute l'indice du joker.
                {
                    indCarte = joueursBot[indexBot].getCarteJaune() - 1;
                    joueursBot[indexBot].setCarteJoker(indCarte);
                }

                else if (pioche.top().getValeur() == 13)
                {
                    indCarte = joueursBot[indexBot].getCarteJaune() - 1;
                    joueursBot[indexBot].setCartePlus4(indCarte);
                }
                break;

            default:
                break;
            }
            pioche.pop();
            joueursBot[indexBot].trierMain();
            return;
        }
        else // Joueur normal.
        {
            joueurs[joueurActif].main.push_back(pioche.top());
            pioche.pop();
            joueurs[joueurActif].modifMainTxt();
            joueurs[joueurActif].modifTalonPiocheTxt(talon, pioche);
        }
    }
    else // cas après des +2 et +4
    {
        if (joueurActif >= nombreJoueurs) // bot
        {
            int indexBot = joueurActif - nombreJoueurs;
            unsigned int indCarte;
            for (int i = 0; i < casPart; i++)
            {
                joueursBot[indexBot].main.push_back(pioche.top());
                switch (pioche.top().getCouleur())
                {
                case 1:
                    joueursBot[indexBot].setCarteRouge();

                    if (pioche.top().getValeur() == 14) // On ajoute l'indice du joker.
                    {
                        indCarte = joueursBot[indexBot].getCarteRouge() - 1;
                        joueursBot[indexBot].setCarteJoker(indCarte);
                    }

                    else if (pioche.top().getValeur() == 13)
                    {
                        indCarte = joueursBot[indexBot].getCarteRouge() - 1;
                        joueursBot[indexBot].setCartePlus4(indCarte);
                    }
                    break;
                case 2:
                    joueursBot[indexBot].setCarteVert();

                    if (pioche.top().getValeur() == 14) // On ajoute l'indice du joker.
                    {
                        indCarte = joueursBot[indexBot].getCarteVert() - 1;
                        joueursBot[indexBot].setCarteJoker(indCarte);
                    }

                    else if (pioche.top().getValeur() == 13)
                    {
                        indCarte = joueursBot[indexBot].getCarteVert() - 1;
                        joueursBot[indexBot].setCartePlus4(indCarte);
                    }
                    break;
                case 3:
                    joueursBot[indexBot].setCarteBleu();

                    if (pioche.top().getValeur() == 14) // On ajoute l'indice du joker.
                    {
                        indCarte = joueursBot[indexBot].getCarteBleu() - 1;
                        joueursBot[indexBot].setCarteJoker(indCarte);
                    }

                    else if (pioche.top().getValeur() == 13)
                    {
                        indCarte = joueursBot[indexBot].getCarteBleu() - 1;
                        joueursBot[indexBot].setCartePlus4(indCarte);
                    }
                    break;
                case 4:
                    joueursBot[indexBot].setCarteJaune();

                    if (pioche.top().getValeur() == 14) // On ajoute l'indice du joker.
                    {
                        indCarte = joueursBot[indexBot].getCarteJaune() - 1;
                        joueursBot[indexBot].setCarteJoker(indCarte);
                    }

                    else if (pioche.top().getValeur() == 13)
                    {
                        indCarte = joueursBot[indexBot].getCarteJaune() - 1;
                        joueursBot[indexBot].setCartePlus4(indCarte);
                    }
                    break;

                default:
                    break;
                }
                pioche.pop();
                joueursBot[indexBot].trierMain();
            }
            casPart = 0;
        }
        else // humain
        {
            for (int i = 0; i < casPart; i++)
            {
                joueurs[joueurActif].main.push_back(pioche.top());
                pioche.pop();
            }
            casPart = 0;
            joueurs[joueurActif].modifMainTxt();
            joueurs[joueurActif].modifTalonPiocheTxt(talon, pioche);
        }
    }
}