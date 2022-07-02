#ifndef BOT_H
#define BOT_H

#include <Jeu.h>
#include <string.h>
#include <Carte.h>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <unistd.h>

class Jeu;

/**
* @brief
* Classe gérant les bots.
* 
*/
class Bot
{
public:
  /**
         * @brief
         * Numéro du bot
         */
  unsigned int numeroBot;

  /**
     * @brief
     * Nom du bot.
     */
  string nom;

  /**
		* @brief
		* Tableau dynamique contenant les cartes du bot.
		*
		*/
  vector<Carte> main;

  /**
     * @brief Construit un nouvelle objet Bot.
     *
     */
  Bot();

  /**
     * @brief Construit un nouvelle objet Bot avec le numéro du bot.
     *
     * @param num : Numéro du bot.
     */
  Bot(const unsigned int num);

  /**
      * @brief 
      * Trie le vecteur main du bot.
      * 
      */
  void trierMain();

  /**
         * @brief 
         * Procédure qui posera la carte que l'on aura choisit dans choix Jeu.
         */
  void poserCarte() const;

  /**
   * @brief 
   * Compte le nombre de carte qui ont la couleur du talon.
   * @return entier
   */
  int carteMemeCouleurTalon(const Jeu &) const;

  /**
    * @brief 
    * Dans le cas où on est en train de faire une partie avec la variante Cumul on doit regarder la valeur du talon pour voir si le bot peut jouer par dessus ou non.
    * @param jeu : L'objet Jeu qui nous permet de récupérer les infos sur le talon.
    * @param c : Couleur de la carte que l'on donnera.
    * @param indiceCarte : Indice de la carte qui devra être Jouer.
    * @return entier ayant soit 100, 0 ou -1 pour erreur.
    */
  int cartePourCumul(const Jeu &jeu, int &c, int &indiceCarte);

  /**
    * @brief 
    * Compte le nombre de Carte de la couleur de la carte qui a la même valeur que celle du Talon.
    * @param c : Couleur de la carte qui a la même valeur que celle du talon.
    * @param indiceCarte : Indice de cette carte.
    * @return int 
    */
  int carteMemeValeurTalon(const Jeu &, int &c, int &indiceCarte);

  /**
     * @brief 
     * Fonction qui renvoie la couleur avec le plus de carte.
     * @return int 
     */
  int couleurAvecPlusDeCarte() const;

  /**
    * @brief 
    * D'après les fonctions carteMemeCouleur et CarteMemeValeur on dit à l'ordinateur quelle carte il va jouer.
    * @param jeu : L'objet Jeu dans lequel le bot est en train d'intéragir.
    */
  void choixJeu(Jeu &jeu);

  /**
     * @brief 
     * Selon l'entier passé en paramètre on déclenchera poserCarte().
     * @param couleur : Couleur de la carte qui sera joué.
     * @param valeurCarteSpeciale : Dans le cas où on va jouer une carte 13 ou 14.
     */
  void joueCouleurSelonEntier(Jeu &jeu, int couleur, int valeurCarteSpeciale);

  /**
    * @brief 
    * Change les indices des cartes Joker et +4 qui sont forcément modifiés quand on va piocher ou poser une carte dans le Jeu.
    * @param indiceCarte 
    */
  void changeIndiceCarteSpeciale(int indiceCarte);

  /**
    * @brief 
    * Test de regression pour voir si les fonctions et procédures sont fonctionnels.
    */
  void testRegression(Jeu &);

  /**
     * @brief 
     * Remet à 0 le nombre de carte Rouge, Vert, Rouge, Jaune .... utile quand on doit changer les jeux.
     */
  void remetNbCarteZero();
  /**
     * @brief 
     * Procédure permettant de transférer les données nbCarte entre Bot.
     * @param bot : Bot qui va transféré ces infos. 
     */
  void copieNbCarte(Bot &bot);

  /**
   * @brief +1 au le nombre de Cartes Rouges
   * 
   */
  void setCarteRouge();
  /**
    * @brief +1 au le nombre de Cartes Vertes
    * 
    */
  void setCarteVert();
  /**
    * @brief +1 au le nombre de Cartes Bleues 
    * 
    */
  void setCarteBleu();
  /**
    * @brief +1 au nombre de Cartes Jaunes
    * 
    */
  void setCarteJaune();
  /**
    * @brief Set l'indice de la carte Joker
    * 
    * @param i : indice.
    */
  void setCarteJoker(int i);
  /**
    * @brief Set l'indice de la carte +4
    * 
    * @param i : nouvelle indice.
    */
  void setCartePlus4(int i);

  // Accesseur ================
  /**
    * @brief Get le nombre de Cartes Rouges
    * 
    * @return int 
    */
  int getCarteRouge();
  /**
    * @brief Get le nombre de Cartes Vertes
    * 
    * @return int 
    */
  int getCarteVert();
  /**
    * @brief Get le nombre de Cartes Blues
    * 
    * @return int 
    */
  int getCarteBleu();
  /**
    * @brief Get le nombre de Cartes Jaunes
    * 
    * @return int 
    */
  int getCarteJaune();
  /**
    * @brief Get l'indice du Joker.
    * 
    * @return int 
    */
  int getCarteJoker();
  /**
    * @brief Get l'indice de la carte +4.
    * 
    * @return int 
    */
  int getCartePlus4();

private:
  /**
    * @brief 
    * le nombre de cartes Rouge dans la main du bot.
    */
  int nbCarteRouge;
  /**
    * @brief 
    * le nombre de cartes Vertes dans la main du bot.
    */
  int nbCarteVert;
  /**
    * @brief 
    * le nombre de cartes Bleues dans la main du bot.
    */
  int nbCarteBleu;
  /**
    * @brief 
    * le nombre de cartes Jaunes dans la main du bot.
    */
  int nbCarteJaune;
  /**
    * @brief 
    * L'indice de la carte Joker dans la main du bot.
    */
  int indCarteJoker;
  /**
    * @brief 
    * L'indice de la carte +4 dans la main du bot.
    */
  int indCartePlus4;
  unsigned int indChoixCarte; // Indice qui dit quelle carte le bot a joué.
};

#endif
