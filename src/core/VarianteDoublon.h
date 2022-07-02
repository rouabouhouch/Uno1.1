#ifndef DOUBLON_H
#define DOUBLON_H

#include "Jeu.h"

/**
* @brief
* Classe gérant le jeu avec la variante doublon.
* 
*/
class VarianteDoublon : public Jeu
{
private:
			 

public:
		/**
		* @brief
		* Constructeur par défaut
		*/
		VarianteDoublon();

		/**
		* @brief
		* Constructeur, avec le tableau de joueurs
		* 
		* @param 
		* nombre de joueurs 
		* @param
		* nombre d'IA
		*/
		VarianteDoublon(const unsigned int nbjoueurs, const unsigned int nbIA);
		
		/**
		* @brief
		* Constructeur par défaut
		*/
		~VarianteDoublon();

		/**
		* @brief 
		* Teste si il y a dans la main du joueur la même carte qu'il vient jouer.
		*
		* @return 
		* entier naturel: indice du doublon dans la main, -1 sinon
		*
		* * @param  
		* c: carte à rechercher.
		* @param
		* main dans laquelle rechercher la carte
		*/
		int indiceDoublon(Carte c, vector<Carte> main);


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
