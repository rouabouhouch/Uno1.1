#include "VarianteDoublon.h"

VarianteDoublon::VarianteDoublon() : Jeu() {}

VarianteDoublon::VarianteDoublon(const unsigned int nbjoueurs, const unsigned int nbIA = 0) : Jeu(nbjoueurs, nbIA) {}

VarianteDoublon::~VarianteDoublon()
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

void VarianteDoublon::poserCarte(const unsigned int &indiceCarte, string &messageErreur)
{
    assert(indiceCarte >= 0);
    if (joueurActif >= nombreJoueurs) // bot
    {
        int indexBot = joueurActif - nombreJoueurs;
        if (carteValide(joueursBot[indexBot].main[indiceCarte]))
        {                                                       // La carte qu'il veut poser est valide
            talon.push(joueursBot[indexBot].main[indiceCarte]); // On pousse la carte que le bot voulait jouer.
            joueursBot[indexBot].main.erase(joueursBot[indexBot].main.begin() + indiceCarte);
            // si le bot a le doublon de la carte qu'il vient de jouer, on lui propose de la jouer
            int indice = indiceDoublon(talon.back(), joueursBot[indexBot].main);
            if (indice >= 0)
            {
                talon.push(joueursBot[indexBot].main[indice]); // On pousse le doublon.
                joueursBot[indexBot].main.erase(joueursBot[indexBot].main.begin() + indice);
            }

            // gestion des cartes spéciales
            if (!testUno()) // Si on est pas dans le cas du Uno
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

                    piocherCarte();
                    piocherCarte();
                    termineTour();
                    break;
                case 13:
                    joueursBot[indexBot].setCartePlus4(newIndice);
                    termineTour();

                    for (unsigned int i = 0; i < 4; i++)
                        piocherCarte();
                    //termineTour();

                    break;
                case 14: // joker
                    joueursBot[indexBot].setCarteJoker(newIndice);
                    termineTour();

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
            // Voir si on ajoute d'autre message.
            cout << messageErreur << endl;
        }
    }
    else // humain
    {
        if (carteValide(joueurs[joueurActif].main[indiceCarte]))
        {                                                       // La carte qu'il veut poser est valide
            talon.push(joueurs[joueurActif].main[indiceCarte]); // On pousse la carte que le joueur voulait jouer.
            joueurs[joueurActif].main.erase(joueurs[joueurActif].main.begin() + indiceCarte);
            // si le joueur a le doublon de la carte qu'il vient de jouer, on lui propose de la jouer
            testUno();
            casPart = 1;
            
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
                termineTour();
                break;
            case 12: // +2
                termineTour();

                piocherCarte();
                piocherCarte();
                break;
            case 13: // +4
                termineTour();

                for (unsigned int i = 0; i < 4; i++)
                    piocherCarte();
                break;
            case 14: // joker
                break;
            }
            if(casPart != 1) termineTour();
        }
        else
        {
            messageErreur = "Cette carte ne peut pas être déposée.";
            // Voir si on ajoute d'autre message.
            cout << messageErreur << endl;
        }
    }
}

// retourne l'indice dans m du doublon de la carte c, -1 sinon
int VarianteDoublon::indiceDoublon(Carte c, vector<Carte> m)
{
    for (unsigned int i = 0; i < m.size(); i++)
    {
        if ((c.getValeur() == 13 && m[i].getValeur() == 13) || (c.getValeur() == 14 && m[i].getValeur() == 14)) //cas joker et +4, pas besoin de tester la couleur
        {
            return i;
        }
        else if (c.getValeur() == m[i].getValeur() && c.getCouleur() == m[i].getCouleur()) // pour les autres cartes, on teste aussi la couleur
        {
            return i;
        }
    }
    return -1;
}