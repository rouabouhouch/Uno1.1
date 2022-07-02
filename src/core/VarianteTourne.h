#ifndef TOURNE_H
#define TOURNE_H

#include "Jeu.h"

/**
* @brief
* Classe gérant le jeu avec la variante tourne.
* 
*/
class VarianteTourne : public Jeu
{
private:
		
		

	 

public:
		/**
		* @brief
		* Constructeur par défaut
		*/
		VarianteTourne();

		/**
		 * @brief 
		 * Constructeur, avec le tableau de joueurs
		 * @param nbjoueurs : le nombre de Joueur
		 * @param nbIA : le nombre d'IA.
		 */
		VarianteTourne(const unsigned int nbjoueurs, const unsigned int nbIA);
		
		/**
		* @brief
		* Constructeur par défaut
		*/
		~VarianteTourne();

		/**
		* @brief 
		* Le joueur met une carte dans le Talon, la carte posée est retirée de la main, l'effet est appliqué.
		* @param indiceCarte : l'indice de la carte que le joueur veut poser.
		* @param messageErreur : Message d'erreur à afficher avec l'interface Graphique.
		*/
		void poserCarte(const unsigned int& indiceCarte, string &messageErreur) override;

		/**
		* @brief 
		* Fait tourner les mains d'un joueur à l'autre dans le sens du jeu.
		*/
		void tournerMains();


		
				
};

#endif
