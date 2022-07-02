#include "AffichageSDL.h"

#define Mix_LoadWAV(file) Mix_LoadWAV_RW(SDL_RWFromFile(file, "rb"), 1)
#define Mix_PlayChannel(channel, chunk, loops) Mix_PlayChannelTimed(channel, chunk, loops, -1)

const int TAILLE_SPRITE = 32;

const SDL_Color noir = {0, 0, 0};
const SDL_Color bleu = {0, 0, 255};
const SDL_Color jaune = {255, 255, 0};

float temps()
{
    return float(SDL_GetTicks()) / CLOCKS_PER_SEC; // conversion des ms en secondes en divisant par 1000
}

// ============= CLASS IMAGE =============== //
//! \brief Pour gérer une image avec SDL2

Image::Image()
{
    surface = NULL;
    texture = NULL;
    has_changed = false;
}

void Image::loadFromFile(const char *filename, SDL_Renderer *renderer)
{
    surface = IMG_Load(filename);
    if (surface == NULL)
    {
        string nfn = string("../") + filename;
        cout << "Error: cannot load " << filename << ". Trying " << nfn << endl;
        surface = IMG_Load(nfn.c_str());
        if (surface == NULL)
        {
            nfn = string("../") + nfn;
            surface = IMG_Load(nfn.c_str());
        }
    }
    if (surface == NULL)
    {
        cout << "Error: cannot load " << filename << endl;
        SDL_Quit();
        exit(1);
    }

    SDL_Surface *surfaceCorrectPixelFormat = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(surface);
    surface = surfaceCorrectPixelFormat;

    texture = SDL_CreateTextureFromSurface(renderer, surfaceCorrectPixelFormat);
    if (texture == NULL)
    {
        cout << "Error: problem to create the texture of " << filename << endl;
        SDL_Quit();
        exit(1);
    }
}

void Image::loadFromCurrentSurface(SDL_Renderer *renderer)
{
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL)
    {
        cout << "Error: problem to create the texture from surface " << endl;
        SDL_Quit();
        exit(1);
    }
}

void Image::draw(SDL_Renderer *renderer, int x, int y, int w, int h)
{
    int ok;
    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.w = (w < 0) ? surface->w : w;
    r.h = (h < 0) ? surface->h : h;

    if (has_changed)
    {
        ok = SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
        assert(ok == 0);
        has_changed = false;
    }

    ok = SDL_RenderCopy(renderer, texture, NULL, &r);
    assert(ok == 0);
}

SDL_Texture *Image::getTexture() const { return texture; }

void Image::setSurface(SDL_Surface *surf) { surface = surf; }

// ============= CLASS SDLJEU =============== //

/**
    La classe g�rant le jeu avec un affichage SDL
*/

sdlJeu::sdlJeu() : window(nullptr), renderer(nullptr), font(nullptr)
{
    // Initialisation de la SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        cout << "Erreur lors de l'initialisation de la SDL : " << SDL_GetError() << endl;
        SDL_Quit();
        exit(1);
    }

    if (TTF_Init() != 0)
    {
        cout << "Erreur lors de l'initialisation de la SDL_ttf : " << TTF_GetError() << endl;
        SDL_Quit();
        exit(1);
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        cout << "No sound !!!" << endl;
        this->~sdlJeu();
    }

    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << endl;
        SDL_Quit();
        exit(1);
    }
    HauteurEcran = 0;
    LargeurEcran = 0;
    lireFichierRes(); // Lis le fichier et initialise les données membres pour la fenêtre.

    cout << HauteurEcran << endl;
    cout << LargeurEcran << endl;

    // Creation de la fenetre
    window = SDL_CreateWindow("UNO", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LargeurEcran, HauteurEcran, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
    {
        cout << "Erreur lors de la creation de la fenetre : " << SDL_GetError() << endl;
        SDL_Quit();
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // IMAGES
    fondMenu.loadFromFile("data/uno1.bmp", renderer);
    fondPartie.loadFromFile("data/uno1.bmp", renderer);

    // FONTS
    font = TTF_OpenFont("data/DejaVuSansCondensed.ttf", 100);
    if (font == NULL)
    {
        cout << "Failed to load DejaVuSansCondensed.ttf! SDL_TTF Error: " << TTF_GetError() << endl;
        SDL_Quit();
        exit(1);
    }
    fontTexte = TTF_OpenFont("data/DejaVuSansCondensed.ttf", 60);

    // Son
    Mix_AllocateChannels(3);                     // Alloue 3 cannaux
    choixVolume = 5;                             // Notre échelle étant de 0 à 10.
    volume = MIX_MAX_VOLUME / 100 * choixVolume; // On met le son à 5 de base (Moitié).

    cout << volume << endl;
    Mix_Volume(0, volume);              // Mets la musique salle attente et de Jeu au niveau volume.
    Mix_Volume(1, MIX_MAX_VOLUME / 60); // Mets le son 25 %

    sons[0] = Mix_LoadWAV("data/sounds/musiqueMenu.wav");
    sons[1] = Mix_LoadWAV("data/sounds/passageSalleAttente.wav");
    sons[2] = Mix_LoadWAV("data/sounds/selection.wav"); // On charge la musique.
    //sons[3] = Mix_LoadWAV("data/sounds/valide.wav");
    for (int i = 0; i < 3; i++)
    {
        if (sons[i] == NULL)
        {
            cout << "Failed to load son.wav! SDL_mixer Error: " << Mix_GetError() << endl;
            this->~sdlJeu();
            exit(1);
        }
    }
    Mix_PlayChannel(0, sons[0], 1);
}

sdlJeu::~sdlJeu()
{
    TTF_CloseFont(font);
    TTF_CloseFont(fontTexte);
    for (int i = 0; i < 3; i++)
        Mix_FreeChunk(sons[i]); // Libére la mémoire allouer pour le son
    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void sdlJeu::lireFichierRes()
{                               // Lis le fichier Résolution pour définir résolution de la fenêtre au démarrage.
    string ligne;               // La ligne lue dans le fichier.
    stringstream largeurStream; // Permettra de convertir notre string en entier.
    stringstream hauteurStream; // Permettra de convertir notre string en entier.
    ifstream monFichier("resolution.txt");
    if (monFichier.is_open())
    {
        getline(monFichier, ligne); // donne le rés de la première ligne.
        largeurStream << ligne;
        largeurStream >> LargeurEcran;
        getline(monFichier, ligne); // donne le rés de la première ligne.
        hauteurStream << ligne;
        hauteurStream >> HauteurEcran;
        monFichier.close();
    }
    else
    {
        cout << "Erreur ouverture du fichier" << endl;
        exit(EXIT_FAILURE);
    }
}
void sdlJeu::modifFichierRes(int largeur, int hauteur)
{
    ofstream monFichier("resolution.txt");
    if (monFichier.is_open())
    {
        monFichier << to_string(largeur) + "\n";
        monFichier << to_string(hauteur) + "\n";
        monFichier.close();
    }
    else
    {
        cout << "Le fichier n'a pas pu être modifier." << endl;
        exit(EXIT_FAILURE);
    }
}

// Affiche la salle d'attente
void sdlJeu::sdlAffSalleAttente(unsigned int variante, unsigned int nombreJoueurs, unsigned int nombreIA)
{
    //Remplir l'écran de noir
    SDL_SetRenderDrawColor(renderer, 254, 254, 254, 255);
    SDL_RenderClear(renderer);

    fondMenu.draw(renderer, 0, 0, LargeurEcran, HauteurEcran);
    SDL_RenderPresent(renderer);

    // Texte "Jeu"
    SDL_Rect texte;
    texte.x = 100;
    texte.y = 50;
    texte.w = 100;
    texte.h = 50;
    font_im.setSurface(TTF_RenderText_Solid(font, "Jeu : ", jaune));
    font_im.loadFromCurrentSurface(renderer);
    SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);

    texte.x = 220;
    texte.y = 50;
    texte.w = 200;
    texte.h = 50;
    switch (variante)
    {
    case 1:
        font_im.setSurface(TTF_RenderText_Solid(font, "Jeu Classique", jaune));
        break;
    case 2:
        font_im.setSurface(TTF_RenderText_Solid(font, "Variante Cumul", jaune));
        break;
    case 3:
        font_im.setSurface(TTF_RenderText_Solid(font, "Variante Doublon", jaune));
        break;
    case 4:
        font_im.setSurface(TTF_RenderText_Solid(font, "Variante Echange", jaune));
        break;
    case 5:
        font_im.setSurface(TTF_RenderText_Solid(font, "Variante Suite", jaune));
        break;
    case 6:
        font_im.setSurface(TTF_RenderText_Solid(font, "Variante Tourne", jaune));
        break;
    default:
        break;
    }
    font_im.loadFromCurrentSurface(renderer);
    SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);

    // Texte "Nombre de joueurs"
    texte.x = 100;
    texte.y = 150;
    texte.w = 300;
    texte.h = 50;
    font_im.setSurface(TTF_RenderText_Solid(font, "Nombre de joueurs : ", jaune));
    font_im.loadFromCurrentSurface(renderer);
    SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);

    // Texte qui récupère la variable nombreJoueur et affiche le numéro.
    texte.x = 420;
    texte.y = 150;
    texte.w = 50;
    texte.h = 50;
    font_im.setSurface(TTF_RenderText_Solid(font, to_string(nombreJoueurs).c_str(), jaune));
    font_im.loadFromCurrentSurface(renderer);
    SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);

    // Texte "Nombre d'ordinateurs"
    texte.x = 100;
    texte.y = 250;
    texte.w = 300;
    texte.h = 50;
    font_im.setSurface(TTF_RenderText_Solid(font, "Nombre d'ordinateurs : ", jaune));
    font_im.loadFromCurrentSurface(renderer);
    SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);

    // Texte qui récupère la variable nombreIA et affiche le numéro.
    texte.x = 420;
    texte.y = 250;
    texte.w = 50;
    texte.h = 50;
    font_im.setSurface(TTF_RenderText_Solid(font, to_string(nombreIA).c_str(), jaune));
    font_im.loadFromCurrentSurface(renderer);
    SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);

    // Texte "Lancer le jeu"
    texte.x = 100;
    texte.y = 350;
    texte.w = 300;
    texte.h = 50;
    font_im.setSurface(TTF_RenderText_Solid(font, "Lancer le jeu", jaune));
    font_im.loadFromCurrentSurface(renderer);
    SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);

    SDL_RenderPresent(renderer);
}

// Préparation de la partie
void sdlJeu::sdlUno()
{

    SDL_SetRenderDrawColor(renderer, 254, 254, 254, 255);
    SDL_RenderClear(renderer);

    fondMenu.draw(renderer, 0, 0, LargeurEcran, HauteurEcran);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    bool quit = false;

    unsigned int variante = 1;      // entre 1 et 6
    unsigned int nombreJoueurs = 2; // entre 1 et 9
    unsigned int nombreIA = 0;      //entre 0 et 2
    //bool choix;
    int sourisX;
    int sourisY;

    // tant que ce n'est pas la fin ...
    while (!quit)
    {

        // attend un évènement à traiter
        while (SDL_PollEvent(&event))
        {
            sourisX = event.button.x;
            sourisY = event.button.y;
            switch (event.type)
            {
            case SDL_WINDOWEVENT:                                // On clique sur la fenêtre.
                if (event.window.event == SDL_WINDOWEVENT_CLOSE) // On appuie sur la croix pour fermer le Jeu.
                    return;                                      // On arrête le Jeu.
                break;
            case SDL_KEYDOWN:                                         // Si une touche est enfoncee
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) // Si la touche est ESC on ferme le Jeu.
                    return;
                break;
            case SDL_MOUSEBUTTONDOWN: // Si le bouton de la souris est relevé
                cout << "case" << endl;
                if (sourisX > 100 && sourisX < 320 && sourisY > 50 && sourisY < 100) // clic sur la ligne "Jeu"
                {
                    variante = sdlAffChoixJeu();
                    sdlAffSalleAttente(variante, nombreJoueurs, nombreIA);
                    SDL_RenderClear(renderer);
                }
                if (sourisX > 100 && sourisX < 470 && sourisY > 150 && sourisY < 200) // clic sur la ligne "Nombre de joueurs"
                {
                    nombreJoueurs = sdlAffChoixJoueurs();
                    sdlAffSalleAttente(variante, nombreJoueurs, nombreIA);
                }
                if (event.button.x > 100 && event.button.x < 470 && event.button.y > 250 && event.button.y < 300) // clic sur la ligne "Nombre d'ordinateurs"
                {
                    nombreIA = sdlAffChoixOrdinateurs();
                    sdlAffSalleAttente(variante, nombreJoueurs, nombreIA);
                    SDL_RenderClear(renderer);
                }
                if (event.button.x > 100 && event.button.x < 400 && event.button.y > 350 && event.button.y < 400) // clic sur la ligne "Lancer le jeu"
                {
                    // Detruire le font ?
                    quit = true;
                }

                break;
                break;
            default:
                break;
            }
            sdlAffSalleAttente(variante, nombreJoueurs, nombreIA);
        }
    }
    switch (variante)
    {
    case 1:
    {
        Jeu jeu(nombreJoueurs, nombreIA);
        sdlBoucleJeu(jeu);
        break;
    }
    case 2:
    {
        VarianteCumul jeu(nombreJoueurs, nombreIA);
        sdlBoucleJeu(jeu);
        break;
    }
    case 3:
    {
        VarianteDoublon jeu(nombreJoueurs, nombreIA);
        sdlBoucleJeu(jeu);
        break;
    }
    case 4:
    {
        VarianteEchange jeu(nombreJoueurs, nombreIA);
        sdlBoucleJeu(jeu);
        break;
    }
    case 5:
    {
        VarianteSuite jeu(nombreJoueurs, nombreIA);
        sdlBoucleJeu(jeu);
        break;
    }
    case 6:
    {
        VarianteTourne jeu(nombreJoueurs, nombreIA);
        sdlBoucleJeu(jeu);
        break;
    }
    default:
        break;
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

// Fait choisir la variante
unsigned int sdlJeu::sdlAffChoixJeu()
{
    unsigned int choixVariante = 1;
    SDL_Window *choixJeu = SDL_CreateWindow("Choix du jeu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 500, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (choixJeu == NULL)
    {
        cout << "Erreur lors de la creation de la fenetre : " << SDL_GetError() << endl;
        SDL_Quit();
        exit(1);
    }

    SDL_Renderer *rendererJeu = SDL_CreateRenderer(choixJeu, -1, SDL_RENDERER_ACCELERATED);

    SDL_Rect variante;
    variante.x = 50;
    variante.y = 50;
    variante.w = 300;
    variante.h = 50;
    font_color.r = 0;
    font_color.g = 0;
    font_color.b = 255;
    font_im.setSurface(TTF_RenderText_Solid(font, "Classique", jaune));
    font_im.loadFromCurrentSurface(rendererJeu);
    SDL_RenderCopy(rendererJeu, font_im.getTexture(), NULL, &variante);

    variante.x = 50;
    variante.y = 120;
    variante.w = 300;
    variante.h = 50;
    font_color.r = 0;
    font_color.g = 0;
    font_color.b = 255;
    font_im.setSurface(TTF_RenderText_Solid(font, "Variante Cumul", jaune));
    font_im.loadFromCurrentSurface(rendererJeu);
    SDL_RenderCopy(rendererJeu, font_im.getTexture(), NULL, &variante);

    variante.x = 50;
    variante.y = 190;
    variante.w = 300;
    variante.h = 50;
    font_color.r = 0;
    font_color.g = 0;
    font_color.b = 255;
    font_im.setSurface(TTF_RenderText_Solid(font, "Variante Doublon", jaune));
    font_im.loadFromCurrentSurface(rendererJeu);
    SDL_RenderCopy(rendererJeu, font_im.getTexture(), NULL, &variante);

    variante.x = 50;
    variante.y = 260;
    variante.w = 300;
    variante.h = 50;
    font_color.r = 0;
    font_color.g = 0;
    font_color.b = 255;
    font_im.setSurface(TTF_RenderText_Solid(font, "Variante Echange", jaune));
    font_im.loadFromCurrentSurface(rendererJeu);
    SDL_RenderCopy(rendererJeu, font_im.getTexture(), NULL, &variante);

    variante.x = 50;
    variante.y = 330;
    variante.w = 300;
    variante.h = 50;
    font_color.r = 0;
    font_color.g = 0;
    font_color.b = 255;
    font_im.setSurface(TTF_RenderText_Solid(font, "Variante Suite", jaune));
    font_im.loadFromCurrentSurface(rendererJeu);
    SDL_RenderCopy(rendererJeu, font_im.getTexture(), NULL, &variante);

    variante.x = 50;
    variante.y = 400;
    variante.w = 300;
    variante.h = 50;
    font_color.r = 0;
    font_color.g = 0;
    font_color.b = 255;
    font_im.setSurface(TTF_RenderText_Solid(font, "Variante Tourne", jaune));
    font_im.loadFromCurrentSurface(rendererJeu);
    SDL_RenderCopy(rendererJeu, font_im.getTexture(), NULL, &variante);

    SDL_RenderPresent(rendererJeu);

    SDL_Event eventJeu;
    bool choixFait = false;
    int sourisX;
    int sourisY;

    while (!choixFait)
    {
        while (SDL_PollEvent(&eventJeu))
        {
            sourisX = eventJeu.button.x;
            sourisY = eventJeu.button.y;
            switch (eventJeu.type)
            {
            case SDL_WINDOWEVENT:                                   // On clique sur la fenêtre.
                if (eventJeu.window.event == SDL_WINDOWEVENT_CLOSE) // On appuie sur la croix pour fermer.
                    choixFait = true;

                break;
            case SDL_KEYDOWN:                                            // Si une touche est enfoncee
                if (eventJeu.key.keysym.scancode == SDL_SCANCODE_ESCAPE) // Si la touche est ESC on ferme.
                    choixFait = true;

                break;
            case SDL_MOUSEBUTTONDOWN:
                if (sourisX > 50 && sourisX < 350 && sourisY > 50 && sourisY < 450) // clic sur une variante
                {
                    Mix_PlayChannel(1, sons[2], 0); // On joue le son selection 1 fois.
                    choixVariante = (sourisY - 50) / 70 + 1;
                    choixFait = true;
                }
                break;
            default:
                break;
            }
        }
    }
    SDL_DestroyRenderer(rendererJeu);
    SDL_DestroyWindow(choixJeu);
    return choixVariante;
}

// Fait choisir le nombre de joueurs
unsigned int sdlJeu::sdlAffChoixJoueurs()
{
    unsigned int Joueurs = 2;
    SDL_Window *choixJoueurs = SDL_CreateWindow("Choix du nombre de joueurs", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 200, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (choixJoueurs == NULL)
    {
        cout << "Erreur lors de la creation de la fenetre : " << SDL_GetError() << endl;
        SDL_Quit();
        exit(1);
    }

    SDL_Renderer *rendererJoueurs = SDL_CreateRenderer(choixJoueurs, -1, SDL_RENDERER_ACCELERATED);

    SDL_Rect choix;
    choix.x = 50;
    choix.y = 200;
    choix.w = 1000;
    choix.h = 500;
    SDL_SetRenderDrawColor(rendererJoueurs, 0, 0, 0, 255);
    SDL_RenderFillRect(rendererJoueurs, &choix);
    for (int i = 0; i < 9; i++)
    {
        SDL_Rect paramJoueurs;
        paramJoueurs.x = 50 + 100 * i;
        paramJoueurs.y = 75;
        paramJoueurs.w = 50;
        paramJoueurs.h = 50;
        font_im.setSurface(TTF_RenderText_Solid(font, to_string(i + 1).c_str(), jaune));
        font_im.loadFromCurrentSurface(rendererJoueurs);
        SDL_RenderCopy(rendererJoueurs, font_im.getTexture(), NULL, &paramJoueurs);
    }
    SDL_RenderPresent(rendererJoueurs);

    SDL_Event eventJoueurs;
    bool choixFait = false;
    int sourisX;
    int sourisY;

    while (!choixFait)
    {
        while (SDL_PollEvent(&eventJoueurs))
        {
            sourisX = eventJoueurs.button.x;
            sourisY = eventJoueurs.button.y;
            switch (eventJoueurs.type)
            {
            case SDL_WINDOWEVENT:                                       // On clique sur la fenêtre.
                if (eventJoueurs.window.event == SDL_WINDOWEVENT_CLOSE) // On appuie sur la croix pour fermer.
                    choixFait = true;
                break;
            case SDL_KEYDOWN:                                                // Si une touche est enfoncee
                if (eventJoueurs.key.keysym.scancode == SDL_SCANCODE_ESCAPE) // Si la touche est ESC on ferme.
                    choixFait = true;

                break;
            case SDL_MOUSEBUTTONDOWN:
                if (sourisX > 50 && sourisX < 900 && sourisY > 75 && sourisY < 125) // clic sur une variante
                {
                    Mix_PlayChannel(1, sons[2], 0); // On joue le son selection 1 fois.
                    Joueurs = (sourisX - 50) / 100 + 1;
                    choixFait = true;
                }
                break;
            default:
                break;
            }
        }
    }
    SDL_DestroyRenderer(rendererJoueurs);
    SDL_DestroyWindow(choixJoueurs);
    return Joueurs;
}

// Fait choisir le nombre de bots
unsigned int sdlJeu::sdlAffChoixOrdinateurs()
{
    unsigned int nombreIA = 0;
    SDL_Window *choixBots = SDL_CreateWindow("Choix du nombre d'ordinateurs", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 500, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (choixBots == NULL)
    {
        cout << "Erreur lors de la creation de la fenetre : " << SDL_GetError() << endl;
        SDL_Quit();
        exit(1);
    }

    SDL_Renderer *rendererBots = SDL_CreateRenderer(choixBots, -1, SDL_RENDERER_ACCELERATED);

    SDL_Rect choix;
    choix.x = 500;
    choix.y = 200;
    choix.w = 1000;
    choix.h = 500;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &choix);
    for (int i = 0; i < 3; i++)
    {
        SDL_Rect paramBots;
        paramBots.x = 150 + 100 * i;
        paramBots.y = 200;
        paramBots.w = 50;
        paramBots.h = 50;
        font_im.setSurface(TTF_RenderText_Solid(font, to_string(i).c_str(), jaune));
        font_im.loadFromCurrentSurface(rendererBots);
        SDL_RenderCopy(rendererBots, font_im.getTexture(), NULL, &paramBots);
    }
    SDL_RenderPresent(rendererBots);

    SDL_Event eventBots;
    bool choixFait = false;
    int sourisX;
    int sourisY;

    while (!choixFait)
    {
        while (SDL_PollEvent(&eventBots))
        {
            sourisX = eventBots.button.x;
            sourisY = eventBots.button.y;
            switch (eventBots.type)
            {
            case SDL_WINDOWEVENT:                                    // On clique sur la fenêtre.
                if (eventBots.window.event == SDL_WINDOWEVENT_CLOSE) // On appuie sur la croix pour fermer.
                {
                    choixFait = true;
                }
                break;
            case SDL_KEYDOWN:                                             // Si une touche est enfoncee
                if (eventBots.key.keysym.scancode == SDL_SCANCODE_ESCAPE) // Si la touche est ESC on ferme.
                {
                    choixFait = true;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (sourisX > 50 && sourisX < 450 && sourisY > 200 && sourisY < 250) // clic sur une variante
                {
                    Mix_PlayChannel(1, sons[2], 0); // On joue le son selection 1 fois.
                    nombreIA = (sourisX - 150) / 100;
                    choixFait = true;
                }
                break;

            default:
                break;
            }
        }
    }
    SDL_DestroyRenderer(rendererBots);
    SDL_DestroyWindow(choixBots);
    return nombreIA;
}

// Affiche une carte à une certaine position
void sdlJeu::sdlAffCarte(const Carte &c, int positionX, int positionY)
{
    int carteEntier;
    char nomFichier[20];
    string nomCarte;
    char const *nomImage;
    if (c.getValeur() == 13)
    {
        im_carte.loadFromFile("data/+4.png", renderer);
        im_carte.draw(renderer, positionX, positionY, (LargeurEcran / 17.5), (HauteurEcran / 6.9));
    }
    if (c.getValeur() == 14)
    {
        im_carte.loadFromFile("data/joker.png", renderer);
        im_carte.draw(renderer, positionX, positionY, (LargeurEcran / 17.5), (HauteurEcran / 6.9));
    }
    if (c.getValeur() <= 12)
    {
        carteEntier = 10 * c.getValeur() + c.getCouleur();
        nomCarte = to_string(carteEntier);
        nomImage = nomCarte.c_str();
        strcpy(nomFichier, "data/");
        strcat(nomFichier, nomImage);
        strcat(nomFichier, ".png");
        im_carte.loadFromFile(nomFichier, renderer);
        im_carte.draw(renderer, positionX, positionY, (LargeurEcran / 17.5), (HauteurEcran / 6.9));
    }
}

// Affiche la table du joueur, indiceJoueur est l'indice dans le tableau jeu.joueurs
void sdlJeu::sdlAffJoueur(Jeu &jeu, unsigned int indiceJoueur)
{
    //Remplir l'écran de noir
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_Rect texte;

    // affiche les adversaires
    for (unsigned int i = 0; i < jeu.nombreJoueurs + jeu.nombreIA - 1; i++)
    {
        int positionCarte = (LargeurEcran - ((LargeurEcran / 14.7) * (jeu.nombreJoueurs + jeu.nombreIA - 1) - (LargeurEcran / 96))) / 2 + (LargeurEcran / 14.7) * i;

        // nom adversaire
        texte.x = positionCarte;
        texte.y = 0;
        texte.w = LargeurEcran / 17.5;
        texte.h = HauteurEcran / 21.5;
        // Calcul l'indice différent du joueur actif pour afficher les noms et nb cartes.
        unsigned int indiceAffiche = (jeu.joueurActif + 1 + i) % (jeu.nombreJoueurs + jeu.nombreIA);
        if (indiceAffiche >= jeu.nombreJoueurs)
        {
            font_im.setSurface(TTF_RenderText_Solid(font, jeu.joueursBot[indiceAffiche - jeu.nombreJoueurs].nom.c_str(), jaune));
        }
        else
        {
            font_im.setSurface(TTF_RenderText_Solid(font, jeu.joueurs[indiceAffiche].nom.c_str(), jaune));
        }
        font_im.loadFromCurrentSurface(renderer);
        SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);

        // carte adversaire
        im_carte.loadFromFile("data/dosvide.png", renderer);
        im_carte.draw(renderer, positionCarte, LargeurEcran / 38, HauteurEcran / 9.8, LargeurEcran / 12.2);

        // nombre de cartes adversaires
        texte.x = positionCarte + LargeurEcran / 38;
        texte.y = HauteurEcran / 9.5;
        texte.w = LargeurEcran / 64;
        texte.h = HauteurEcran / 36;
        if (indiceAffiche >= jeu.nombreJoueurs)
        {
            font_im.setSurface(TTF_RenderText_Solid(font, to_string(jeu.joueursBot[indiceAffiche - jeu.nombreJoueurs].main.size()).c_str(), jaune));
        }
        else
        {
            font_im.setSurface(TTF_RenderText_Solid(font, to_string(jeu.joueurs[indiceAffiche].main.size()).c_str(), jaune));
        }
        font_im.loadFromCurrentSurface(renderer);
        SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);
    }

    // affiche le centre
    // pioche
    im_carte.loadFromFile("data/dos.png", renderer);
    im_carte.draw(renderer, LargeurEcran / 2.4, HauteurEcran / 3.6, LargeurEcran / 17.5, HauteurEcran / 6.9);

    // talon
    sdlAffCarte(jeu.talon.back(), LargeurEcran / 1.76, HauteurEcran / 3.6);

    // Uno
    im_carte.loadFromFile("data/carteuno.png", renderer);
    im_carte.draw(renderer, LargeurEcran / 3.35, HauteurEcran / 3.34, LargeurEcran / 16.5, HauteurEcran / 11);

    texte.x = LargeurEcran / 3.17;
    texte.y = HauteurEcran / 3.16;
    texte.w = LargeurEcran / 38;
    texte.h = HauteurEcran / 21.6;
    font_im.setSurface(TTF_RenderText_Solid(font, "Uno", noir));
    font_im.loadFromCurrentSurface(renderer);
    SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);

    // Contre Uno
    im_carte.loadFromFile("data/cartecontreuno.png", renderer);
    im_carte.draw(renderer, LargeurEcran / 1.516, HauteurEcran / 3.34, LargeurEcran / 16, HauteurEcran / 11);

    texte.x = LargeurEcran / 1.484;
    texte.y = HauteurEcran / 3.22;
    texte.w = LargeurEcran / 30;
    texte.h = HauteurEcran / 22;
    font_im.setSurface(TTF_RenderText_Solid(font, "Contre", noir));
    font_im.loadFromCurrentSurface(renderer);
    SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);
    texte.x = LargeurEcran / 1.48;
    texte.y = HauteurEcran / 2.95;
    texte.w = LargeurEcran / 40;
    texte.h = HauteurEcran / 21.6;
    font_im.setSurface(TTF_RenderText_Solid(font, "Uno", noir));
    font_im.loadFromCurrentSurface(renderer);
    SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);

    // affiche le nom et la main du joueur actif
    if (indiceJoueur < jeu.nombreJoueurs)
    { // Si c'est un Joueur et non pas un Bot qui doit jouer on affiche sa main.
        // le nom
        texte.x = 0;
        texte.y = LargeurEcran / 3.5;
        texte.w = LargeurEcran / 17.5;
        texte.h = HauteurEcran / 21.6;
        font_im.setSurface(TTF_RenderText_Solid(font, jeu.joueurs[indiceJoueur].nom.c_str(), jaune));
        font_im.loadFromCurrentSurface(renderer);
        SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);

        // la main
        unsigned int tailleMain = jeu.joueurs[indiceJoueur].main.size();
        for (unsigned int i = 0; i < tailleMain; i++)
        {
            sdlAffCarte(jeu.joueurs[indiceJoueur].main[i], (LargeurEcran / 17.5) * (i % 17), (LargeurEcran / 3.2) + (LargeurEcran / 9.6) * (i / 17));
        }
    }

    SDL_RenderPresent(renderer);
}

// Fait choisir la couleur à jouer (après joker et +4)
unsigned int sdlJeu::choixCouleur()
{
    SDL_Window *choixCouleur = SDL_CreateWindow("Quelle couleur?", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 200, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (choixCouleur == NULL)
    {
        cout << "Erreur lors de la creation de la fenetre : " << SDL_GetError() << endl;
        SDL_Quit();
        exit(1);
    }

    SDL_Renderer *rendererCouleur = SDL_CreateRenderer(choixCouleur, -1, SDL_RENDERER_ACCELERATED);

    Image im_choix;
    im_choix.loadFromFile("data/questioncouleur.png", rendererCouleur);
    im_choix.draw(rendererCouleur, LargeurEcran / 38.4, HauteurEcran / 48, LargeurEcran / 17.5, HauteurEcran / 8);

    SDL_Rect texte;
    texte.x = LargeurEcran / 9.1;
    texte.y = HauteurEcran / 14.4;
    texte.w = LargeurEcran / 19.2;
    texte.h = HauteurEcran / 21.6;
    font_im.setSurface(TTF_RenderText_Solid(font, "Rouge", jaune));
    font_im.loadFromCurrentSurface(rendererCouleur);
    SDL_RenderCopy(rendererCouleur, font_im.getTexture(), NULL, &texte);

    texte.x = LargeurEcran / 5.641;
    texte.y = HauteurEcran / 14.4;
    texte.w = LargeurEcran / 19.2;
    texte.h = HauteurEcran / 21.6;
    font_im.setSurface(TTF_RenderText_Solid(font, "Vert", jaune));
    font_im.loadFromCurrentSurface(rendererCouleur);
    SDL_RenderCopy(rendererCouleur, font_im.getTexture(), NULL, &texte);

    texte.x = LargeurEcran / 4.08;
    texte.y = HauteurEcran / 14.4;
    texte.w = LargeurEcran / 19.2;
    texte.h = HauteurEcran / 21.6;
    font_im.setSurface(TTF_RenderText_Solid(font, "Bleu", jaune));
    font_im.loadFromCurrentSurface(rendererCouleur);
    SDL_RenderCopy(rendererCouleur, font_im.getTexture(), NULL, &texte);

    texte.x = LargeurEcran / 3.2;
    texte.y = HauteurEcran / 14.4;
    texte.w = LargeurEcran / 19.2;
    texte.h = HauteurEcran / 21.6;
    font_im.setSurface(TTF_RenderText_Solid(font, "Jaune", jaune));
    font_im.loadFromCurrentSurface(rendererCouleur);
    SDL_RenderCopy(rendererCouleur, font_im.getTexture(), NULL, &texte);

    SDL_RenderPresent(rendererCouleur);

    SDL_Event eventCouleur;
    bool choixFait = false;
    int sourisX;
    int sourisY;
    unsigned int couleur;

    while (!choixFait)
    {
        while (SDL_PollEvent(&eventCouleur) && eventCouleur.type == SDL_MOUSEBUTTONDOWN)
        {
            sourisX = eventCouleur.button.x;
            sourisY = eventCouleur.button.y;
            if (eventCouleur.type == SDL_MOUSEBUTTONDOWN)
            {
                if (sourisX > (LargeurEcran / 9.1) && sourisX < (LargeurEcran / 2.74) && sourisY > (HauteurEcran / 14.4) && sourisY < (HauteurEcran / 4.69)) // clic sur une couleur
                {
                    couleur = (sourisX - (LargeurEcran / 9.1)) / (LargeurEcran / 14.7) + 1;
                    choixFait = true;
                }
            }
        }
    }
    SDL_DestroyRenderer(rendererCouleur);
    SDL_DestroyWindow(choixCouleur);
    return couleur;
}

// affiche une carte de la couleur qui a été choisie
void sdlJeu::sdlAffCouleurChoisie(unsigned int couleur)
{
    char nomFichier[20];
    string nomCouleur;
    char const *nomImage;
    nomCouleur = to_string(couleur);
    nomImage = nomCouleur.c_str();
    strcpy(nomFichier, "data/carte");
    strcat(nomFichier, nomImage);
    strcat(nomFichier, ".png");
    im_carte.loadFromFile(nomFichier, renderer);
    im_carte.draw(renderer, (LargeurEcran / 1.76), (HauteurEcran / 3.6), LargeurEcran / 17.5, HauteurEcran / 7);
    SDL_RenderPresent(renderer);
}

void sdlJeu::sdlMenu()
{
    SDL_Event events;
    bool isOpen{true};
    bool openReglage{false};
    bool openSalleAttente{false};
    Menu menu;
    // Police des textes
    TTF_Font *fontMenuTitre = TTF_OpenFont("data/DejaVuSansCondensed.ttf", 110);
    if (LargeurEcran == 1920 && HauteurEcran == 1080)
    {
        fontTexte = TTF_OpenFont("data/DejaVuSansCondensed.ttf", 80);
    }

    // Créer tous les textes pour le Menu en allant récupérer dans le vecteur string dans la classe Menu.
    SDL_Surface *surfTexte[5];
    surfTexte[0] = TTF_RenderText_Blended(fontMenuTitre, "Uno", SDL_Color{255, 255, 255, 255});
    for (int i = 0; i < 4; i++)
    {
        surfTexte[i + 1] = TTF_RenderText_Blended(fontTexte, (menu.getNomMenu(i)).c_str(), SDL_Color{255, 255, 255, 255});
    }
    // Creations des textures pour les textes.
    SDL_Texture *textureTexte[5];
    for (int i = 0; i < 5; i++)
    {
        textureTexte[i] = SDL_CreateTextureFromSurface(renderer, surfTexte[i]);
    }

    // Rectangle pour régler la position des texte.
    SDL_Rect positionTexte[5];
    for (int i = 0; i < 5; i++)
    {
        SDL_QueryTexture(textureTexte[i], nullptr, nullptr, &positionTexte[i].w, &positionTexte[i].h); // Récupere la dimension de la texture
    }

    // Centre la texture du titre sur l'écran avec le rect.
    positionTexte[0].x = (LargeurEcran / 2 - positionTexte[0].w / 2); // on soustrait pour que le texte soit alignée.
    positionTexte[0].y = HauteurEcran / 14;

    // On définie les coordo des textes Jouer, Par rapport à la coordo du titre.
    positionTexte[1].x = positionTexte[0].x + LargeurEcran / 6 - positionTexte[0].x / 2.7; // Le x est toujours le même pour tous les textes.
    positionTexte[1].y = positionTexte[0].y + HauteurEcran / 4;

    // Reglage
    positionTexte[2].x = positionTexte[0].x + LargeurEcran / 6 - positionTexte[0].x / 2.2; // Le x est toujours le même pour tous les textes.
    positionTexte[2].y = positionTexte[0].y + HauteurEcran / 2.6;

    // Regles
    positionTexte[3].x = positionTexte[0].x + LargeurEcran / 6 - positionTexte[0].x / 2.45; // Le x est toujours le même pour tous les textes.
    positionTexte[3].y = positionTexte[0].y + HauteurEcran / 1.9;

    // Quitter
    positionTexte[4].x = positionTexte[0].x + LargeurEcran / 6 - positionTexte[0].x / 2.4; // Le x est toujours le même pour tous les textes.
    positionTexte[4].y = positionTexte[0].y + HauteurEcran / 1.5;

    // On libère la surface.
    for (int i = 0; i < 5; i++)
    {
        SDL_FreeSurface(surfTexte[i]);
    }

    TTF_CloseFont(fontMenuTitre);

    // Chemin + nom Image à charger pour animation.
    string nomImageBase = "data/UnoAnim/76061103e5c846bf867d0ee82f9fcbbd42WnmC6xmFteZPZe-";
    // Variable qui contiendra le nom finale.
    string imageACharger;

    // On créer un tableau de surface et un de texture.
    SDL_Surface *image[15];
    SDL_Texture *textImage[15]; // On créer un tableau de Texture pour chaque image.

    for (int i = 0; i < 15; i++)
    { // On charge les 15 images pour faire le gif.
        imageACharger = nomImageBase + to_string(i) + ".bmp";
        // On convertit en char pour la fonction IMG_LOAD
        image[i] = IMG_Load(imageACharger.c_str());
        if (image == NULL) // On regarde si on a réussi à charger les images.
        {
            cout << "Error: cannot load " << imageACharger << endl;
            SDL_Quit();
            exit(EXIT_FAILURE);
        }
        textImage[i] = SDL_CreateTextureFromSurface(renderer, image[i]);
        SDL_FreeSurface(image[i]); // On libère l'espace de la surface.
    }

    // Rectangle destionation pour la position image dans la fenêtre.
    SDL_Rect dstrect = {
        0,
        0,
        100, 100};
    // Nous permettra de charger chaque image dans le tableau image.
    dstrect.y = HauteurEcran / 3.1;
    int k = 0;
    // Variable qui contiendront la position de la souris.
    int posSourisX;
    int posSourisY;

    // On récupère les événements.
    while (isOpen)
    {
        dstrect.x = (LargeurEcran / 2 + LargeurEcran / 15);

        while (SDL_PollEvent(&events))
        {
            switch (events.type)
            {
            case SDL_WINDOWEVENT:
                if (events.window.event == SDL_WINDOWEVENT_CLOSE)
                    isOpen = false;
                break;
            case SDL_MOUSEBUTTONDOWN: // Click de souris
                SDL_Log("+clic");
                posSourisX = events.button.x;
                posSourisY = events.button.y;

                // On clique sur jouer et ça lance la salle d'attente (voir si il faut pas enlever des trucs).
                if ((posSourisX > positionTexte[1].x && posSourisY > positionTexte[1].y) &&                        // Point en haut à gauche
                    (posSourisX > positionTexte[1].x && posSourisY < positionTexte[1].y + HauteurEcran / 12) &&    // Point en bas à gauche
                    (posSourisX < (positionTexte[1].x + LargeurEcran / 8) && (posSourisY > positionTexte[1].y)) && // Point en haut à droite
                    (posSourisX < (positionTexte[1].x + LargeurEcran / 8) && (posSourisY < positionTexte[1].y + HauteurEcran / 12)))
                {
                    cout << "je clique sur Jouer." << endl;
                    Mix_PlayChannel(1, sons[1], 0); // On joue le son salleAttente.
                    openSalleAttente = true;
                    isOpen = false;
                }
                // On clique sur Reglage
                if ((posSourisX > positionTexte[2].x && posSourisY > positionTexte[2].y) &&                        // Point en haut à gauche
                    (posSourisX > positionTexte[2].x && posSourisY < positionTexte[2].y + HauteurEcran / 12) &&    // Point en bas à gauche
                    (posSourisX < (positionTexte[2].x + LargeurEcran / 8) && (posSourisY > positionTexte[2].y)) && // Point en haut à droite
                    (posSourisX < (positionTexte[2].x + LargeurEcran / 8) && (posSourisY < positionTexte[2].y + HauteurEcran / 12)))
                {
                    isOpen = false; // On quitte la boucle.
                    openReglage = true;
                    Mix_PlayChannel(1, sons[2], 0); // On joue le son selection 1 fois.
                }
                // On clique sur Regles
                if ((posSourisX > positionTexte[3].x && posSourisY > positionTexte[3].y) &&                        // Point en haut à gauche
                    (posSourisX > positionTexte[3].x && posSourisY < positionTexte[3].y + HauteurEcran / 12) &&    // Point en bas à gauche
                    (posSourisX < (positionTexte[3].x + LargeurEcran / 8) && (posSourisY > positionTexte[3].y)) && // Point en haut à droite
                    (posSourisX < (positionTexte[3].x + LargeurEcran / 8) && (posSourisY < positionTexte[3].y + HauteurEcran / 12)))
                {
                    cout << "Je clique sur Regles" << endl;
                    Mix_PlayChannel(1, sons[2], 0); // On joue le son selection 1 fois.
                }
                // On clique sur quitter
                if ((posSourisX > positionTexte[4].x && posSourisY > positionTexte[4].y) &&                        // Point en haut à gauche
                    (posSourisX > positionTexte[4].x && posSourisY < positionTexte[4].y + HauteurEcran / 12) &&    // Point en bas à gauche
                    (posSourisX < (positionTexte[4].x + LargeurEcran / 8) && (posSourisY > positionTexte[4].y)) && // Point en haut à droite
                    (posSourisX < (positionTexte[4].x + LargeurEcran / 8) && (posSourisY < positionTexte[4].y + HauteurEcran / 12)))
                {
                    isOpen = false;
                }
                break;
            }
        }

        // On met à jour l'affichage.
        SDL_SetRenderDrawColor(renderer, 254, 254, 254, 255); // Affiche la couleur de fond.
        SDL_RenderClear(renderer);

        fondMenu.draw(renderer, 0, 0, LargeurEcran, HauteurEcran);
        // Affichage des Textes
        for (int i = 0; i < 5; i++)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // On passe sur la couleur du texte.
            SDL_RenderCopy(renderer, textureTexte[i], nullptr, &positionTexte[i]);
        }

        SDL_RenderCopy(renderer, textImage[k], nullptr, &dstrect); // Affichage Image.
        k++;
        SDL_Delay(50);
        if (k == 14)
            k = 0;

        SDL_RenderPresent(renderer);
    }
    for (int i = 0; i < 5; i++)
    {
        SDL_DestroyTexture(textureTexte[i]);
    }
    if (openReglage)
    {
        sdlReglage(menu);
    }
    if (openSalleAttente)
    {

        sdlUno();
    }
}

void sdlJeu::sdlReglage(Menu &menu)
{
    SDL_Event events;
    bool isOpen{true};
    bool openMenu{false};

    // Police des textes
    TTF_Font *fontResText = TTF_OpenFont("data/DejaVuSansCondensed.ttf", 45);

    // Créer tous les textes pour les réglages en allant récupérer dans le vecteur string dans la classe Menu.
    SDL_Surface *tabSurfTitre[4];
    for (int i = 0; i < 4; i++)
    {
        tabSurfTitre[i] = TTF_RenderText_Blended(fontTexte, (menu.getOptions(i)).c_str(), SDL_Color{255, 255, 255, 255});
    }
    if (LargeurEcran == 800 && HauteurEcran == 800)
    {
        tabSurfTitre[3] = TTF_RenderText_Blended(fontResText, (menu.getOptions(3)).c_str(), SDL_Color{255, 255, 255, 255});
    }
    // Textes pour les résolutions d'écran.
    SDL_Surface *tabRes[3];
    for (int i = 0; i < 3; i++)
    {
        tabRes[i] = TTF_RenderText_Blended(fontResText, (menu.getResolution(i)).c_str(), SDL_Color{255, 255, 255, 255});
        ;
    }
    // Creations des textures pour les textes.
    SDL_Texture *tabTextureTitre[4];
    for (int i = 0; i < 4; i++)
    {
        tabTextureTitre[i] = SDL_CreateTextureFromSurface(renderer, tabSurfTitre[i]); // Crée la texture qu'on va afficher a partir de la surface
    }

    // Résolutions
    SDL_Texture *tabTextRes[3];
    for (int i = 0; i < 3; i++)
    {
        tabTextRes[i] = SDL_CreateTextureFromSurface(renderer, tabRes[i]); // Crée la texture qu'on va afficher a partir de la surface
    }
    // Creation des textes pour régler le volume du son.
    SDL_Texture *tabSon[11];
    SDL_Surface *surfEchelle;
    SDL_Rect positionEchelle[11];

    for (int i = 0; i < 11; i++)
    {
        surfEchelle = TTF_RenderText_Blended(fontResText, (menu.getSon(i)).c_str(), SDL_Color{255, 255, 255, 255});
        tabSon[i] = SDL_CreateTextureFromSurface(renderer, surfEchelle);
        SDL_QueryTexture(tabSon[i], nullptr, nullptr, &positionEchelle[i].w, &positionEchelle[i].h);
    }

    // Rectangle pour régler la position des textes.
    SDL_Rect tabPositionTitre[4];
    // Postion Résolution texte.
    SDL_Rect tabPositionRes[3];
    SDL_Rect sonSelectionne; // Encadre le son sur lequelle on clique ou qui est déjà actif.

    for (int i = 0; i < 4; i++)
    {
        SDL_QueryTexture(tabTextureTitre[i], nullptr, nullptr, &tabPositionTitre[i].w, &tabPositionTitre[i].h); // Récupere la dimension de la texture
    }

    for (int i = 0; i < 3; i++)
    {
        SDL_QueryTexture(tabTextRes[i], nullptr, nullptr, &tabPositionRes[i].w, &tabPositionRes[i].h); // Récupere la dimension de la texture
    }

    // On définie les coordo des titres Resolutions, Changer Pseudo, Son.
    tabPositionTitre[3].x = LargeurEcran / 30; // Le x est toujours le même pour tous les textes.
    tabPositionTitre[3].y = HauteurEcran / 30;

    tabPositionTitre[0].x = (LargeurEcran / 2 - tabPositionTitre[0].w / 2); // on soustrait pour que le texte soit alignée.
    tabPositionTitre[0].y = HauteurEcran / 14;

    tabPositionTitre[1].x = tabPositionTitre[0].x + LargeurEcran / 6 - tabPositionTitre[0].x / 1.8; // Le x est toujours le même pour tous les textes.
    tabPositionTitre[1].y = tabPositionTitre[0].y + HauteurEcran / 2.6;

    tabPositionTitre[2].x = tabPositionTitre[0].x + LargeurEcran / 6 - tabPositionTitre[0].x / 3.5; // Le x est toujours le même pour tous les textes.
    tabPositionTitre[2].y = HauteurEcran / 1.4;

    //############### Résolutions ###############
    tabPositionRes[0].x = tabPositionTitre[0].x + LargeurEcran / 6 - tabPositionTitre[0].x / 2.7; // Le x est toujours le même pour tous les textes.
    tabPositionRes[0].y = tabPositionTitre[0].y + HauteurEcran / 9;

    tabPositionRes[1].x = tabPositionTitre[0].x + LargeurEcran / 6 - tabPositionTitre[0].x / 2.7; // Le x est toujours le même pour tous les textes.
    tabPositionRes[1].y = tabPositionTitre[0].y + HauteurEcran / 5;

    tabPositionRes[2].x = tabPositionTitre[0].x + LargeurEcran / 6 - tabPositionTitre[0].x / 2.7; // Le x est toujours le même pour tous les textes.
    tabPositionRes[2].y = tabPositionTitre[0].y + HauteurEcran / 3.5;

    // ############## Position des sons.
    positionEchelle[0].x = LargeurEcran / 20;
    positionEchelle[0].y = HauteurEcran / 1.2;

    for (int i = 1; i < 11; i++)
    {
        positionEchelle[i].x = positionEchelle[i - 1].x + LargeurEcran / 12;
        positionEchelle[i].y = positionEchelle[0].y;
        if (i == 5) // choixVolume est pour rappelle la donnée membre qui nous indique sur l'échelle de 0 à 10 le volume de la musique.
        {
            // Position du rectangle repère pour régler le son.
            sonSelectionne.x = positionEchelle[i].x - LargeurEcran / 130;
            sonSelectionne.y = positionEchelle[i].y;
            if (LargeurEcran == 1920 && HauteurEcran == 1080)
            {
                sonSelectionne.w = LargeurEcran / 40;
                sonSelectionne.h = HauteurEcran / 22;
            }
            else if (LargeurEcran == 1280 && HauteurEcran == 720)
            {
                sonSelectionne.w = LargeurEcran / 30;
                sonSelectionne.h = HauteurEcran / 15;
            }
            else
            {
                sonSelectionne.w = LargeurEcran / 20;
                sonSelectionne.h = HauteurEcran / 15;
            }
        }
    }
    // On libère la surface.
    for (int i = 0; i < 4; i++)
    {
        SDL_FreeSurface(tabSurfTitre[i]);
    }
    for (int i = 0; i < 3; i++)
    {
        SDL_FreeSurface(tabRes[i]);
    }
    SDL_FreeSurface(surfEchelle);
    TTF_CloseFont(fontResText);

    // Variable qui contiendront la position de la souris.
    int posSourisX;
    int posSourisY;

    // On récupère les événements.
    while (isOpen)
    {

        while (SDL_PollEvent(&events))
        {
            switch (events.type)
            {
            case SDL_WINDOWEVENT:
                if (events.window.event == SDL_WINDOWEVENT_CLOSE)
                    isOpen = false;
                break;
            case SDL_MOUSEBUTTONDOWN: // Click de souris
                SDL_Log("+clic");
                posSourisX = events.button.x;
                posSourisY = events.button.y;
                int posResX = positionEchelle[0].x + LargeurEcran / 50;
                int posResY = positionEchelle[0].y;

                cout << posResX << " " << posResY << endl;
                cout << posSourisX << " " << posSourisY << endl;

                if ((posSourisX > tabPositionTitre[3].x && posSourisY > tabPositionTitre[3].y) &&                                          // Point en haut à gauche
                    (posSourisX < (tabPositionTitre[3].x + LargeurEcran / 8) && (posSourisY > tabPositionTitre[3].y)) &&                   // Point en haut à droite
                    (posSourisX > tabPositionTitre[3].x && posSourisY < tabPositionTitre[3].y + HauteurEcran / 18) &&                      // Point en bas à gauche
                    (posSourisX < (tabPositionTitre[3].x + LargeurEcran / 8) && (posSourisY < tabPositionTitre[3].y + HauteurEcran / 18))) // Point en bas à droite.
                {
                    openMenu = true;
                    isOpen = false;
                }

                // Pour les résolutions
                if ((posSourisX > tabPositionRes[0].x && posSourisY > tabPositionRes[0].y) &&                                          // Point en haut à gauche
                    (posSourisX < (tabPositionRes[0].x + LargeurEcran / 8) && (posSourisY > tabPositionRes[0].y)) &&                   // Point en haut à droite
                    (posSourisX > tabPositionRes[0].x && posSourisY < tabPositionRes[0].y + HauteurEcran / 18) &&                      // Point en bas à gauche
                    (posSourisX < (tabPositionRes[0].x + LargeurEcran / 8) && (posSourisY < tabPositionRes[0].y + HauteurEcran / 18))) // Point en bas à droite.
                {
                    modifFichierRes(800, 800);
                    Mix_PlayChannel(1, sons[2], 0); // On joue le son valider 1 fois.
                    // Afficher : La résolution sera changé au redémarrage.
                }
                if ((posSourisX > tabPositionRes[1].x && posSourisY > tabPositionRes[1].y) &&                                          // Point en haut à gauche
                    (posSourisX < (tabPositionRes[1].x + LargeurEcran / 8) && (posSourisY > tabPositionRes[1].y)) &&                   // Point en haut à droite
                    (posSourisX > tabPositionRes[1].x && posSourisY < tabPositionRes[1].y + HauteurEcran / 18) &&                      // Point en bas à gauche
                    (posSourisX < (tabPositionRes[1].x + LargeurEcran / 8) && (posSourisY < tabPositionRes[1].y + HauteurEcran / 18))) // Point en bas à droite.
                {
                    // Afficher : La résolution sera changé au redémarrage.
                    modifFichierRes(1280, 720);
                    Mix_PlayChannel(1, sons[2], 0); // On joue le son valider 1 fois.
                }
                if ((posSourisX > tabPositionRes[2].x && posSourisY > tabPositionRes[2].y) &&                                          // Point en haut à gauche
                    (posSourisX < (tabPositionRes[2].x + LargeurEcran / 8) && (posSourisY > tabPositionRes[2].y)) &&                   // Point en haut à droite
                    (posSourisX > tabPositionRes[2].x && posSourisY < tabPositionRes[2].y + HauteurEcran / 18) &&                      // Point en bas à gauche
                    (posSourisX < (tabPositionRes[2].x + LargeurEcran / 8) && (posSourisY < tabPositionRes[2].y + HauteurEcran / 18))) // Point en bas à droite.
                {
                    // Afficher : La résolution sera changé au redémarrage.
                    modifFichierRes(1920, 1080);
                    Mix_PlayChannel(1, sons[2], 0); // On joue le son valider 1 fois.
                }
                if ((posSourisX > positionEchelle[0].x && posSourisY > positionEchelle[0].y)) // Si on est dans la partie basse vers l'echelle de son on regarde si on clique sur un nombre.
                {
                    for (int i = 0; i < 11; i++)
                    {
                        if ((posSourisX > positionEchelle[i].x && posSourisY > positionEchelle[i].y) &&                                           // Point en haut à gauche
                            (posSourisX < (positionEchelle[i].x + LargeurEcran / 50) && (posSourisY > positionEchelle[i].y)) &&                   // Point en haut à droite
                            (posSourisX > positionEchelle[i].x && posSourisY < positionEchelle[i].y + HauteurEcran / 18) &&                       // Point en bas à gauche
                            (posSourisX < (positionEchelle[i].x + LargeurEcran / 50) && (posSourisY < positionEchelle[i].y + HauteurEcran / 18))) // Point en bas à droite.
                        {
                            choixVolume = i;
                            if (i == 10)
                            {
                                sonSelectionne.x = positionEchelle[i].x - 5;
                                sonSelectionne.y = positionEchelle[i].y;
                                if (LargeurEcran == 1920 && HauteurEcran == 1080)
                                {
                                    sonSelectionne.w = LargeurEcran / 30;
                                    sonSelectionne.h = HauteurEcran / 20;
                                }
                                else if (LargeurEcran == 1280 && HauteurEcran == 720)
                                {
                                    sonSelectionne.w = LargeurEcran / 19;
                                    sonSelectionne.h = HauteurEcran / 15;
                                }
                                else
                                {
                                    sonSelectionne.w = LargeurEcran / 13;
                                    sonSelectionne.h = HauteurEcran / 15;
                                }
                            }
                            else
                            {
                                sonSelectionne.x = positionEchelle[i].x - LargeurEcran / 130;
                                sonSelectionne.y = positionEchelle[i].y;
                                if (LargeurEcran == 1920 && HauteurEcran == 1080)
                                {
                                    sonSelectionne.w = LargeurEcran / 40;
                                    sonSelectionne.h = HauteurEcran / 22;
                                }
                                else if (LargeurEcran == 1280 && HauteurEcran == 720)
                                {
                                    sonSelectionne.w = LargeurEcran / 30;
                                    sonSelectionne.h = HauteurEcran / 15;
                                }
                                else
                                {
                                    sonSelectionne.w = LargeurEcran / 20;
                                    sonSelectionne.h = HauteurEcran / 15;
                                }
                            }
                            Mix_PlayChannel(1, sons[2], 0);              // On joue le son valider 1 fois.
                            volume = MIX_MAX_VOLUME / 100 * choixVolume; // On met le son à 5 de base (Moitié).
                            Mix_Volume(0, volume);                       // On modifie le son du cannal actif.
                        }
                    }
                }
            }

            // Affiche l'image.
            // On met à jour l'affichage.
            SDL_SetRenderDrawColor(renderer, 254, 254, 254, 255); // Affiche la couleur de fond.
            SDL_RenderClear(renderer);

            fondMenu.draw(renderer, 0, 0, LargeurEcran, HauteurEcran);

            for (int i = 0; i < 4; i++)
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);                            // On passe sur la couleur du texte.
                SDL_RenderCopy(renderer, tabTextureTitre[i], nullptr, &tabPositionTitre[i]); // Affichage du texte.
            }
            for (int i = 0; i < 3; i++)
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);                     // On passe sur la couleur du texte.
                SDL_RenderCopy(renderer, tabTextRes[i], nullptr, &tabPositionRes[i]); // Affichage du texte.
            }

            for (int i = 0; i < 11; i++) // Echelle son [1...10]
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);                  // On passe sur la couleur du texte.
                SDL_RenderCopy(renderer, tabSon[i], nullptr, &positionEchelle[i]); // Affichage du texte.
            }

            // Affiche le rectangle qui indique le volume de la musique du Jeu.
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // On passe sur la couleur du texte.
            SDL_RenderDrawRects(renderer, &sonSelectionne, 1);

            SDL_RenderPresent(renderer);
        }
    }
    for (int i = 0; i < 4; i++)
    {
        SDL_DestroyTexture(tabTextureTitre[i]);
    }
    for (int i = 0; i < 3; i++)
    {
        SDL_DestroyTexture(tabTextRes[i]);
    }
    for (int i = 0; i < 10; i++)
        SDL_DestroyTexture(tabSon[i]);
    if (openMenu)
    {
        sdlMenu();
    }
}

void sdlJeu::situationUno(Jeu &jeu)
{
    bool attendre = true;
    SDL_Event event;
    int temps0 = SDL_GetTicks();
    while (attendre)
    {
        if (SDL_GetTicks() - temps0 < 2000) // temps inférieur à 2000 ms pour cliquer sur Uno
        {
            while (SDL_PollEvent(&event) && event.type == SDL_MOUSEBUTTONDOWN)
            {
                int sourisX = event.button.x;
                int sourisY = event.button.y;
                if (sourisX > (LargeurEcran / 3.35) && sourisX < (LargeurEcran / 2.63) && sourisY > (HauteurEcran / 3.34) && sourisY < (HauteurEcran / 2.5)) // clic sur Uno
                {
                    cout << "Uno" << endl;
                    jeu.statut_Uno = false;
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderClear(renderer);
                    SDL_Rect texte;
                    texte.x = LargeurEcran / 4.8;
                    texte.y = HauteurEcran / 3.6;
                    texte.w = LargeurEcran / 1.6;
                    texte.h = HauteurEcran / 2.7;
                    font_im.setSurface(TTF_RenderText_Solid(font, "UNOoooooo", jaune));
                    font_im.loadFromCurrentSurface(renderer);
                    SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);
                    SDL_RenderPresent(renderer);
                    SDL_Delay(1000);
                    attendre = false;
                }
            }
        }
        else
        {
            cout << "Pas cliqué sur Uno" << endl;
            attendre = false;
        }
    }
}

void sdlJeu::situationContreUno(Jeu &jeu)
{
    bool attendre = true;
    SDL_Event event;
    int temps0 = SDL_GetTicks();
    if (jeu.joueurActif >= jeu.nombreJoueurs)
    { // Si c'est le tour d'un des bots il fait un contre Uno direct.
        jeu.statut_Uno = false;
    }
    else
    { // C'est un Joueur Humain qui doit faire Contre Uno.
        while (attendre)
        {
            if (SDL_GetTicks() - temps0 < 3000) // temps inférieur à 3000 ms pour cliquer sur Contre Uno
            {
                while (SDL_PollEvent(&event) && event.type == SDL_MOUSEBUTTONDOWN)
                {
                    int sourisX = event.button.x;
                    int sourisY = event.button.y;
                    cout << "Contre Uno" << endl;
                    if (sourisX > (LargeurEcran * (127 / 192)) && sourisX < (LargeurEcran * 0.7432) && sourisY > (HauteurEcran * 0.299) && sourisY < (HauteurEcran * 0.40)) // clic sur Contre Uno
                    {
                        jeu.statut_Uno = false;
                        cout << jeu.statut_Uno << "après clic" << endl;
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        SDL_RenderClear(renderer);
                        SDL_Rect texte;
                        texte.x = LargeurEcran * 5 / 24;
                        texte.y = HauteurEcran * 5 / 18;
                        texte.w = LargeurEcran * 0.625;
                        texte.h = HauteurEcran * 10 / 27;
                        font_im.setSurface(TTF_RenderText_Solid(font, "Contre UNOoooooo", jaune));
                        font_im.loadFromCurrentSurface(renderer);
                        SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);
                        SDL_RenderPresent(renderer);
                        SDL_Delay(1000);
                        attendre = false;
                    }
                }
            }
            else
            {
                attendre = false;
            }
        }
    }
}

void sdlJeu::sdlEchange(Jeu &jeu)
{
    SDL_Window *choixEchange = SDL_CreateWindow("Echange", SDL_WINDOWPOS_CENTERED, (HauteurEcran * 5 / 27), (LargeurEcran * 25 / 48), (HauteurEcran * 5 / 27), SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (choixEchange == NULL)
    {
        cout << "Erreur lors de la creation de la fenetre : " << SDL_GetError() << endl;
        SDL_Quit();
        exit(1);
    }
    SDL_Renderer *rendererEchange = SDL_CreateRenderer(choixEchange, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(rendererEchange, 0, 0, 255, 255);
    SDL_RenderClear(rendererEchange);

    SDL_Rect texte;
    texte.x = LargeurEcran * 25 / 192;
    texte.y = HauteurEcran * 5 / 216;
    texte.w = LargeurEcran * 25 / 96;
    texte.h = HauteurEcran * 5 / 108;
    font_im.setSurface(TTF_RenderText_Solid(font, "Echange avec le joueur", noir));
    font_im.loadFromCurrentSurface(rendererEchange);
    SDL_RenderCopy(rendererEchange, font_im.getTexture(), NULL, &texte);
    unsigned int pos = ((LargeurEcran * 25 / 48) - (LargeurEcran * 5 / 96) * (jeu.nombreJoueurs + jeu.nombreIA - 1)) / 2;
    int indiceBot = 0;
    for (unsigned int i = 0; i < ((jeu.nombreJoueurs + jeu.nombreIA) -1); i++)
    {
        texte.x = pos + (LargeurEcran * 5 / 96) * i;
        texte.y = HauteurEcran * 25 / 216;
        texte.w = (LargeurEcran * 5 / 96);
        texte.h = HauteurEcran * 5 / 108;
        cout << i << endl;
        if (i < jeu.joueurActif)
        {
            cout << "Affiche avant le joueur actuel" << endl;
            font_im.setSurface(TTF_RenderText_Solid(font, jeu.joueurs[i].nom.c_str(), noir));
        }
        else
        {
            if (i < jeu.nombreJoueurs - 1 && jeu.nombreJoueurs != 1)
            {           // On doit afficher les joueurs après le joueur actif.
                //i += 1; // On ajoute un 1 pour réduire
                cout << "Affiche le nom des joueurs après celui actuel" << endl;
                font_im.setSurface(TTF_RenderText_Solid(font, jeu.joueurs[i + 1].nom.c_str(), noir));
            }
            else if (i >= jeu.nombreJoueurs - 1 && i != 0)
            { // Si on
                cout << "Affiche les noms des bots après celui actuel" << endl;
                if (indiceBot == 0)
                {
                    font_im.setSurface(TTF_RenderText_Solid(font, jeu.joueursBot[indiceBot].nom.c_str(), noir));
                    indiceBot += 1;
                }
                else
                {
                    font_im.setSurface(TTF_RenderText_Solid(font, jeu.joueursBot[indiceBot].nom.c_str(), noir));
                    indiceBot += 1;
                }
            }
            else if (jeu.nombreJoueurs == 1 && i == 0)
            { // Si on a 1 seul Joueur et au moins 1 bots.
                cout << "Un seul Joueur avec plus de 1 bot" << endl;
                font_im.setSurface(TTF_RenderText_Solid(font, jeu.joueursBot[i].nom.c_str(), noir));
            }
        }
        font_im.loadFromCurrentSurface(rendererEchange);
        SDL_RenderCopy(rendererEchange, font_im.getTexture(), NULL, &texte);
    }
    SDL_RenderPresent(rendererEchange);

    SDL_Event event;
    bool choixFait = false;
    unsigned int sourisX;
    int sourisY;
    unsigned int indice;

    while (!choixFait)
    {
        while (SDL_PollEvent(&event) && event.type == SDL_MOUSEBUTTONDOWN)
        {
            sourisX = event.button.x;
            sourisY = event.button.y;
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if ((sourisX > pos && sourisX < pos + (LargeurEcran * 25 / 192) * (jeu.nombreJoueurs - 1)) && (sourisY > (HauteurEcran * 25 / 216) && sourisY < (HauteurEcran * 35 / 216))) // clic sur un nom
                {
                    indice = (sourisX - pos) / (LargeurEcran * 25 / 192);
                    if (indice >= jeu.joueurActif)
                        indice++;
                    choixFait = true;
                }
            }
        }
    }
    jeu.joueurs[jeu.joueurActif].main.swap(jeu.joueurs[indice].main);
    jeu.casPart = -1;
    jeu.termineTour();
    SDL_DestroyRenderer(rendererEchange);
    SDL_DestroyWindow(choixEchange);
}

void sdlJeu::sdlDoublon(Jeu &jeu)
{
    int indice = -1;
    for (unsigned int i = 0; i < jeu.joueurs[jeu.joueurActif].main.size(); i++)
    {
        if ((jeu.talon.back().getValeur() == 13 && jeu.joueurs[jeu.joueurActif].main[i].getValeur() == 13) || (jeu.talon.back().getValeur() == 14 && jeu.joueurs[jeu.joueurActif].main[i].getValeur() == 14)) //cas joker et +4, pas besoin de tester la couleur
        {
            indice = i;
        }
        else if (jeu.talon.back().getValeur() == jeu.joueurs[jeu.joueurActif].main[i].getValeur() && jeu.talon.back().getCouleur() == jeu.joueurs[jeu.joueurActif].main[i].getCouleur())
        {
            indice = i;
        }
    }

    if (indice != -1)
    {
        SDL_Window *choixDoublon = SDL_CreateWindow("Doublon", SDL_WINDOWPOS_CENTERED, (HauteurEcran * 7 / 27), (LargeurEcran * 25 / 48), (HauteurEcran * 5 / 27), SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (choixDoublon == NULL)
        {
            cout << "Erreur lors de la creation de la fenetre : " << SDL_GetError() << endl;
            SDL_Quit();
            exit(1);
        }

        SDL_Renderer *rendererDoublon = SDL_CreateRenderer(choixDoublon, -1, SDL_RENDERER_ACCELERATED);
        SDL_SetRenderDrawColor(rendererDoublon, 255, 0, 0, 255);
        SDL_RenderClear(rendererDoublon);

        SDL_Rect texte;
        texte.x = LargeurEcran * 25 / 192;
        texte.y = HauteurEcran * 5 / 216;
        texte.w = LargeurEcran * 25 / 96;
        texte.h = HauteurEcran * 5 / 108;
        font_im.setSurface(TTF_RenderText_Solid(font, "Jouer le doublon", noir));
        font_im.loadFromCurrentSurface(rendererDoublon);
        SDL_RenderCopy(rendererDoublon, font_im.getTexture(), NULL, &texte);
        //int pos = ((LargeurEcran * 25 / 48) - (LargeurEcran * 5 / 96) * (jeu.nombreJoueurs - 1)) / 2;
        texte.x = LargeurEcran * 35 / 192;
        texte.y = HauteurEcran * 125 / 108;
        texte.w = (LargeurEcran * 5 / 96);
        texte.h = HauteurEcran * 5 / 108;
        font_im.setSurface(TTF_RenderText_Solid(font, "oui", noir));
        font_im.loadFromCurrentSurface(rendererDoublon);
        SDL_RenderCopy(rendererDoublon, font_im.getTexture(), NULL, &texte);
        texte.x = LargeurEcran * 55 / 192;
        texte.y = HauteurEcran * 25 / 216;
        texte.w = (LargeurEcran * 5 / 96);
        texte.h = HauteurEcran * 5 / 108;
        font_im.setSurface(TTF_RenderText_Solid(font, "non", noir));
        font_im.loadFromCurrentSurface(rendererDoublon);
        SDL_RenderCopy(rendererDoublon, font_im.getTexture(), NULL, &texte);

        SDL_RenderPresent(rendererDoublon);

        SDL_Event event;
        bool choixFait = false;
        int sourisX;
        int sourisY;

        while (!choixFait)
        {
            while (SDL_PollEvent(&event) && event.type == SDL_MOUSEBUTTONDOWN)
            {
                sourisX = event.button.x;
                sourisY = event.button.y;
                if (sourisX > (LargeurEcran * 35 / 192) && sourisX < (LargeurEcran * 15 / 64) && sourisY > (HauteurEcran * 25 / 216) && sourisY < (HauteurEcran * 35 / 216)) // clic sur oui
                {
                    jeu.talon.push(jeu.joueurs[jeu.joueurActif].main[indice]); // On pousse le doublon.
                    jeu.joueurs[jeu.joueurActif].main.erase(jeu.joueurs[jeu.joueurActif].main.begin() + indice);
                    choixFait = true;
                }
                if (sourisX > (LargeurEcran * 55 / 192) && sourisX < (LargeurEcran * 65 / 192) && sourisY > (HauteurEcran * 25 / 216) && sourisY < (HauteurEcran * 35 / 216)) // clic sur non
                {
                    choixFait = true;
                }
            }
        }
        SDL_DestroyRenderer(rendererDoublon);
        SDL_DestroyWindow(choixDoublon);
    }
    jeu.casPart = -1;
    jeu.termineTour();
}

// la procédure est appelée parce que la carte dont l'indice est passé en paramètre fait bien partie d'une suite de 0 à 7
void sdlJeu::sdlSuite(Jeu &jeu, unsigned int indice)
{
    SDL_Window *choixSuite = SDL_CreateWindow("Suite", 0, HauteurEcran * 27 / 7, LargeurEcran * 19 / 64, HauteurEcran * 5 / 27, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (choixSuite == NULL)
    {
        cout << "Erreur lors de la creation de la fenetre : " << SDL_GetError() << endl;
        SDL_Quit();
        exit(1);
    }

    SDL_Renderer *rendererSuite = SDL_CreateRenderer(choixSuite, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(rendererSuite, 0, 255, 0, 255);
    SDL_RenderClear(rendererSuite);

    SDL_Rect texte;
    texte.x = LargeurEcran * 7 / 384;
    texte.y = HauteurEcran * 5 / 216;
    texte.w = LargeurEcran * 25 / 96;
    texte.h = HauteurEcran * 5 / 108;
    font_im.setSurface(TTF_RenderText_Solid(font, "Suite: Jouer la carte suivante", noir));
    font_im.loadFromCurrentSurface(rendererSuite);
    SDL_RenderCopy(rendererSuite, font_im.getTexture(), NULL, &texte);
    //int pos = ((LargeurEcran * 25 / 48) - (LargeurEcran * 5 / 96) * (jeu.nombreJoueurs - 1)) / 2;
    texte.x = LargeurEcran * 9 / 128;
    texte.y = HauteurEcran * 25 / 215;
    texte.w = LargeurEcran * 5 / 96;
    texte.h = HauteurEcran * 5 / 108;
    font_im.setSurface(TTF_RenderText_Solid(font, "oui", noir));
    font_im.loadFromCurrentSurface(rendererSuite);
    SDL_RenderCopy(rendererSuite, font_im.getTexture(), NULL, &texte);
    texte.x = LargeurEcran * 67 / 384;
    texte.y = HauteurEcran * 25 / 216;
    texte.w = LargeurEcran * 5 / 96;
    texte.h = HauteurEcran * 5 / 108;
    font_im.setSurface(TTF_RenderText_Solid(font, "non", noir));
    font_im.loadFromCurrentSurface(rendererSuite);
    SDL_RenderCopy(rendererSuite, font_im.getTexture(), NULL, &texte);

    SDL_RenderPresent(rendererSuite);

    SDL_Event event;
    bool choixFait = false;
    int sourisX;
    int sourisY;
    unsigned int carteTalon = jeu.talon.back().getValeur();

    while (!choixFait && jeu.talon.back().getValeur() < 7) // le joueur peut jouer la carte suivante jusqu'à 7
    {
        while (SDL_PollEvent(&event) && event.type == SDL_MOUSEBUTTONDOWN)
        {
            sourisX = event.button.x;
            sourisY = event.button.y;
            if (sourisX > (LargeurEcran * 9 / 128) && sourisX < (LargeurEcran * 29 / 128) && sourisY > (HauteurEcran * 25 / 216) && sourisY < (HauteurEcran * 35 / 216)) // clic sur oui
            {
                if (jeu.joueurs[jeu.joueurActif].main[indice].getValeur() == carteTalon)
                    indice++; // il a le doublon de la carte du talon, on la passe

                jeu.talon.push(jeu.joueurs[jeu.joueurActif].main[indice]); // On pousse le doublon.
                jeu.joueurs[jeu.joueurActif].main.erase(jeu.joueurs[jeu.joueurActif].main.begin() + indice);
                carteTalon++;
            }
            if (sourisX > (LargeurEcran * 55 / 192) && sourisX < (LargeurEcran * 65 / 192) && sourisY > (HauteurEcran * 25 / 216) && sourisY < (HauteurEcran * 35 / 216)) // clic sur non
            {
                choixFait = true;
            }
        }
    }
    SDL_DestroyRenderer(rendererSuite);
    SDL_DestroyWindow(choixSuite);

    jeu.casPart = -1;
    jeu.termineTour();
}

void sdlJeu::sdlBoucleJeu(Jeu &jeu)
{
    //Remplir l'écran de noir
    SDL_SetRenderDrawColor(renderer, 254, 254, 254, 255);
    SDL_RenderClear(renderer);

    fondMenu.draw(renderer, 0, 0, LargeurEcran, HauteurEcran);
    SDL_RenderPresent(renderer);

    //sdlAffJoueurActif(jeu);

    SDL_Event event;
    bool quit = false;
    int sourisX;
    int sourisY;
    unsigned int couleur = 0;
    bool couleurChangee = false;
    bool joueurChange = true;
    unsigned int indiceJoueur = jeu.joueurActif;
    int compteurBotUno = 0;

    cout << jeu.joueurActif << endl;
    // tant que ce n'est pas la fin ...
    while (!quit)
    {
        cout << "Joueur actif debut boucle" << endl;
        cout << jeu.joueurActif << endl;
        if (jeu.joueurActif >= jeu.nombreJoueurs)
        {
            if (jeu.joueursBot[jeu.joueurActif - jeu.nombreJoueurs].main.size() == 0)
            {
                cout << "Le bot a plus de carte c'est la fin du Jeu." << endl;
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);
                SDL_Rect texte;
                texte.x = 400;
                texte.y = 100;
                texte.w = 1200;
                texte.h = 400;
                font_im.setSurface(TTF_RenderText_Solid(font, "Good Game", jaune));
                font_im.loadFromCurrentSurface(renderer);
                SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);
                texte.x = 500;
                texte.y = 500;
                texte.w = 800;
                texte.h = 400;
                font_im.setSurface(TTF_RenderText_Solid(font, jeu.joueursBot[jeu.joueurActif].nom.c_str(), jaune));
                font_im.loadFromCurrentSurface(renderer);
                SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);
                SDL_RenderPresent(renderer);
                SDL_Delay(2000);
                quit = true;
            }
            if (jeu.statut_Uno)
            {
                if (indiceJoueur < jeu.nombreJoueurs)
                {                            // C'était un joueur Humain qui jouait juste avant.
                    situationContreUno(jeu); // Le bot va Contre Uno direct.
                }
                else // Le joueur d'avant était un bot.
                {
                    indiceJoueur = jeu.joueurActif; // On save l'indice du bot qui est en Uno.
                    jeu.termineTour();              // On termine le tour pour que ce soit un autre bot ou un joueur qui puisse faire Contre Uno.
                    if (compteurBotUno != 1)
                    {                            // Si le bot suivant n'a jamais fait un contre Uno automatique.
                        situationContreUno(jeu); // Il contre Uno direct.
                        compteurBotUno++;
                    }
                }
                if (!jeu.statut_Uno && indiceJoueur < jeu.nombreJoueurs)
                { // Si le statut est passé à false (on a appuyé sur le contre Uno)
                    jeu.joueurs[indiceJoueur].main.push_back(jeu.pioche.top());
                    jeu.pioche.pop();
                    jeu.joueurs[indiceJoueur].main.push_back(jeu.pioche.top());
                    jeu.pioche.pop();
                }
                else if (!jeu.statut_Uno && indiceJoueur >= jeu.nombreJoueurs)
                {
                    jeu.joueursBot[indiceJoueur].main.push_back(jeu.pioche.top());
                    jeu.pioche.pop();
                    jeu.joueursBot[indiceJoueur].main.push_back(jeu.pioche.top());
                    jeu.pioche.pop();
                }
                else // Si le statut est encore à true alors on a pas appuyé donc on ne le fait pas piocher.
                    jeu.statut_Uno = false;
            }
            cout << "Bot actif Joue" << endl;
            jeu.joueursBot[jeu.joueurActif - jeu.nombreJoueurs].choixJeu(jeu);
            if (jeu.talon.back().getValeur() == 13 || jeu.talon.back().getValeur() == 14)
            { // On affiche la nouvelle couleur posé par le bot quand c'est un +4 ou joker.
                couleur = jeu.talon.back().getCouleur();
                couleurChangee = true;
            }
            continue;
        }
        if (joueurChange) // pour éviter le clignotement de la carte couleur, ne pas actualiser l'affichage
        {
            if (jeu.joueurActif < jeu.nombreJoueurs)
            {
                sdlAffJoueur(jeu, jeu.joueurActif);
                if (couleur != 0 && couleurChangee)
                {
                    sdlAffCouleurChoisie(couleur);
                }
            }
            if (jeu.statut_Uno)
            {
                situationContreUno(jeu);
                if (!jeu.statut_Uno)
                {
                    cout << "Fait piocher" << endl;
                    jeu.joueurs[indiceJoueur].main.push_back(jeu.pioche.top());
                    jeu.pioche.pop();
                    jeu.joueurs[indiceJoueur].main.push_back(jeu.pioche.top());
                    jeu.pioche.pop();
                }
                else if (jeu.nombreIA != 0)
                { // Si il y a des bots en plus du Joueur n mais que le Joueur n n'a pas appuyé sur contre Uno les bots contre Uno direct.
                    cout << "Le bot fait piocher le joueur alors que c'est le tour du Joueur n" << endl;
                    jeu.joueurs[indiceJoueur].main.push_back(jeu.pioche.top());
                    jeu.pioche.pop();
                    jeu.joueurs[indiceJoueur].main.push_back(jeu.pioche.top());
                    jeu.pioche.pop();
                    jeu.statut_Uno = false;
                }
                else
                {
                    jeu.statut_Uno = false;
                }
            }
            // on exclut le cas particulier de la version cumul
            if (jeu.casPart % 2 == 1)
                indiceJoueur = jeu.joueurActif;
            joueurChange = false;
        }

        while (SDL_PollEvent(&event) && jeu.joueurActif < jeu.nombreJoueurs)
        {
            sourisX = event.button.x;
            sourisY = event.button.y;
            switch (event.type)
            {
            case SDL_QUIT: // Si l'utilisateur a cliqué sur la croix de fermeture
                quit = true;
                break;
            case SDL_KEYDOWN: // Si une touche est enfoncée
                if (event.key.keysym.scancode == SDL_SCANCODE_Q)
                    quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN: // Si le bouton de la souris est appuyé
                joueurChange = true;
                if (sourisX > (LargeurEcran * 5 / 12) && sourisX < (LargeurEcran * 91 / 192) && sourisY > (HauteurEcran * 5 / 18) && sourisY < (HauteurEcran * 457 / 1080)) // clic sur la pioche
                {
                    jeu.piocherCarte();
                    sdlAffJoueur(jeu, jeu.joueurActif);
                    // variante cumul, le joueur actif ne peut pas jouer
                    if (jeu.casPart % 2 == 0 && jeu.casPart > 0 && jeu.talon.back().getValeur() == 13)
                    {
                        sdlAffJoueur(jeu, indiceJoueur); // on revient au joueur précédent pour qu'il choisisse la couleur
                        couleur = choixCouleur();
                        couleurChangee = true;
                        jeu.talon.back().setCouleur(couleur);
                    }
                    if (jeu.casPart % 2 == 1)
                        jeu.casPart = -1;
                    else if (jeu.casPart % 2 == 0)
                    {
                        jeu.casPart = 0;
                        indiceJoueur = jeu.joueurActif;
                    }
                    cout << "casPart après pioche " << jeu.casPart << endl;
                    jeu.termineTour();

                    if (couleur != 0 && couleurChangee)
                    {
                        sdlAffCouleurChoisie(couleur);
                        couleur = 0;
                        couleurChangee = false;
                    }
                }
                if (sourisX > 0 && sourisX < LargeurEcran / 1.05 && sourisY > HauteurEcran / 1.8 && sourisY < HauteurEcran / 1.15) // clic sur une carte de la main
                {
                    couleur = 0;
                    string messageErreur;
                    unsigned int indiceCarte;
                    if (sourisY < HauteurEcran / 1.43)
                        indiceCarte = sourisX / 110;
                    if (sourisY > HauteurEcran / 1.35)
                        indiceCarte = 17 + sourisX / 110;
                    if (jeu.carteValide(jeu.joueurs[jeu.joueurActif].main[indiceCarte]))
                    {
                        couleurChangee = false;
                        if (jeu.joueurs[jeu.joueurActif].main[indiceCarte].getValeur() == 13 || jeu.joueurs[jeu.joueurActif].main[indiceCarte].getValeur() == 14)
                        {
                            couleurChangee = false;
                            if ((jeu.joueurs[jeu.joueurActif].main[indiceCarte].getValeur() == 13 && jeu.casPart % 2 != 0) || jeu.joueurs[jeu.joueurActif].main[indiceCarte].getValeur() == 14)
                            {
                                couleur = choixCouleur();
                                couleurChangee = true;
                                jeu.joueurs[jeu.joueurActif].main[indiceCarte].setCouleur(couleur);
                            }
                        }
                        indiceJoueur = jeu.joueurActif;
                        cout << "L'indice du joueur est " << indiceJoueur << endl;
                        // pour la variante cumul
                        if (jeu.casPart % 2 == 0)
                            indiceJoueur = jeu.joueurActif;
                        jeu.poserCarte(indiceCarte, messageErreur);
                        // pour les variantes doublon, echange et suite
                        if (jeu.casPart == 5)
                            sdlSuite(jeu, indiceCarte);
                        if (jeu.casPart == 3)
                        {
                            cout << "Debut affichage echange" << endl;
                            sdlEchange(jeu);
                        }
                        if (jeu.casPart == 1)
                            sdlDoublon(jeu);
                        sdlAffJoueur(jeu, indiceJoueur);
                        if (couleur != 0 && couleurChangee)
                        {
                            sdlAffCouleurChoisie(couleur);
                            couleur = 0;
                            couleurChangee = false;
                        }
                        if (jeu.statut_Uno)
                        {
                            cout << "Situation Uno" << endl;
                            situationUno(jeu);
                        }
                    }
                }
                break;
            default:
                break;
            }
        }
        cout << "Apres Poll Event" << endl;
        SDL_Delay(500);
        // Teste s'il y a un gagnant
        if (jeu.joueurs[indiceJoueur].main.size() == 0 && indiceJoueur < jeu.nombreJoueurs)
        {
            cout << "Joueur gagnant ?" << endl;
            cout << indiceJoueur << endl;
            cout << jeu.joueurs[jeu.joueurActif].main.size() << endl;
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_Rect texte;
            texte.x = 400;
            texte.y = 100;
            texte.w = 1200;
            texte.h = 400;
            font_im.setSurface(TTF_RenderText_Solid(font, "Good Game", jaune));
            font_im.loadFromCurrentSurface(renderer);
            SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);
            texte.x = 500;
            texte.y = 500;
            texte.w = 800;
            texte.h = 400;
            font_im.setSurface(TTF_RenderText_Solid(font, jeu.joueurs[indiceJoueur].nom.c_str(), jaune));
            font_im.loadFromCurrentSurface(renderer);
            SDL_RenderCopy(renderer, font_im.getTexture(), NULL, &texte);
            SDL_RenderPresent(renderer);
            SDL_Delay(2000);
            quit = true;
        }
    }
}
