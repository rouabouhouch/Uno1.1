#ifndef JOUEUR_H
#define JOUEUR_H


#include <string.h>
#include <unistd.h>
#include <cassert>
#include <vector>
#include <Carte.h>
#include <queue>
#include <stack>
#include <iostream>

using namespace std;

/**
* @brief
* Classe gérant chaque joueur.
* 
*/
class Joueur
{
private:
		/**
		* @brief
		* Largeur de la table de jeu du joueur, pour l'affichage txt.
		* 
		*/
		unsigned int larg;

		/**
		* @brief
		* Hauteur de la table de jeu du joueur, pour l'affichage txt.
		* 
		*/
		unsigned int haut;	 

public:
		/**
		* @brief
		* Nom du joueur.
		* 
		*/		
		string nom;
		
		/**
		* @brief
		* Tableau dynamique contenant les cartes du joueur.
		* 
		*/
		vector<Carte> main;

		/**
		* @brief
		* Numéro du joueur.
		* 
		*/		
		unsigned int numeroJoueur;

		/**
		* @brief
		* Dessin de la table du Joueur pour l'affichage txt.
		* 
		*/
		char tableJoueur [50][200];// 

		/**
		* @brief
		* Position du curseur pour l'affichage txt.
		* 
		*/
		unsigned int indiceEtoile;

		/**
		* @brief 
		* Constructeur par défaut. Initialise numeroJoueur à 1 et nom à Joueur 1.
		* 
		*/
		Joueur();
		
		/**
		* @brief 
		* Constructeur. Affecte num à numeroJoueur.
		* 
		* @param
		* [in] entier naturel inférieur ou égal au nombre de joueurs.
		* 
		*/
		Joueur (const unsigned int num);
		
		/**
		* @brief 
		* Destructeur.
		* 
		*/
		~Joueur();
		
		/**
		* @brief 
		* Insère une carte dans la tableJoueur pour la version txt.
		* 
		* @param
		* [in] entier naturel inférieur à 38, position verticale.
		* @param
		* [in] entier naturel inférieur à 180, position horizontale.
		* @param
		* [in/out] carte à insérer.
		* @param
		* [in] booléen indiquant si le curseur est ici.
		*/
		void insererCartePositionIJ(const unsigned int indi, const unsigned int indj, Carte & c, const bool aEtoile);

		/**
		* @brief 
		* Insère une carte adversaire vierge avec le nom de l'adversaire dans tableJoueur pour la version txt.
		* 
		* @param
		* [in] entier naturel inférieur à 180, position sur la ligne.
		* @param
		* [in/out] entier naturel, numéro de l'adversaire.
		*/
		void insererCarteAdversairePositionJ(const unsigned int indj, const Joueur & j);

		/**
		* @brief 
		* Trie les cartes du joueur par couleur puis par valeur.
		* 
		*/
		void trierMain();

		/**
		* @brief 
		* Efface les cartes en mettant un espace dans la tableJoueur pour la version txt.
		* 
		*/
		void effacerMainTxt();

		/**
		* @brief 
		* Dessine toutes les cartes de la main dans tableJoueur pour la version txt.
		* 
		*/
		void dessinerMainTxt();

		/**
		* @brief 
		* Actualise les cartes de la main dans tableJoueur pour la version txt.
		* 
		*/
		void modifMainTxt();

		/**
		* @brief 
		* Actualise le talon et la pioche dans tableJoueur pour la version txt.
		* 
		* @param
		* [in/out] queue de Carte pour le talon.
		* [in/out] stack de Carte pour la pioche.
		*/
		void modifTalonPiocheTxt(const queue<Carte> & t, const stack<Carte> & p);
		
		/**
		* @brief 
		* Détermine si le joueur est gagnant.
		* 
		* @return
		* Vrai si la main du joueur est vide
		*/
		bool gagnant();

		/**
		* @brief 
		* Accesseur: récupère la largeur de la table du joueur.
		* 
		* @return
		* Entier naturel: largeur de la table.
		*/
		unsigned int getLarg () const;

		/**
		* @brief 
		* Accesseur: récupère la hauteur de la table du joueur.
		* 
		* @return
		* [in] entier naturel: hauteur de la table.
		*/
		unsigned int getHaut () const;

		/**
		* @brief 
		* Accesseur: récupère le caractère de la table du joueur à la position x,y.
		* 
		* @param
		* [in] entier inférieur à 38, position verticale.
		* 
		* @param
		* [in] entier inférieur à 180, position horizontale.
		*
		* @return
		* Caractère: à la position x,y de la table.
		*/
		char getXY (const int x, const int y) const;

};

#endif
