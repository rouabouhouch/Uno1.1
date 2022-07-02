#ifndef MENU_H
#define MENU_H

#include <string.h>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

/**
* @brief
* Classe gérant le menu et les choix d'options.
* 
*/
class Menu
{
private:
	/**
	 * @brief 
	 * Vecteur contenant (Jouer,Règlages,Quitter,Logs,Règles).
	 */
	vector<string> nomMenu;
	/**
	 * @brief 
	 * Vecteur de string contenant (Résolution, Changer Pseudo, Son).
	 */
	vector<string> nomOptions;
	/**
	 * @brief 
	 * Vecteur de string contenant (1920 x 1080, 720 x 576, 1280 x 720) pour régler la résolution de la fenêtre.
	 */
	vector<string> resolutionOptions;
	/**
	 * @brief 
	 * Vecteur de string contenant les entier des 0 à 10 pour le volume du Jeu.
	 */
	vector<string> sonOptions;		   

public:
	Menu();
	~Menu();

	/**
	 * @brief 
	 * Initialise le vecteur nomMenu avec les mots cités plus Haut.
	 */
	void initNomMenu();
	/**
	 * @brief 
	 * Initialise le vecteur nomOptions avec les mots cités plus Haut.
	 */
	void initNomOptions();
	/**
	 * @brief 
	 * Initialise le vecteur ResolutionOptions avec les mots cités plus Haut.
	 */
	void initResolutionOptions();
	/**
	 * @brief 
	 * Initialise le vecteur SonOptions avec les mots cités plus Haut.
	 */
	void initSonOptions();

	// Accesseur.
	string getNomMenu(int i);
	string getOptions(int i);
	string getResolution(int i);
	string getSon(int i);

	void testRegression();

};

#endif
