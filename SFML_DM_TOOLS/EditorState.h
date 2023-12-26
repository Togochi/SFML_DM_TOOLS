#ifndef EDITORSTATE_H
#define EDITORSTATE_H

#include "State.h"
#include "PauseMenu.h"
#include "TileMap.h"


class EditorState :
    public State
{
private:
    //Variables
    sf::View view;

    sf::Font font;
    sf::Text cursorText;
    PauseMenu* pmenu;
    TileMap* tileMap;

    std::map <std::string, gui::Button*> buttons;
    sf::RectangleShape selectorRect;
    sf::IntRect textureRect;
    gui::TextureSelector* textureSelector;
    sf::RectangleShape sidebar;

    float cameraSpeed;
    bool collision;
    short type;
    int layer;
    bool tileAddLock;

    //Functions
    void initVariables();
    void initView();
    void initBackground();
    void initFonts();
    void initText();
    void initKeybinds();
    void initPauseMenu();
    void initGui();
    void initButtons();
    void initTileMap();
public:
    EditorState(StateData* state_data);
    virtual ~EditorState();

    //Functions
    void updateInput(const float& dt);
    void upateEditorInput(const float& dt);
    void updateButtons();
    void updateGui(const float& dt);
    void updatePauseMenuButtons();
    void update(const float& dt);

    void render(sf::RenderTarget* target = NULL);
    void renderButtons(sf::RenderTarget& target);
    void renderGui(sf::RenderTarget& target);

};

#endif

