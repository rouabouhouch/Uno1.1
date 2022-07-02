#include "VarianteEchange.h"

VarianteEchange::VarianteEchange() : Jeu() {}

VarianteEchange::VarianteEchange(const unsigned int nbjoueurs, const unsigned int nbIA = 0) : Jeu(nbjoueurs, nbIA) {}

VarianteEchange::~VarianteEchange()
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

void VarianteEchange::poserCarte(const unsigned int &indiceCarte, string &messageErreur)
{
    cout << "POSER CARTE ECHANGE" << endl;
    if (joueurActif >= nombreJoueurs) // bot
    {
        int indexBot = joueurActif - nombreJoueurs;
        if (carteValide(joueursBot[indexBot].main[indiceCarte]))
        {                                                       // La carte qu'il veut poser est valide
            talon.push(joueursBot[indexBot].main[indiceCarte]); // On pousse la carte que le joueur voulait jouer.
            joueursBot[indexBot].main.erase(joueursBot[indexBot].main.begin() + indiceCarte);
            // si la carte posée est un 7, le joueur échange sa main avec un autre joueur
            if (talon.back().getValeur() == 7)
            {
                echangeJeuVersionBot();
            }
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
                case 14: // joker
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
            testUno();

            // si la carte posée est un 7, le joueur échangera sa main avec un autre joueur
            if (talon.back().getValeur() == 7)
            {
                casPart = 3;
            }
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
            if (casPart != 3) termineTour();
        }
        else
        {

            messageErreur = "Cette carte ne peut pas être déposée.";
            // Voir si on ajoute d'autre message.
            cout << messageErreur << endl;
        }
    }
}

// echange la main du bot avec un autre joueur
void VarianteEchange::echangeJeuVersionBot()
{

    unsigned int indexBot = joueurActif - nombreJoueurs;
    unsigned int nombreJoueurMax = nombreJoueurs + nombreIA;
    // On génère un indice entre 0 et le nombre total de joueur + IA.
    unsigned int numJoueur = rand() % nombreJoueurMax;
    cout << "Le bot va échanger son Jeu avec le joueur " << numJoueur << endl;
    assert(numJoueur < nombreJoueurMax);
    if (numJoueur == joueurActif)
    { // On teste quand même si on est pas avec le même indice que le bot qui joue.
        numJoueur = rand() % nombreJoueurMax;
    }
    if (numJoueur >= nombreJoueurs && numJoueur != joueurActif)
    { // Si le nombre est un indice d'un bot alors.
        cout << "Echange entre Bot "<< endl;
        // On copie les données du bot dans l'autre :
        joueursBot[indexBot].copieNbCarte(joueursBot[numJoueur]);
        // On échange les mains.
        joueursBot[indexBot].main.swap(joueursBot[numJoueur].main);
    }
    else if (numJoueur < nombreJoueurs)
    {
        cout << "Echange entre bot et Joueur" << endl;
        // On remet à 0 les données du bot.
        joueursBot[indexBot].remetNbCarteZero();
        // On met les nouveaux nombres de carte dans le joueursBot qui va recevoir les cartes du joueur.
        for (unsigned int j = 0; j < joueurs[numJoueur].main.size(); j++)
        {
            definieCouleurBot(joueursBot[indexBot], joueurs[numJoueur].main[j]);
        }
        // On échange la main du bot avec un Joueur.
        joueursBot[indexBot].main.swap(joueurs[numJoueur].main);
    }
}