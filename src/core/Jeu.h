#ifndef JEU_H
#define JEU_H

#include <stack> // Pour les piles.
#include <queue>
#include <Joueur.h>
#include <Carte.h>
#include <Bot.h>
#include <iostream>
#include <vector>
#include <set>   // utile dans initCarte
#include <cassert>
#include <algorithm>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <chrono>

using namespace std;

class Bot;

/**
* @brief 
* Classe gérant les actions du jeu.
*/
class Jeu
{
private:
public:
	/**
	* @brief 
	* Pile du jeu de cartes.
	*/
	stack<Carte> pioche;

	/**
	* @brief 
	* Talon du jeu de cartes sur lequel les joueurs joueront leurs cartes.
	*   
	*/
	queue<Carte> talon;

	/**
	* @brief 
	* Tableau de Joueurs.
	*/
	Joueur *joueurs;

	/**
	 * @brief 
	 * Tableau de Joueurs.
	 */
	Bot *joueursBot;

	/**
	* @brief 
	* Nombre de Joueurs dans la partie.
	*/
	unsigned int nombreJoueurs;

	/**
	* @brief 
	* Nombre d'ordinateurs dans la partie.
	*/
	unsigned int nombreIA;

	/**
	* @brief 
	* 0 ou 1 :
	* 0 on joue vers la droite.
	* 1 on joue vers la gauche.
	*/
	unsigned int sensJeu;

	/**
	* @brief 
	* Numéro du joueur qui est en train de jouer.
	*/
	unsigned int joueurActif;

	/**
	* @brief 
	* Indique le numéro de la variante si le jeu se trouve dans un cas particulier.
	* -1 classique, 1 doublon, 3 echange, 5 suite, 0 ou smultiple de 2 cumul
	*/
	int casPart;

	/**
	* @brief 
	* Booléen qui permettra de dire que la partie est terminée.
	*/
	bool finPartie;

	/**
	* @brief 
	* Booléen qui permet de signifier que c'est la fin du tour.
	* true : tour terminé on déclenche termineTour().
	* false : tour pas terminé.
	*/
	bool finTour;

	/**
	* @brief 
	* Variable qui permettra faire une boucle tant que
	* qui attendra une touche d'un des joueurs C ou U pour pouvoir relancer le Jeu.
	*/
	bool statut_Uno;

	// =========================================== FONCTION / Procédure ===============================================================

	/**
	* @brief 
	* Constructeur par défaut.
	* 
	*/
	Jeu();

	/**
	* @brief 
	* Destructeur par défaut.
	* 
	*/
	~Jeu();

	/**
	* @brief Construit un objet Jeu avec un tableau de joueurs en paramètre 
	* Constructeur: initialise le nombre de joueurs, le tableau de joueurs, distribue les cartes, complète la table de chaque joueur
	* 
	* @param 
	* [in] entier naturel: nombre de joueurs 
	* @param  
	* [in] entier naturel: nombre de joueurs 
	*
	*/
	Jeu(const unsigned int nbjoueurs, const unsigned int nbIA);

	/**
	* @brief 
	* Initialise la pioche.
	*/
	void initCarte();

	/**
	* @brief 
	* Initialise le Talon à partir de la pioche.
	*/
	void initTalon();

	/**
	* @brief 
	* Distribue les 7 cartes nécessaires à chaque joueur au début du jeu.
	*/
	void distribueCarte();

	/**
	 * @brief 
	 * 
	 * @param tabBot : Tableau qui servira à déréference le pointeur joueursBot 
	 * @param c : Une carte que l'on regardera pour ajouter une carte rouge.
	 */
	void definieCouleurBot(Bot &tabBot, Carte &c);
	/**
	* @brief 
	* Vérifie si la carte qui va être posée est valide ou non.
	* 
	* @param 
	* [in] carte à comparer avec le talon
	*
	* @return
	* true si la carte est valide.
	*/
	virtual bool carteValide(const Carte) const;

	/**
	* @brief 
	* On met dans la main du joueur une carte provenant de la pioche.
	*/
	virtual void piocherCarte();

	/**
	* @brief 
	* Le joueur met une carte dans le Talon et on enlève la carte de la main du joueur.
	*
	* @param 
	* [in] entier naturel: indice, dans la main, de la carte que le joueur veut poser.
	* @param 
	* messageErreur : Message d'erreur à afficher avec l'interface Graphique.
	*/
	virtual void poserCarte(const unsigned int &indiceCarte, string &messageErreur);

	/**
	* @brief 
	* Actions clavier du joueur.
	*
	* @param 
	* [in] caractère: touche qui a été appuyée.
	*/
	void actionJoueur(const char action);

	/**
	* @brief 
	* Vérifie que la pioche est vide.
	*
	* @return 
	* Booléen: true si la pioche est vide, false sinon.
	*
	*/
	bool piocheVide();

	/**
	* @brief 
	* Si la pioche est vide, réinitialisation avec le talon.
	*	
	*/
	void relancePiocheJeu();

	/**
	* @brief 
	* Teste si le joueur actif est en situation de dire Uno
	*
	* @return 
	* Booléen: true s'il reste 1 seule carte au joueur, false sinon. 
	* 
	*/
	bool testUno();

	/**
	* @brief 
	* Le joueur a appuyé avant sur ESPACE ce qui provoque le UNO,
	* on termine le tour et les autres joueurs ne peuvent plus dire contre Uno.
	* @param c : caractère entrée dans la boucle Jeu de Affichage Txt.
	*/
	void Uno(int c);

	/**
	* @brief 
	* Les joueurs ont cliqués sur CONTRE UNO avant le joueur concerné,
	* le joueur qui n'a pas appuyé doit donc piocher 2 cartes et le tour se termine.
	*
	*/
	void contreUno();

	/**
	* @brief 
	* Change le joueur actif, et termine le tour.
	* 
	*/
	void termineTour();

	/**
	 * @brief 
	 * Annonce le gagnant de la partie.
	 */
	void annonceGagnant();

	/**
	* @brief 
	* Actualise le nombre de cartes des  adversaires dans la table des joueurs pour la version txt.
	* 
	*/
	void modifAdversairesTxt();

	/**
	* @brief 
	* Actualise la table du joueur actif au début de son tour. 
	*
	*/
	void MaJTableJoueurActifDebutTour();

	/**
	* @brief 
	* Teste les fonctions et procédures du module. 
	*
	*/
	void testRegression();
};

#endif
