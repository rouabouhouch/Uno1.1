#ifndef CUMUL_H
#define CUMUL_H

#include "Jeu.h"

/**
* @brief
* Classe gérant le jeu avec la variante cumul.
* 
*/
class VarianteCumul : public Jeu
{
private:
		
		

public:
		/**
		* @brief
		* Constructeur par défaut
		*/
		VarianteCumul();

		/**
		* @brief
		* Constructeur, avec le tableau de joueurs
		* 
		* @param 
		* nombre de joueurs 
		* @param
		* nombre d'IA
		*/
		VarianteCumul(const unsigned int nbjoueurs, const unsigned int nbIA);
		
		/**
		* @brief
		* Constructeur par défaut
		*/
		~VarianteCumul();
		
		/**
		* @brief Vérifie si la carte qui va être posé est valide ou non.
		* 
		* @return true 
		* @return false 
		*/
		bool carteValide(const Carte) const override;

		/**
		* @brief 
		* Le joueur met une carte dans le Talon, la carte posée est retirée de la main, l'effet est appliqué.
		* 
		* @param indiceCarte : l'indice de la carte que le joueur veut poser.
		* @param messageErreur : Message d'erreur à afficher avec l'interface Graphique.
		*/
		void poserCarte(const unsigned int& indiceCarte, string &messageErreur) override;

		
		/**
		* @brief 
	 	* On met dans la main du joueur des cartes provenant de la pioche selon cumulCarteAPiocher.
	 	*/
		void piocherCarte() override;

		
				
};

#endif
