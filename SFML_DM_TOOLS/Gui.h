#ifndef GUI_H
#define GUI_H

enum button_states{BTN_IDLE = 0, BTN_HOVER, BTN_ACTIVE};

namespace gui
{
	const float p2pX(const float perc, const sf::VideoMode& vm);
	const float p2pY(const float perc, const sf::VideoMode& vm);
	const unsigned calcCharSize(const sf::VideoMode& vm, const unsigned modifier = 60);

	class Button
	{
	private:
		short unsigned buttonState;
		short unsigned id;

		sf::RectangleShape shape;
		sf::Font* font;
		sf::Text text;

		sf::Color textIdleColor;
		sf::Color textHoverColor;
		sf::Color textActiveColor;

		sf::Color idleColor;
		sf::Color hoverColor;
		sf::Color activeColor;

		sf::Color outlineIdleColor;
		sf::Color outlineHoverColor;
		sf::Color outlineActiveColor;

	public:
		Button(float x, float y, float width, float height,
			sf::Font* font, std::string text, unsigned character_size,
			sf::Color text_idle_color, sf::Color text_hover_color, sf::Color text_active_color,
			sf::Color idle_color, sf::Color hover_color, sf::Color active_color,
			sf::Color outline_idle_color = sf::Color::Transparent, sf::Color outline_hover_color = sf::Color::Transparent, sf::Color outline_active_color = sf::Color::Transparent,
			short unsigned id = 0);
		~Button();

		//Accessors
		const bool isPressed() const;
		const std::string getText() const;
		const short unsigned& getId() const;

		//Modifiers
		void setText(const std::string text);
		void setId(const short unsigned id);

		//Functions
		void update(const sf::Vector2i& mousePosWindow);
		void render(sf::RenderTarget& target);
	};

	class DropDownList
	{
	private:
		float keytime;
		float keytimeMax;

		sf::Font& font;
		gui::Button* activeElement;
		std::vector<gui::Button*> list;
		bool showList;

	public:
		DropDownList(float x, float y, float width, float height, 
			sf::Font& font, std::string list[], 
			unsigned nrOfElements, unsigned default_index = 0);
		~DropDownList();

		//Accessors
		const unsigned short& getActiveElementId() const;

		//Functions
		const bool getKeytime();
		void updateKeytime(const float& dt);
		void update(const sf::Vector2i& mousePosWindow, const float& dt);
		void render(sf::RenderTarget& target);
	};

	class TextureSelector
	{
	private:
		float keytime;
		const float keytimeMax;
		float gridSize;
		bool active;
		bool hidden;
		gui::Button* hide_btn;
		sf::RectangleShape bounds;
		sf::Sprite sheet;
		sf::RectangleShape selector;
		sf::Vector2u mousePosGrid;
		sf::IntRect textureRect;

	public:
		TextureSelector(float x, float y, float width, float height, 
			float gridSize, const sf::Texture* texture_sheet, 
			sf::Font& font, std::string text);
		~TextureSelector();

		//Accessors
		const bool& getActive() const;
		const sf::IntRect& getTextureRect() const;

		//Functions
		const bool getKeytime();
		void updateKeytime(const float& dt);
		void update(const sf::Vector2i& mousePosWindow, const float& dt);
		void render(sf::RenderTarget& target);
	};



	class TextBox
	{
	private:

		sf::Text textbox;
		std::ostringstream text;
		bool isSelected = false;
		bool hasLimit = false;
		int limit;

		gui::Button* save_btn;

		float keytime;
		const float keytimeMax;

		void deleteLastChar() {
			std::string t = text.str();
			std::string newT = "";

			for (int i = 0; i < t.length() - 1; i++)
			{
				newT += t[i];
			}
			text.str("");
			text << newT;

			textbox.setString(text.str());
		}

		void InputLogic(int charTyped)
		{

			if (charTyped != 8 && charTyped != 46 && charTyped != 27)
			{
				text << static_cast<char> (charTyped);
			}
			else if (charTyped == 8)
			{
				if (text.str().length() > 0)
				{
					deleteLastChar();
				}
			}
			if (charTyped == 13)
			{
				text << "\n";
			}


			textbox.setString(text.str() + "_");
		}

	public:
		

		TextBox(int size, sf::Color color, bool is_selected, 
			float x, float y, sf::Font& font) 
		: keytimeMax(1.f), keytime(0.f) 
		{
			this->textbox.setCharacterSize(size);
			this->textbox.setFillColor(color);
			this->isSelected = is_selected;
			this->hasLimit = false;
			this->limit = 100;

			if (is_selected)
			{
				textbox.setString("_");
			} 
			else
			{
				textbox.setString("");
			}

			this->save_btn = new gui::Button(
				y, x, 100.f, 50.f,
				&font, "Save", 24,
				sf::Color(255, 255, 255, 200), sf::Color(255, 255, 255, 250), sf::Color(255, 255, 255, 50),
				sf::Color(70, 70, 70, 200), sf::Color(150, 150, 150, 250), sf::Color(20, 20, 20, 50)
			);
		};
		virtual ~TextBox() {};

		//Accessors

		std::string getText()
		{
			return text.str();
		}

		const bool getKeytime()
		{
			if (this->keytime >= this->keytimeMax)
			{
				this->keytime = 0.f;
				return true;
			}

			return false;
		}
		void updateKeytime(const float& dt)
		{
			if (this->keytime < this->keytimeMax)
				this->keytime += 10.f * dt;
		}

		//Modifiers

		void setFont(sf::Font& font)
		{
			this->textbox.setFont(font);
		}

		void setPosition(sf::Vector2f position)
		{	
			this->textbox.setPosition(position);
		}

		void setLimit(bool ToF)
		{
			this->hasLimit = ToF;
		}

		void setLimit(bool ToF, int limit)
		{
			this->hasLimit = ToF;
			this->limit = limit;
		}

		void setSelected(bool is_selected)
		{
			this->isSelected = is_selected;
			if (!is_selected)
			{
				std::string t = text.str();
				std::string newT = "";

				for (int i = 0; i < t.length() - 1; ++i)
				{
					newT += t[i];
				}
				this->textbox.setString(newT);
			}
		}

		//Functions

		void typedOn(sf::Event input) {
			if (isSelected)
			{
				int charTyped = input.text.unicode;
				if (charTyped < 128)
				{
					if (hasLimit)
					{
						if (text.str().length() <= this->limit)
							this->InputLogic(charTyped);
						else if (text.str().length() > this->limit && charTyped == 8)
							this->deleteLastChar();
						else if (charTyped == 13)
							text << "\n";
					}
					else
					{
							this->InputLogic(charTyped);
					}
				}
			}
		
		}

		void update(const sf::Vector2i& mousePosWindow)
		{
			this->save_btn->update(mousePosWindow);

			if (this->save_btn->isPressed() && this->getKeytime())
			{
				
			}
		}

		void render(sf::RenderTarget& target)
		{
			target.draw(textbox);
			this->save_btn->render(target);
		}
	};
};

#endif