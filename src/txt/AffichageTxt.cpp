#include <iostream>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // WIN32
#include "AffichageTxt.h"

#include "Jeu.h"

// change la couleur de la carte quand +4 ou joker
void changeCouleurCarte(Jeu &jeu, Fenetre win)
{
    //int indiceCarte = jeu.joueurs[jeu.joueurActif].indiceEtoile;
    //unsigned int numeroCarte = jeu.joueurs[jeu.joueurActif].main[indiceCarte].getValeur();
    int c;

    bool choixCouleur = true;
    cout << "### Vous devez choisir une nouvelle couleur sur laquelle vous jouerez ###" << endl;
    while (choixCouleur)
    {
        c = win.getCh();
        switch (c)
        {
        case 'r': // choix rouge
            jeu.actionJoueur('r');
            choixCouleur = false;
            break;
        case 'v': // choix vert
            jeu.actionJoueur('v');
            choixCouleur = false;
            break;

        case 'b': // choix bleu
            jeu.actionJoueur('b');
            choixCouleur = false;
            break;

        case 'j': // choix jaune
            jeu.actionJoueur('j');
            choixCouleur = false;
            break;
        }
    }
}

void txtAff(Fenetre &win, const Jeu &jeu)
{
    if (jeu.joueurActif >= jeu.nombreJoueurs)
    {
        cout << jeu.joueurActif << endl;
        return;
    }
    else
    {
        const Joueur &ter = jeu.joueurs[jeu.joueurActif];

        win.clear();

        // Affichage de la pioche, talon, passer ...
        for (unsigned int x = 0; x < ter.getHaut(); ++x)
            for (unsigned int y = 0; y < ter.getLarg(); ++y)
                win.prepaFenetre(x, y, ter.getXY(x, y));

        win.dessine();
    }
}

// affiche la salle d'attente sur le terminal
void txtAffSalleAttente(Fenetre &winSA, const SalleAttente &s)
{
    winSA.clear();
    for (unsigned int x = 0; x < s.haut; x++)
        for (unsigned int y = 0; y < s.larg; y++)
            winSA.prepaFenetre(x, y, s.salle[x][y]);
    winSA.dessine();
}

// boucle à partir de la salle d'attente
int txtBoucleDebut(SalleAttente &s)
{
    Fenetre winSA(38, 180);
    // on affiche la salle d'attente
    txtAffSalleAttente(winSA, s);
    int c;
    bool finPartie = false;
    while (!finPartie)
    {
        c = winSA.getCh();
        switch (c)
        {
        // l'étoile monte
        case 'q':
            finPartie = true;
            break;
        case 't':
            if (s.etoile > 11)
            {
                s.salle[s.etoile][49] = ' ';
                s.etoile--;
                s.MaJFenetreSalle();
                txtAffSalleAttente(winSA, s);
            }
            break;
        // l'étoile descend
        case 'b':
            if (s.etoile < 14)
            {
                s.salle[s.etoile][49] = ' ';
                s.etoile++;
                s.MaJFenetreSalle();
                txtAffSalleAttente(winSA, s);
            }
            break;
        // on valide le choix
        case 'e':
            switch (s.etoile)
            {
            // choix du jeu
            case 11:
                s.fenetreSalleRegles();
                txtAffSalleAttente(winSA, s);
                s.choixJeu();
                txtAffSalleAttente(winSA, s);
                break;
            // choix du nombre de joueurs
            case 12:
                s.choixNombreJoueurs();
                txtAffSalleAttente(winSA, s);
                break;
            // choix du nombre d'IA
            case 13:
                s.choixNombreIA();
                txtAffSalleAttente(winSA, s);
                break;
            // on lance la partie
            case 14:
                if (s.nombreJoueurs + s.nombreIA > 1) // il faut au moins 2 joueurs
                    // création du jeu selon le choix et lancement de la partie
                    return s.variante;
                else
                    cout << "Pas assez de joueurs pour commencer la partie." << endl;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    return 0;
}

// boucle de jeu
void txtBoucle(Jeu &jeu) //
{
    // Creation d'une nouvelle fenetre en mode texte
    // => fenetre de dimension et position (WIDTH,HEIGHT,STARTX,STARTY)
    Fenetre win(38, 180);
    bool ok = true;
    char touche;
    int c;
    int c2;
    
    unsigned int indiceJoueur; // utile dans le cas de la variante cumul

    do
    {
        txtAff(win, jeu); // On initialise le jeu avec les éléments principaux.
        
#ifdef _WIN32
        Sleep(100);
#else
        usleep(100000);
#endif // WIN32
        jeu.finTour = false;
        if (jeu.joueurActif < jeu.nombreJoueurs)
            jeu.joueurs[jeu.joueurActif].indiceEtoile = 0;
        jeu.MaJTableJoueurActifDebutTour(); // Modif rendu main joueur, adversaire et talon.
        jeu.annonceGagnant();               // Annonce le gagnant de la partie.
        
        if (jeu.finPartie == true)
            return;
        while (jeu.finTour == false || jeu.finPartie == false) // Tant que l'on a pas terminé le tour.
        {
            indiceJoueur = jeu.joueurActif;
            c = win.getCh();  // On récupère le caractère de la touche appuyé et on le met dans c.
            printf("%s", &touche);
            printf(&touche, win);
            
            switch (c)
            {
                case 'a':
                    jeu.actionJoueur('a');
                    break;
                case 'd':
                    jeu.actionJoueur('d');
                    break;
                case 'p':
                    jeu.actionJoueur('p');
                    // cas de la variante cumul
                    if (jeu.casPart % 2 == 0 && jeu.casPart > 0 && jeu.talon.back().getValeur() == 13)
                        {
                            int jActifTmp = jeu.joueurActif;
                            jeu.joueurActif = indiceJoueur; // on revient au joueur précédent pour qu'il coisisse la couleur
                            jeu.MaJTableJoueurActifDebutTour();
                            txtAff(win, jeu);
                            changeCouleurCarte(jeu, win);
                            jeu.joueurActif = jActifTmp;
                        }
                    if (jeu.casPart % 2 == 0) // variante cumul
                    {
                        jeu.casPart = 0;
                        indiceJoueur = jeu.joueurActif;
                    }
                    else jeu.casPart = -1;
                    jeu.termineTour();
                    break;
                case 'e':
                    {int indiceCarte = jeu.joueurs[jeu.joueurActif].indiceEtoile;
                    if ((jeu.joueurs[jeu.joueurActif].main[indiceCarte].getValeur() == 13 && jeu.casPart % 2 != 0) || jeu.joueurs[jeu.joueurActif].main[indiceCarte].getValeur() == 14)
                    {
                        changeCouleurCarte(jeu, win);
                    }
                    jeu.actionJoueur('e');
                    if (jeu.casPart == 5) // variante suite
                    {
                        char reponse = 'O';
                        //int carteTalon = jeu.talon.back().getValeur();

                        while (reponse == 'O' && jeu.talon.back().getValeur() < 7)
                        {
                            cout << "Veux-tu jouer la carte suivante? saisir O/N puis entrer";
                            cin >> reponse;
                            cout << endl;
                            if (reponse == 'O')
                            {
                                jeu.talon.push(jeu.joueurs[jeu.joueurActif].main[jeu.joueurs[jeu.joueurActif].indiceEtoile]); 
                                jeu.joueurs[jeu.joueurActif].main.erase(jeu.joueurs[jeu.joueurActif].main.begin() + jeu.joueurs[jeu.joueurActif].indiceEtoile);
                            }
                        }
                        jeu.casPart = -1;
                        jeu.termineTour();
                    }
                    if (jeu.casPart == 3) // variante echange
                    {
                        unsigned int numJoueur;
                        cout << "indique le numéro du joueur avec lequel tu veux échanger ta main: ";
                        cin >> numJoueur;
                        jeu.joueurs[jeu.joueurActif].main.swap(jeu.joueurs[numJoueur - 1].main);
                        jeu.joueurs[jeu.joueurActif].modifMainTxt();
                        jeu.joueurs[numJoueur-1].modifMainTxt();
                        jeu.termineTour();
                    }
                    if (jeu.casPart == 1) // variante doublon
                    {
                        int indice = -1;
                        for (unsigned int i = 0; i < jeu.joueurs[jeu.joueurActif].main.size(); i++)
                        {
                            if ((jeu.talon.back().getValeur() == 13 && jeu.joueurs[jeu.joueurActif].main[i].getValeur() == 13) || (jeu.talon.back().getValeur() == 14 && jeu.joueurs[jeu.joueurActif].main[i].getValeur() == 14)) //cas joker et +4, pas besoin de tester la couleur
                            {
                                indice = i;
                            }
                            else if (jeu.talon.back().getValeur() == jeu.joueurs[jeu.joueurActif].main[i].getValeur() && jeu.talon.back().getCouleur() == jeu.joueurs[jeu.joueurActif].main[i].getCouleur())
                            {
                                indice = i;
                            }
                        }
                        if (indice >= 0) 
                        {
                            char reponse;
                            cout << "Veux-tu jouer la carte doublon? O/N" << endl;
                            cin >> reponse;
                            if (reponse == 'O')
                            {
                                jeu.talon.push(jeu.joueurs[jeu.joueurActif].main[indice]); // On pousse le doublon.
                                jeu.joueurs[jeu.joueurActif].main.erase(jeu.joueurs[jeu.joueurActif].main.begin() + indice);
                            }
                        }
                        jeu.termineTour();
                    }
                    break;}
                case 'q':
                    jeu.finTour = true;
                    ok = false;
                    jeu.finPartie = true;
                    break;
            }
            
            jeu.MaJTableJoueurActifDebutTour(); // Modif rendu main joueur, adversaire et talon.
            txtAff(win, jeu);

            if (jeu.joueurActif >= jeu.nombreJoueurs)
            {
                sleep(1);
                // Appelle choix joueur.
                int indexBot = jeu.joueurActif - jeu.nombreJoueurs;
                jeu.joueursBot[indexBot].choixJeu(jeu);
            }
            if (jeu.statut_Uno)
            {
                cout << "========== Un des joueurs peut jouer UNO !!! ===============" << endl;
                sleep(1.0);
                c2 = win.getCh(); // On récupère le caractère de la touche appuyé et on le met dans c.
                while (jeu.statut_Uno)
                {
                    sleep(1);
                    if (c2 == 'u' && jeu.joueurActif < jeu.nombreJoueurs)
                    { // Si le joueur humain appuie sur U pour uno, avant le sleep alors il continue de jouer.
                        jeu.Uno('u');
                    }
                    else if (c2 == 'c' && jeu.joueurActif >= jeu.nombreJoueurs)
                    { // Si le joueur Humain fait un contre Uno alors que le bot est en situation de Uno, le bot pioche.
                        jeu.Uno(c);
                    }
                    else if (jeu.joueurActif >= jeu.nombreJoueurs && (c2 != 'c' || c2 != 'u'))
                    {
                        jeu.Uno('u');
                    }
                    // Si le joueur Humain n'appuie pas sur une des touches après le sleep alors l'ordi fait un contre uno.
                    else if ((c2 != 'c' || c2 != 'u') && jeu.joueurActif < jeu.nombreJoueurs)
                    {
                        jeu.Uno('c');
                    }
                }
            }
        }
    } while (ok || !jeu.finPartie);
}
