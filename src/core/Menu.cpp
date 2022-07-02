#include <Menu.h>

Menu::Menu()
{ // On initialise les tableaux avec les chaines :
    initNomMenu();
    initNomOptions();
    initResolutionOptions();
    initSonOptions();
}
void Menu::initNomMenu()
{
    string conteneurTemp[4] = {"Jouer", "Reglages", "Regles", "Quitter"};
    for (int i = 0; i < 4; i++)
    {
        nomMenu.insert(nomMenu.begin() + i, conteneurTemp[i]);
    }
}
void Menu::initNomOptions()
{
    string conteneurTemp[4] = {"Resolutions", "Changer Pseudo", "Son", "Retour"};
    for (int i = 0; i < 4; i++)
        nomOptions.insert(nomOptions.begin() + i, conteneurTemp[i]);
}
void Menu::initResolutionOptions()
{
    string conteneurTemp[3] = {"800 x 800","1280 x 720", "1920 x 1080"};
    for (int i = 0; i < 3; i++)
        resolutionOptions.insert(resolutionOptions.begin() + i, conteneurTemp[i]);
}
void Menu::initSonOptions()
{
    for (int i = 0; i < 11; i++)
    {
        sonOptions.insert(sonOptions.begin() + i, to_string(i));
    }
}

Menu::~Menu()
{
}

void Menu::testRegression()
{
    cout << "Test init des tableaux :" << endl;

    for (auto &i : nomMenu)
        cout << i << endl;

    for (auto &i : nomOptions)
        cout << i << endl;

    for (auto &i : resolutionOptions)
        cout << i << endl;

    for (auto &i : sonOptions)
        cout << i << endl;
}
string Menu::getNomMenu(int i)
{
    return nomMenu[i];
}
string Menu::getOptions(int i)
{
    return nomOptions[i];
}
string Menu::getResolution(int i)
{
    return resolutionOptions[i];
}
string Menu::getSon(int i)
{
    return sonOptions[i];
}