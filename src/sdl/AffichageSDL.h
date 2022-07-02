#ifndef _SDLJEU_H
#define _SDLJEU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <Menu.h>
#include <fstream>
#include <sstream>
#include <cassert>
#include <time.h>
#include <stdlib.h>
#include "VarianteDoublon.h"
#include "VarianteEchange.h"
#include "VarianteTourne.h"
#include "VarianteSuite.h"
#include "VarianteCumul.h"
#include "AffichageSDL.h"
#include <iostream>

#include "Jeu.h"

using namespace std;

class Image
{

private:
	/**
 	* @brief 
 	* Structure qui permet de faire le traitement de ce que l'on affichera.
 	*/
	SDL_Surface *surface;
	/**
	 * @brief 
	 * Structure qui fait le traitement et la représentation d'une surface.
	 */
	SDL_Texture *texture;
	bool has_changed;

public:
	Image();
	void loadFromFile(const char *filename, SDL_Renderer *renderer);
	void loadFromCurrentSurface(SDL_Renderer *renderer);
	void draw(SDL_Renderer *renderer, int x, int y, int w = -1, int h = -1);
	SDL_Texture *getTexture() const;
	void setSurface(SDL_Surface *surf);
};

class sdlJeu
{

private:
	/**
	 * @brief 
	 * Structure dans laquelle on créer nos fenêtres.
	 */
	SDL_Window *window;
	/**
	 * @brief 
	 * Structure qui permet de faire le rendu de nos fenêtres.
	 */
	SDL_Renderer *renderer;

	/**
	 * @brief 
	 * Font que l'on utilise pour le texte dans la partie.
	 */
	TTF_Font *font;
	/**
	 * @brief 
	 * Font que l'on utilise essentiellement dans les parties Menu, Reglage et Salle Attente.
	 */
	TTF_Font *fontTexte;

	Image font_im;
	SDL_Color font_color;

	/**
	 * @brief 
	 * L'image que l'on charge qui fait office d'image de fond pour le Menu.
	 */
	Image fondMenu;
	/**
	 * @brief 
	 * Image de fond pour la partie Uno.
	 */
	Image fondPartie;
	/**
	 * @brief 
	 * L'image des cartes que l'on charge.
	 */
	Image im_carte;

	Mix_Chunk *sons[3]; // Tableau dans lequel on charge nos fichiers audio.

	int choixVolume;	  // Volume qui sert de référence pour la définition du volume.
	int volume;			  // Volume Sons.
	Mix_Music *selection; // son pour la sélection d'un paramètre.

	/**
	 * @brief 
	 * Pour la Resolution de la fenêtre.
	 */
	int HauteurEcran;
	/**
	 * @brief 
	 * Pour la Resolution de la fenêtre.
	 */
	int LargeurEcran;

public:
	sdlJeu();
	~sdlJeu();
	/**
	 * @brief Lis le fichier temporaire contenant la résolution de la fenêtre.
	 * 
	 */
	void lireFichierRes();
	/**
	 * @brief 
	 * Ecris la nouvelle résolution en remplaçant l'ancienne.
	 * @param largeur : Nouvelle Largeur de la fenêtre.
	 * @param hauteur : Nouvelle Hauteur de la fenêtre.
	 */
	void modifFichierRes(int largeur, int hauteur);

	/**
	 * @brief 
	 * Affiche le Menu avec tous les fonts à partir d'un objet Menu
	 * qui contient des vecteurs string dans lesquels on a mis tous les titres, résolutions ..
	 */
	void sdlMenu();

	/**
	 * @brief 
	 * Affiche la partie Regalge (Resolution, Son ....)
	 * @param m : Objet Menu qui contient les string à afficher pour la partie Reglage. 
	 */
	void sdlReglage(Menu &m);

	/**
	 * @brief 
	 * Affiche la salle d'attente avec les textes (Nombre Joueurs, Nombre Bots ...)
	 * @param variante : Variante du Jeu sur lequel on va jouer.
	 * @param nombreJoueurs : Nombre de Joueur dans la partie.
	 * @param nombreIA : Nombre d'ordinateurs dans le Jeu.
	 */
	void sdlAffSalleAttente(unsigned int variante, unsigned int nombreJoueurs, unsigned int nombreIA);

	/**
	 * @brief 
	 * Appelle l'affichage de la salle d'attente et s'occupe des interactions avec la souris, 
	 * c'est aussi dans cette procédure qu'on va lancer la boucle du Jeu suivant les paramètres de la salle d'attente. 
	 */
	void sdlUno();

	/**
	 * @brief 
	 * Créer une nouvelle fenêtre et affiche [1..9] pour le nombre de joueurs.
	 * @return unsigned int correspondant au nombre de Joueur. 
	 */
	unsigned int sdlAffChoixJoueurs();

	/**
	 * @brief 
	 * Créer une nouvelle fenêtre et affiche de [0..2] pour le nombre de Bots.
	 * @return unsigned int correspondant au nombre de Bot. 
	 */
	unsigned int sdlAffChoixOrdinateurs();

	/**
	 * @brief 
	 * Créer une nouvelle fenêtre et affiche toutes les variantes sur lequel on peut Jouer.
	 * @return unsigned int correspondant à la variante du Jeu. 
	 */
	unsigned int sdlAffChoixJeu();

	/**
	 * @brief 
	 * La boucle qui fera tourner la partie de UNO.
	 * @param jeu 
	 */
	void sdlBoucleJeu(Jeu &jeu);

	/**
	 * @brief 
	 * Procédure qui s'occupe d'afficher une carte.
	 * @param c : La carte.
	 * @param positionX  : La position dans la fenêtre.
	 * @param positionY : //
	 */
	void sdlAffCarte(const Carte &c, int positionX, int positionY);

	/**
	 * @brief 
	 * Affiche les cartes du joueur Actif, des adversaires, talon, pioche ..
	 * @param jeu : Variable permettant de récupérer les infos du Joueur.
	 * @param indiceJoueur : Indice du Joueur que l'on affiche. 
	 */
	void sdlAffJoueur(Jeu &jeu, unsigned int indiceJoueur);

	/**
	 * @brief 
	 * Fonction permettant de choisir la couleur sur laquelle on jouera lorsque l'on pose un Joker 
	 * ou un +4.
	 * @return unsigned int entre [] chaque entier correspond à une Couleur.
	 */
	unsigned int choixCouleur();
	/**
	 * @brief 
	 * Procédure qui gère le mouse click, affichage du texte Contre Uno et de changer le statut de contre Uno.
	 * @param jeu : Objet Jeu sur lequel on est en train jouer.
	 */
	void situationContreUno(Jeu &jeu);
	/**
	 * @brief 
	 * Procédure qui créer une nouvelle fenêtre sdl pour demander au joueur qui a mis la carte 7
	 * avec quelle joueur il veut changer son Jeu.
	 * @param jeu 
	 */
	void sdlEchange(Jeu &jeu);
	/**
	 * @brief 
	 * Procédure qui créer une nouvelle fenêtre sdl pour demander au joueur qui a mis une carte dont le 
	 * doublon est égalment présent dans sa main si il veut jouer les deux.
	 * @param jeu 
	 */
	void sdlDoublon(Jeu &jeu);
	/**
	 * @brief 
	 * Procédure qui créer une nouvelle fenêtre sdl pour demander au joueur si il veut jouer 
	 * la suite de carte possible dans sa main.
	 * @param jeu
	 * @param indice : indice de la carte jouée dans la main du joueur 
	 */
	void sdlSuite(Jeu &jeu, unsigned int indice);

	/**
	 * @brief 
	 * Affiche à côté du talon la couleur que l'on a choisie dans choixCouleur.
	 * @param couleur : Entier qui correspond à une couleur.
	 */
	void sdlAffCouleurChoisie(unsigned int couleur);

	/**
	 * @brief 
	 * Procédure qui gère le moment où on est dans un Uno.
	 * @param jeu
	 */
	void situationUno(Jeu &jeu);
};

#endif
