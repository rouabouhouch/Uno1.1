#ifndef SUITE_H
#define SUITE_H

#include "Jeu.h"

/**
* @brief
* Classe gérant le jeu avec la variante suite.
* 
*/
class VarianteSuite : public Jeu
{
private:
		
		

	 

public:
		/**
		* @brief
		* Constructeur par défaut
		*/
		VarianteSuite();

		/**
		* @brief
		* Constructeur, avec le tableau de joueurs
		* 
		* @param 
		* nombre de joueurs 
		* @param
		* nombre d'IA
		*/
		VarianteSuite(const unsigned int nbjoueurs, const unsigned int nbIA);
		
		/**
		* @brief
		* Constructeur par défaut
		*/
		~VarianteSuite();

		/**
		* @brief 
		* Teste si la carte passée en paramètre fait partie d'une suite (0 à 7 de la même couleur dans la main du joueur.
		*
		* @return 
		* booléen: vrai la suite existe, faux sinon
		*
		* @param  
		* indice de la carte référence
		* @param
		* main dans laquelle rechercher la suite
		*/
		bool testSuite(unsigned int indice, vector<Carte>);


		/**
		* @brief 
		* Le joueur met une carte dans le Talon, la carte posée est retirée de la main, l'effet est appliqué.
		*
		* @param indiceCarte : 
		* l'indice de la carte que le joueur veut poser.
		* @param messageErreur : 
		* Message d'erreur à afficher avec l'interface Graphique.
		*/
		void poserCarte(const unsigned int& indiceCarte, string &messageErreur) override;


		
				
};

#endif
