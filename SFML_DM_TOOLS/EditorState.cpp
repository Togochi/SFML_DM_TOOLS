#include "stdafx.h"
#include "EditorState.h"

//Initializing functions

void EditorState::initVariables()
{
	this->textureRect = sf::IntRect(0, 0, static_cast<int> (this->stateData->gridSize), static_cast<int> (this->stateData->gridSize));
	this->fill = false;
	this->type = TileTypes::DEFAULT;
	this->cameraSpeed = 1000.f;
	this->layer = 0;
	this->tileAddLock = false;
}

void EditorState::initView()
{
	this->view.setSize(sf::Vector2f(
		static_cast<float> (this->stateData->gfxSettings->resolution.width),
		static_cast<float>  (this->stateData->gfxSettings->resolution.height)));
	this->view.setCenter(sf::Vector2f(
		static_cast<float> (this->stateData->gfxSettings->resolution.width) / 2.f,
		static_cast<float> (this->stateData->gfxSettings->resolution.height) / 2.f));
}

void EditorState::initBackground()
{

}

void EditorState::initFonts()
{
	if (!this->font.loadFromFile("Fonts/Dosis-Light.ttf"))
	{
		std::cerr << "ERROR::EDITORSTATE::INITFONTS: COULD NOT LOAD FONT";
	}
}

void EditorState::initText()
{
	this->cursorText.setFont(this->font);
	this->cursorText.setCharacterSize(12);
	this->cursorText.setFillColor(sf::Color::White);
}

void EditorState::initKeybinds()
{

	std::ifstream ifs("Config/editorstate_keybinds.ini");
	if (ifs.is_open())
	{
		std::string key = "";
		std::string key2 = "";

		while (ifs >> key >> key2)
		{
			this->keybinds[key] = this->supportedKeys->at(key2);
		}
	}

	ifs.close();

}

void EditorState::initPauseMenu()
{
	const sf::VideoMode& vm = this->stateData->gfxSettings->resolution;

	this->pmenu = new PauseMenu(this->stateData->gfxSettings->resolution, this->font);

	this->pmenu->addButton("QUIT", gui::p2pY(74.f, vm), gui::p2pX(13.f, vm), gui::p2pY(6.f, vm), gui::calcCharSize(vm), "Quit");
	this->pmenu->addButton("LOAD", gui::p2pY(46.f, vm), gui::p2pX(13.f, vm), gui::p2pY(6.f, vm), gui::calcCharSize(vm), "Load ts/map");
	this->pmenu->addButton("SAVE", gui::p2pY(37.f, vm), gui::p2pX(13.f, vm), gui::p2pY(6.f, vm), gui::calcCharSize(vm), "Save map as");
}

void EditorState::initGui()
{
	this->sidebar.setSize(sf::Vector2f(static_cast<float>(this->stateData->gfxSettings->resolution.width), 40.f) );
	this->sidebar.setFillColor(sf::Color(50, 50, 50, 0));
	this->sidebar.setOutlineColor(sf::Color(200, 200, 200, 150));
	this->sidebar.setOutlineThickness(1.f);

	this->selectorRect.setSize(sf::Vector2f(this->stateData->gridSize, this->stateData->gridSize));
	this->selectorRect.setFillColor(sf::Color(255, 255, 255, 150));
	this->selectorRect.setOutlineThickness(1.f);
	this->selectorRect.setOutlineColor(sf::Color::Green);
}


void EditorState::initTileGui()
{
	this->selectorRect.setTexture(this->tileMap->getTileSheet());
	this->selectorRect.setTextureRect(this->textureRect);
	this->textureSelector = new gui::TextureSelector(5.f, 41.f, this->stateData->gfxSettings->resolution.width, this->stateData->gfxSettings->resolution.height, this->stateData->gridSize, this->tileMap->getTileSheet(), this->font, "Textures");
}

void EditorState::initButtons()
{

}

void EditorState::initTileMap(std::string file_name)
{
	file_name = file_name.substr(0, file_name.length() - 4);
	this->tileMap = new TileMap(this->stateData->gridSize, 1000, 1000, "Resources/Images/Tiles/" + file_name + ".png");
}

EditorState::EditorState(StateData* state_data)
	:State(state_data)
{
	this->initVariables();
	this->initView();
	this->initBackground();
	this->initFonts();
	this->initText();
	this->initKeybinds();
	this->initPauseMenu();
	this->initButtons();
	
	this->initGui();
	this->textureSelector = NULL;
	this->tileMap = NULL;
}

EditorState::~EditorState()
{
	auto it = this->buttons.begin();
	for (it = this->buttons.begin(); it != this->buttons.end(); ++it)
	{
		delete it->second;
	}
	delete this->pmenu;

	if (this->tileMap)
		delete this->tileMap;
	if (this->textureSelector)
		delete this->textureSelector;
}

std::string EditorState::enumToString(short type)
{
	switch (type)
	{
	case 0:
		return "DEFAULT";
	case 1:
		return "RED";
	case 2:
		return "GREEN";
	case 3:
		return "BLUE";
	case 4:
		return "YELLOW";
	case 5:
		return "WHITE";
	case 6:
		return "GREY";
	case 7:
		return "ORANGE";
	case 8:
		return "PURPLE";
	case 9:
		return "TRANSPARENT";
	default:
		return "?";
	}
}



void EditorState::updateInput(const float& dt)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->keybinds.at("CLOSE"))) && this->getKeytime())
	{
		if (!this->paused)
			this->pauseState();
		else
			this->unpauseState();
	}
}

void EditorState::upateEditorInput(const float& dt)
{

	if (!this->textbox1->getSelect())
	{
		//Move view
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->keybinds.at("MOVE_CAMERA_UP"))))
		{
			this->view.move(0.f, -(this->cameraSpeed * dt));
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->keybinds.at("MOVE_CAMERA_DOWN"))))
		{
			this->view.move(0.f, this->cameraSpeed * dt);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->keybinds.at("MOVE_CAMERA_LEFT"))))
		{
			this->view.move(-(this->cameraSpeed * dt), 0.f);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->keybinds.at("MOVE_CAMERA_RIGHT"))))
		{
			this->view.move(this->cameraSpeed * dt, 0.f);
		}
	}

	// Add a tile to the Tilemap
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && this->getKeytime())
	{
		if (!this->sidebar.getGlobalBounds().contains(sf::Vector2f(this->mousePosWindow)))
		{
			if (!this->textureSelector->getActive())
			{
				if (this->tileAddLock)
				{
					if (this->tileMap->tileEmpty(this->mousePosGrid.x, this->mousePosGrid.y, 0))
						if (this->charContains)
						{
							this->tileMap->addTile(this->mousePosGrid.x, this->mousePosGrid.y, 0, this->textureRect, this->fill, this->type, this->charContains, this->showText, this->font, this->textbox1->getText());

						}
						else
						{
							this->tileMap->addTile(this->mousePosGrid.x, this->mousePosGrid.y, 0, this->textureRect, this->fill, this->type, this->charContains, this->showText, this->font, "_");
						}

				}
				else
				{
					if (this->charContains)
					{
						this->tileMap->addTile(this->mousePosGrid.x, this->mousePosGrid.y, 0, this->textureRect, this->fill, this->type, this->charContains, this->showText, this->font,  this->textbox1->getText());
						this->tileMap->setShowText(this->mousePosGrid.x, this->mousePosGrid.y, 0, 0, this->showText);
					}
					else
					{
						this->tileMap->addTile(this->mousePosGrid.x, this->mousePosGrid.y, 0, this->textureRect, this->fill, this->type, this->charContains, this->showText, this->font, "_");
					}
				}

			}
			else
			{
				this->textureRect = this->textureSelector->getTextureRect();
			}
		}
	}

	// Remove a tile from the Tilemap
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && this->getKeytime())
	{
		if (!this->sidebar.getGlobalBounds().contains(sf::Vector2f(this->mousePosWindow)))
		{
			if (!this->textureSelector->getActive())
				this->tileMap->removeTile(this->mousePosGrid.x, this->mousePosGrid.y, 0);
		}
	}

	if (!this->textbox1->getSelect())
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->keybinds.at("TOGGLE_FILL"))) && this->getKeytime())
		{
			if (this->fill)
				this->fill = false;
			else
				this->fill = true;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->keybinds.at("INCREASE_TYPE"))) && this->getKeytime())
		{
			if (this->type < 9)
				++this->type;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->keybinds.at("DECREASE_TYPE"))) && this->getKeytime())
		{
			if (this->type > 0)
				--this->type;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->keybinds.at("TOGGLE_TEXT"))) && this->getKeytime())
		{
			if (this->charContains)
				this->charContains = false;
			else
				this->charContains = true;
		}


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->keybinds.at("TOGGLE_HIDE_TEXT"))) && this->getKeytime())
		{
			if (this->showText)
				this->showText = false;
			else
				this->showText = true;
		}

		//Set lock on / off
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->keybinds.at("TOGGLE_TILE_LOCK"))) && this->getKeytime())
		{
			if (this->tileAddLock)
				this->tileAddLock = false;
			else
				this->tileAddLock = true;
		}
	}

}

void EditorState::updateButtons(const float & dt)
{
	//Updates all buttons in the state and handles their functionality
	for (auto& it : this->buttons)
	{
		it.second->update(this->mousePosWindow, dt);
	}
}

void EditorState::updateGui(const float& dt)
{
	this->textureSelector->update(this->mousePosWindow, dt);

	if (!this->textureSelector->getActive())
	{
		this->selectorRect.setTextureRect(this->textureRect);
		this->selectorRect.setPosition(this->mousePosGrid.x * this->stateData->gridSize, this->mousePosGrid.y * this->stateData->gridSize);
	}

	this->cursorText.setPosition(this->mousePosView.x + 100.f, this->mousePosView.y - 50);
	std::stringstream ss;
	ss << this->mousePosView.x << ' ' << this->mousePosView.y << '\n'
		<< this->mousePosGrid.x << ' ' << this->mousePosGrid.y << '\n'
		<< this->textureRect.left << ' ' << this->textureRect.top << '\n'
		<< "Fill: " << this->fill << '\n'
		<< "Type: " << this->enumToString(this->type) << '\n'
		<< "Tiles: " << this->tileMap->getLayerSize(this->mousePosGrid.x, this->mousePosGrid.y, this->layer) << '\n'
		<< "Text: " << this->charContains << '\n'
		<< "Show text: " << this->showText << '\n'
		<< "Tile lock: " << this->tileAddLock;


	this->cursorText.setString(ss.str());
}

void EditorState::updatePauseMenuButtons()
{
	std::string file_name = "";

	if (this->pmenu->isButtonPressed("QUIT"))
		this->endState();

	if (this->pmenu->isButtonPressed("SAVE") && this->getKeytime())
	{
		std::string file_name = this->textbox1->getText();
		this->tileMap->saveToFile(file_name);
	}

	if (this->pmenu->isButtonPressed("LOAD") && this->getKeytime())
	{
		std::string file_name = this->textbox1->getText();
		this->initTileMap(file_name);
		this->initTileGui();
		this->tileMap->loadFromFile(file_name);
	}
		
}


void EditorState::update(const float& dt)
{
	this->updateMousePositions(&this->view);
	this->updateKeytime(dt);
	this->updateInput(dt);
	this->textbox1->update(this->mousePosWindow, dt);

	if (!this->paused)
	{
		this->updateButtons( dt);

		if (this->tileMap)
		{
			this->updateGui(dt);
			this->upateEditorInput(dt);
		}
	}
	else
	{
		this->pmenu->update(this->mousePosWindow, dt);
		this->updatePauseMenuButtons();
	}

}

void EditorState::renderButtons(sf::RenderTarget& target)
{
	for (auto& it : this->buttons)
	{
		it.second->render(target);
	}

}

void EditorState::renderGui(sf::RenderTarget& target)
{
	if (!this->textureSelector->getActive())
	{
		target.setView(this->view);
		target.draw(this->selectorRect);

	}

	target.setView(this->window->getDefaultView());
	this->textureSelector->render(target);
	target.draw(this->sidebar);
	

	target.setView(this->view);
	target.draw(this->cursorText);
}

void EditorState::render(sf::RenderTarget* target)
{
	if (!target)
	{
		target = this->window;
	}

	target->setView(this->view);
	if (this->tileMap)
	{
		this->tileMap->render(*target, this->mousePosGrid, true);
	}

	target->setView(this->window->getDefaultView());
	this->renderButtons(*target);
	this->textbox1->render(*target);

	if (this->tileMap)
	{
		this->renderGui(*target);
	}

	if (this->paused)
	{
		target->setView(this->window->getDefaultView());
		this->pmenu->render(*target);
		this->textbox1->render(*target);
		
	}
}


