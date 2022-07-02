#ifndef AFFICHAGE_TXT
#define AFFICHAGE_TXT

#include <vector>
#include <string>
#include <Jeu.h>
#include "SalleAttente.h"
#include "Fenetre.h"
#include "SalleAttente.h"
#include "VarianteDoublon.h"
#include "VarianteEchange.h"
#include "VarianteTourne.h"
#include "VarianteSuite.h"
#include "VarianteCumul.h"

using namespace std;

		/**
		 * @brief 
		 * Affiche la table de jeu.
		 * @param win : fenêtre sur laquelle on fera l'affichage.
		 * @param jeu : Objet Jeu qui nous permet de récupérer les infos à afficher pour faire  l'affichage de la partie.
		 */
		void txtAff(Fenetre &win, const Jeu &jeu);

		/**
		 * @brief 
		 * Affiche la salle d'attente.
		 * @param win : Fenêtre sur laquelle on fera l'affichage.
		 * @param s : L'objet salle attente dans lequelle on a les tableaux de char ... pour faire le rendu de la salle attente.
		 */
		void txtAffSalleAttente(Fenetre & win, const SalleAttente & s);

		/**
		 * @brief 
		 * Boucle Salle Attente qui nous permet de sélectionné dans cette dernière le nombre de joueur, bots ... toutes les interactions avec user.
		 * @param s : L'objet Salle Attente qui sera modifié car on va sans doute changer le nombre de joueur, bots , les variantes ;..
		 * @return int 
		 */
		int txtBoucleDebut(SalleAttente & s);


		/**
		 * @brief 
		 * Boucle de jeu dans laquelle on fait tourner la partie de Uno.
		 * @param jeu : L'objet Jeu sur lequelle on est en train de jouer.
		 */
		void txtBoucle(Jeu & jeu);

#endif
