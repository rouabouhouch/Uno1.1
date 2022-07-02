#include "VarianteTourne.h"

VarianteTourne::VarianteTourne() : Jeu() {}

VarianteTourne::VarianteTourne(const unsigned int nbjoueurs, const unsigned int nbIA = 0) : Jeu(nbjoueurs, nbIA) {}

VarianteTourne::~VarianteTourne()
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

void VarianteTourne::poserCarte(const unsigned int &indiceCarte, string &messageErreur)
{
    if (joueurActif >= nombreJoueurs) // bot
    {
        int indexBot = joueurActif - nombreJoueurs;
        if (carteValide(joueursBot[indexBot].main[indiceCarte]))
        {                                                       // La carte qu'il veut poser est valide
            talon.push(joueursBot[indexBot].main[indiceCarte]); // On pousse la carte que le joueur voulait jouer.
            joueursBot[indexBot].main.erase(joueursBot[indexBot].main.begin() + indiceCarte);
            // si la carte posée est 0, on fait tourner les mains
            if (talon.back().getValeur() == 0)
                tournerMains();
            bool carteSpeciale = false;
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

                    carteSpeciale = true;
                    termineTour();
                case 12: // +2
                    termineTour();

                    piocherCarte();
                    piocherCarte();
                    carteSpeciale = true;
                    termineTour();
                    break;
                case 13: // +4
                    carteSpeciale = true;
                    joueursBot[indexBot].setCartePlus4(newIndice);
                    termineTour();

                    for (unsigned int i = 0; i < 4; i++)
                        piocherCarte();
                    termineTour();

                    break;
                case 14:// joker
                    joueursBot[indexBot].setCarteJoker(newIndice);
                    termineTour();
                    carteSpeciale = true;

                    break;
                }
                if (!carteSpeciale)
                    termineTour();
                if (joueursBot[indexBot].main.size() == 0)
                    annonceGagnant();
                return;
            }
            else
            {

                messageErreur = "Cette carte ne peut pas être déposée.";
                // Voir si on ajoute d'autre message.
                cout << messageErreur << endl;
            }
        }
    }
    else // humain
    {
        if (carteValide(joueurs[joueurActif].main[indiceCarte]))
        {                                                       // La carte qu'il veut poser est valide
            talon.push(joueurs[joueurActif].main[indiceCarte]); // On pousse la carte que le joueur voulait jouer.
            joueurs[joueurActif].main.erase(joueurs[joueurActif].main.begin() + indiceCarte);
            // si la carte posée est 0, on fait tourner les mains
            if (talon.back().getValeur() == 0)
                tournerMains();
            // On appelle la fonction/Procédure qui efface le cadre de la carte et le texte.
            joueurs[joueurActif].modifMainTxt();
            // On appelle la F°/Proc qui met à jour la carte sur laquelle on joue.
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

// Fait tourner les mains des joueurs
void VarianteTourne::tournerMains()
{
    if (sensJeu == 1)
    {
        for (unsigned int i = nombreJoueurs + nombreIA - 1; i > 0; i--)
        {
            if (i > nombreJoueurs)
            { // On échange les jeux entre les bots

                // On copie les données du bot dans l'autre :
                joueursBot[i - nombreJoueurs].copieNbCarte(joueursBot[(i - nombreJoueurs) - 1]);
                // On échange les mains.
                joueursBot[i - nombreJoueurs].main.swap(joueursBot[(i - nombreJoueurs) - 1].main);
            }
            else if (i == nombreJoueurs)
            { // On échange les jeux entre un bot et un joueur

                // On remet à 0 les données du bot.
                joueursBot[(i - nombreJoueurs)].remetNbCarteZero();
                // On met les nouveaux nombres de carte dans le joueursBot qui va recevoir les cartes du joueur.
                for (unsigned int j = 0; j < joueurs[i - 1].main.size(); j++)
                {
                    definieCouleurBot(joueursBot[(i - nombreJoueurs)], joueurs[i - 1].main[j]);
                }
                // On peut échanger les mains.
                joueursBot[i - nombreJoueurs].main.swap(joueurs[i - 1].main);
            }
            else if (i < nombreJoueurs)
            { // On échange les jeux entre Joueurs
                joueurs[i].main.swap(joueurs[i - 1].main);
            }
        }
    }
    else // Sens du jeu = 0 donc on inverse vers la gauche.
    {
        for (unsigned int i = 0; i < nombreJoueurs + nombreIA - 2; i++)
        {
            if (i > nombreJoueurs - 1) // Si on passe sur un indice qui n'est plus valide pour joueurs
            {
                // On copie les données du bot dans l'autre :
                joueursBot[i - nombreJoueurs].copieNbCarte(joueursBot[(i - nombreJoueurs) + 1]);
                // On échange les mains.
                joueursBot[i - nombreJoueurs].main.swap(joueursBot[(i - nombreJoueurs) + 1].main);
            }
            else if (i == nombreJoueurs - 1)
            {
                // On remet à 0 les données du bot.
                joueursBot[(i - nombreJoueurs) + 1].remetNbCarteZero();
                // On met les nouveaux nombres de carte dans le joueursBot qui va recevoir les cartes du joueur.
                for (unsigned int j = 0; j < joueurs[i].main.size(); j++)
                {
                    definieCouleurBot(joueursBot[(i - nombreJoueurs) + 1], joueurs[i].main[j]);
                }
                // On échange les mains entre joueur et Bot.
                joueurs[i].main.swap(joueursBot[(i - nombreJoueurs) + 1].main);
            }
            else if (i < nombreJoueurs)
            {
                joueurs[i].main.swap(joueurs[i + 1].main);
            }
        }
    }
}