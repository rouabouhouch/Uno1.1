#ifndef ATTENTE_H
#define ATTENTE_H


#include <string.h>
#include <cassert>
#include <iostream>

using namespace std;

/**
* @brief
* Classe gérant les choix pour la partie.
* 
*/
class SalleAttente
{
private:
		
			

	 

public:
		/**
		* @brief
		* Largeur de la fenêtre de la salle, pour l'affichage txt.
		* 
		*/
		unsigned int larg;

		/**
		* @brief
		* Hauteur de la fenêtre de la salle, pour l'affichage txt.
		* 
		*/
		unsigned int haut;

		/**
		* @brief
		* Nombre de joueurs de la partie.
		* 
		*/
		unsigned int nombreJoueurs;

		/**
		* @brief
		* Nombre de bots de la partie.
		* 
		*/
		unsigned int nombreIA;

		/**
		* @brief
		* Variante choisie.
		* 1 classique, 2 cumul, 3 doublon, 4 echange, 5 suite, 6 tourne
		* 
		*/
		unsigned int variante;
		/**
		* @brief
		* Dessin de la table du Joueur pour l'affichage txt.
		* 
		*/
		char salle [20][80];

		/**
		* @brief
		* Position du curseur pour l'affichage txt.
		* 
		*/
		unsigned int etoile; 
				


		/**
		* @brief 
		* Constructeur par défaut. Initialise nombreJoueurs à 2, nombreIA à 0 ,variante à 1 et la salle pour l'affichage txt.
		* 
		*/
		SalleAttente(); 

		/**
		* @brief 
		* Destructeur par défaut.
		* 
		*/
		~SalleAttente();
		
		/**
		* @brief
		* Modifie le tableau de caractères pour l'affichage du choix des règles.
		* 
		*/
		void fenetreSalleRegles();

		/**
		* @brief
		* Modifie l'intérieur du cadre, accueil ou choix des règles.
		* 
		* @param
		[in] caractère, 'a' pour accueil, 'r' pour règles
		*/
		void insererContenuCadre(char cadre);

		/**
		* @brief
		* Modifie le tableau de caractères avec les valeurs collectées.
		* 
		*/
		void MaJFenetreSalle();

		/**
		* @brief
		* Demande le nombre de joueurs et met à jour le tableau de caractères.
		* 
		*/
		void choixNombreJoueurs();

		/**
		* @brief
		* Demande le nombre d'ordinateurs et met à jour le tableau de caractères.
		* 
		*/
		void choixNombreIA();
		
		/**
		* @brief
		* Demande la variante et met à jour le tableau de caractères.
		* 
		*/
		void choixJeu();
		
				
};

#endif
