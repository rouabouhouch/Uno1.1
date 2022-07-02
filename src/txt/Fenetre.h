#ifndef FENETRE_H
#define FENETRE_H

/**
* @brief 
* Classe qui gère la fenêtre d'affichage.
*/
class Fenetre
{

private:
	/**
	* @brief 
	* Largeur de la fenêtre.
	*/
	int dimx;
	/**
	* @brief 
	* Hauteur de la fenêtre.
	*/
	int dimy;

	/**
	* @brief 
	* Stocke le contenu de la fenêtre dans un tableau 1D,mais on y accede en 2D.
	* 
	*/
	char *ter;

public:
	/**
	* @brief 
	* Constructeur: construit le tableau 1D de caractères.
	* 
	* @param 
	* [in] entier inférieur à 38: hauteur de la fenêtre. 
	* @param 
	* [in] entier inférieur à 180: largeur de la fenêtre. 
	*/
	Fenetre(int dx, int dy);

	/**
	* @brief 
	* Remplit le tableau de caractères identiques, utile pour effacer
	*
	* @param
	* [in] caractère: espace pour vider
	*/
	void clear(char c = ' ');

	/**
	* @brief 
	* Met un caractère à la position x,y.
	* 
	* @param 
	* [in] entier: position verticale 
	* @param 
	* [in] entier: position horizontale 
	* @param 
	* caractère à placer
	* 
	*/
	void prepaFenetre(int x, int y, char c);
	
	/**
	* @brief 
	* Affiche la fenêtre à l'écran
	*
	* @param
	* [in] entier: position verticale.
	* @param
	* [in] entier: position horizontale.
	*/
	void dessine(int x = 0, int y = 0);

	/**
	 * @brief 
	 * 
	 */
	void pause();

	/**
	* @brief
	* Accesseur: renvoie une touche.
	*
	* @return
	* caractère
	*/
	char getCh();
};

void termClear();

#endif
