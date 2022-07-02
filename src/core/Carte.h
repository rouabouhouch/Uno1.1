#ifndef CARTE_H
#define CARTE_H

#include <cassert>
#include <iostream>
using namespace std;

const char carteVierge[8][12] = {
	"###########",
	"#         #",
	"#         #",
	"#         #",
	"#         #",
	"#         #",
	"#         #",
	"###########"};

/**
* @brief
* Classe gérant les cartes du jeu.
* 
*/
class Carte
{
private:
	/**
		* @brief 
		* Valeur de la carte, entre 0 et 14 (10 inverse, 11 passe, 12 +2, 13 +4, 14 jocker).
		*/
	unsigned int valeur;

	/**
		* @brief 
		* Couleur de la carte, entre 1 et 4 (1 rouge, 2 vert, 3 bleu, 4 jaune).
		*/
	unsigned int couleur;

	/**
		* @brief 
		* Hauteur de la carte pour l'affichage txt.
		*/
	int dimx;

	/**
		* @brief 
		* Largeur de la carte pour l'affichage txt.
		*/
	int dimy;

public:
	/**
		* @brief 
		* Dessin de la carte pour l'affichage txt
		*/
	char carte[8][11];

	/**
		* @brief 
		* Position du curseur pour indiquer dans quelle carte on est.
		*/
	unsigned int positionEtoile;

	/**
		* @brief 
		* Constructeur par défaut.
		* 
		*/
	Carte();

	/**
		* @brief 
		* Constructeur : initialisation de valeur, de couleur et du tableau pour l'affichage txt.
		* 
		* @param v entier naturel entre 0 et 14: valeur de la carte.
		* @param c entier naturel entre 1 et 4: couleur de la carte.
		*
		*/
	Carte(const unsigned int v, const unsigned int c);

	/**
		* @brief 
		* Destructeur.
		* 
		*/
	~Carte();

	/**
		* @brief 
		* Accesseur: récupère la valeur de la carte.
		* 
		* @return
		* Entier naturel entre 0 et 14: valeur de la carte.
		*/
	unsigned int getValeur() const;

	/**
		* @brief 
		* Accesseur: récupère la couleur de la carte.
		* 
		* @return
		* Entier naturel entre 1 et 4: couleur de la carte.
		*/
	unsigned int getCouleur() const;

	/**
		* @brief 
		* Mutateur: modifie la couleur de la carte.
		* 
		* @param
		* [in] entier naturel entre 1 et 4, pour la couleur.
		*/
	void setCouleur(const unsigned int c);

	/**
		* @brief 
		* Mutateur: modifie la valeur de la carte.
		* 
		* @param
		* [in] entier naturel entre 0 et 14, pour la valeur.
		*/
	void setValeur(const unsigned int val);

	/**
		* @brief 
		* Opérateur <.
		* 
		* @param
		* [in] carte avec laquelle comparer.
		*
		* @return
		* Vrai si la couleur est inférieure à celle de la carte passée en paramètre, ou la couleur est la même mais la valeur inférieure.
		*/
	bool operator<(const Carte &c);

	/**
		* @brief 
		* Opérateur ==.
		* 
		* @param
		* [in] carte avec laquelle comparer.
		*
		* @return
		* Vrai si les couleurs et les valeurs des 2 cartes sont identiques
		*/
	bool operator==(const Carte &c);

	/**
	 * @brief 
	 * Affiche le numéro et la couleur de la carte.
	 * @param flux : le flux de la sortie standard. 
	 */
	void afficher(std::ostream &flux) const;
	/**
	 * @brief 
	 * Surcharge de l'opérateur cout.
	 * @param out 
	 * @return ostream& 
	 */
	ostream& operator <<(ostream& out);

};

#endif
