#include <wtimer.h>
#include <stdexcept>
#include <fstream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 240;

WTimer::WTimer()
{
	// Initialize SDL Window and TTf
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		throw std::runtime_error("Failed to initialize SDL2");
	}
	if (TTF_Init() < 0)
	{
		throw std::runtime_error("Failed to initialize TrueType Font");
	}


	// Initialize Window
	m_window = SDL_CreateWindow("TaskTimer",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (m_window == NULL)
	{
		throw std::runtime_error("Failed to create SDL2 window");
	}

	// Initialize Renderer
	m_renderer = SDL_CreateRenderer(m_window, -1, 0);

	// Initialize Arial font for display
	m_font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 25);
	if (!m_font)
	{
		throw std::runtime_error("C:/Windows/Fonts/arial.ttf not found");
	}

	// Set text color as black
	m_textColor = { 0, 0, 0, 0xFF };

	// Set Initial text to guide Texture
	m_surface = TTF_RenderText_Solid(m_font,
		"Press Enter to Confirm (20 character limit)", m_textColor);
	m_guideTexture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
	SDL_FreeSurface(m_surface);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(m_guideTexture, NULL, NULL, &texW, &texH);
	m_guideRect = { 20, 40, texW / 2, texH / 2 };
	
	m_bgRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	m_quitApp = false;
}

WTimer::~WTimer()
{
	// Close Application when complete
	TTF_CloseFont(m_font);
	SDL_DestroyTexture(m_textTexture);
	SDL_DestroyTexture(m_guideTexture);
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	TTF_Quit();
	SDL_Quit();
}

void WTimer::run()
{
	// Manage quitting the application and loops   
	bool quit = false;

	// Event handler
	SDL_Event e;

	// The current input text.
	std::string inputText = "";

	// Texture values
	int texW = 0, texH = 0;

	// File to save to
	std::ofstream tableFile;

	// Write to file
	std::string toWrite;

	// Time difference
	long int dif;
	int ticksLast, ticks, ticksDif, ticksCount = 0;
	int hours = 0, minutes = 0, seconds = 0, hourLast = 0, minLast = 0;

	// Mouse Values
	int posX, posY;

	// Run until User quits
	while (!m_quitApp)
	{
		// Initialize variables for loop
		quit = false;
		inputText = "";
		m_textColor = { 0, 0, 0, 0xFF };
		SDL_SetRenderDrawColor(m_renderer, 0xF5, 0xF5, 0xF5, 0xFF);

		// Set initial text to Texture
		m_surface = TTF_RenderText_Solid(m_font,
			m_insertText.c_str(), m_textColor);
		m_textTexture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
		SDL_FreeSurface(m_surface);
		SDL_QueryTexture(m_textTexture, NULL, NULL, &texW, &texH);
		m_displayRect = { 20, 10, texW, texH };

		// Render to screen
		SDL_RenderFillRect(m_renderer, &m_bgRect);
		SDL_RenderCopy(m_renderer, m_textTexture, NULL, &m_displayRect);
		SDL_RenderCopy(m_renderer, m_guideTexture, NULL, &m_guideRect);

		SDL_RenderPresent(m_renderer);

		//Enable text input
		SDL_StartTextInput();

		//While waiting for text
		while (!quit)
		{
			//The rerender text flag
			bool renderText = false;

			//Handle events on queue
			while (SDL_PollEvent(&e))
			{
				//Check if user quits app
				if (e.type == SDL_QUIT)
				{
					m_quitApp = true;
					return;
				}
				//Special key input
				else if (e.type == SDL_KEYDOWN)
				{
					// Handle Quit
					if (e.key.keysym.sym == SDLK_ESCAPE)
					{
						m_quitApp = true;
						return;
					}
					//Handle backspace
					else if (e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)
					{
						//lop off character
						inputText.pop_back();
						renderText = true;
					}
					//Handle copy
					else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
					{
						SDL_SetClipboardText(inputText.c_str());
					}
					//Handle paste
					else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
					{
						inputText = SDL_GetClipboardText();
						renderText = true;
					}
					else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_RETURN2)
					{
						if (inputText.length() == 0)
						{
							// Alert user to name their current task
							SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
								"Alert",
								"Please give a name for your current task.",
								NULL);
						}
						else
						{
							quit = true;
						}
					}
				}
				//Special text input event
				else if (e.type == SDL_TEXTINPUT && inputText.length() < 20)
				{
					//Not copy or pasting
					if (!(SDL_GetModState() & KMOD_CTRL && (e.text.text[0] == 'c' || e.text.text[0] == 'C' || e.text.text[0] == 'v' || e.text.text[0] == 'V')))
					{
						//Append character
						inputText += e.text.text;
						renderText = true;
					}
				}
				if (renderText)
				{
					// Update Text
					m_surface = TTF_RenderText_Solid(m_font,
						(m_insertText + inputText).c_str(), m_textColor);
					m_textTexture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
					SDL_FreeSurface(m_surface);
					SDL_QueryTexture(m_textTexture, NULL, NULL, &texW, &texH);
					m_displayRect = { 20, 10, texW, texH };
					
					// Render to screen
					SDL_RenderFillRect(m_renderer, &m_bgRect);
					SDL_RenderCopy(m_renderer, m_textTexture, NULL, &m_displayRect);
					SDL_RenderCopy(m_renderer, m_guideTexture, NULL, &m_guideRect);

					SDL_RenderPresent(m_renderer);
				}
			}
		}

		// Rerender to screen with updated values
		m_textColor = { 0xFF, 0xFF, 0xFF, 0xFF };

		// Update Text
		m_surface = TTF_RenderText_Solid(m_font,
			(m_insertText + inputText).c_str(), m_textColor);
		m_textTexture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
		SDL_FreeSurface(m_surface);
		m_displayRect = { 20, 10, texW, texH };

		// Render to screen
		SDL_RenderFillRect(m_renderer, &m_bgRect);
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0xFF);
		SDL_RenderFillRect(m_renderer, &m_displayRect);
		SDL_RenderCopy(m_renderer, m_textTexture, NULL, &m_displayRect);

		//  ---------  Display Clock  ---------  

		// Draw Background
		m_displayRect = { 20, 50, 600, 175 };
		SDL_RenderFillRect(m_renderer, &m_displayRect);

		// Draw Start Button
		m_textColor = { 0, 0, 0, 0xFF };
		SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

		m_surface = TTF_RenderText_Solid(m_font,
			"Start", m_textColor);
		m_textTexture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
		SDL_FreeSurface(m_surface);
		SDL_QueryTexture(m_textTexture, NULL, NULL, &texW, &texH);
		m_displayRect = { 520, 125, 53, 28 };

		SDL_RenderFillRect(m_renderer, &m_displayRect);
		SDL_RenderCopy(m_renderer, m_textTexture, NULL, &m_displayRect);

		// Draw Colons
		{
			SDL_SetRenderDrawColor(m_renderer, 0xFF, 0x0F, 0x0F, 0x0F);
			m_displayRect = m_clockNumbers.m_colonTop;
			SDL_RenderFillRect(m_renderer, &m_displayRect);
			m_displayRect.x += 150;
			SDL_RenderFillRect(m_renderer, &m_displayRect);
			m_displayRect = m_clockNumbers.m_colonBot;
			SDL_RenderFillRect(m_renderer, &m_displayRect);
			m_displayRect.x += 150;
			SDL_RenderFillRect(m_renderer, &m_displayRect);
		}

		// Draw Numbers
		drawClock(0,0,0);

		// Prepare Clock Start
		quit = false;
		while (!quit)
		{
			while (SDL_PollEvent(&e))
			{
				if (e.type == SDL_QUIT)
				{
					m_quitApp = true;
					return;
				}
				else if (e.type == SDL_KEYDOWN)
				{
					// Handle Quit
					if (e.key.keysym.sym == SDLK_ESCAPE)
					{
						m_quitApp = true;
						return;
					}
					// Handle Start
					else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_RETURN2)
					{
						quit = true;
					}
				}
				else if (e.type == SDL_MOUSEBUTTONUP)
				{
					if (e.button.button == SDL_BUTTON_LEFT)
					{
						SDL_GetMouseState(&posX, &posY);

						// Collide
						if (posX >= 520 && posX <= 573 && posY >= 125 && posY <= 153)
						{
							quit = true;
						}
					}
				}
			}
		}

		// Start Clock
		time(&m_start);
		toWrite = ctime(&m_start);
		toWrite.pop_back();

		// Draw Stop/Save Button
		m_textColor = { 0, 0, 0, 0xFF };
		SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

		m_surface = TTF_RenderText_Solid(m_font,
			"Stop", m_textColor);
		m_textTexture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
		SDL_FreeSurface(m_surface);
		m_displayRect = { 520, 125, 53, 28 };

		SDL_RenderFillRect(m_renderer, &m_displayRect);
		SDL_RenderCopy(m_renderer, m_textTexture, NULL, &m_displayRect);

		SDL_RenderPresent(m_renderer);

		// Manage Clock
		quit = false;
		ticksLast = SDL_GetTicks();
		while (!quit)
		{
			while (SDL_PollEvent(&e))
			{
				if (e.type == SDL_QUIT)
				{
					m_quitApp = true;
					return;
				}
				else if (e.type == SDL_KEYDOWN)
				{
					// Handle Quit
					if (e.key.keysym.sym == SDLK_ESCAPE)
					{
						m_quitApp = true;
						return;
					}
					// Handle Stop
					else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_RETURN2)
					{
						quit = true;
					}
				}
				else if (e.type == SDL_MOUSEBUTTONUP)
				{
					if (e.button.button == SDL_BUTTON_LEFT)
					{
						SDL_GetMouseState(&posX, &posY);

						// Collide
						if (posX >= 520 && posX <= 573 && posY >= 125 && posY <= 153)
						{
							quit = true;
						}
					}
				}
			}

			ticks = SDL_GetTicks();
			ticksDif = ticks - ticksLast;
			if (ticksDif < 33)
			{
				SDL_Delay(33 - ticksDif);
			}
			ticks = SDL_GetTicks();
			ticksDif = ticks - ticksLast;
			ticksCount += ticksDif;
			ticksLast = ticks;
			if (ticksCount >= 999)
			{
				ticksCount -= 1000;
				time(&m_finish);
				dif = difftime(m_finish, m_start);
				hours = dif / 3600, minutes = (dif % 3600) / 60, seconds = dif % 60;
				if (hours != hourLast)
				{
					drawClock(seconds, minutes, hours);
				}
				else if (minutes != minLast)
				{
					drawClock(seconds, minutes);
				}
				else
				{
					drawClock(seconds);
				}
				hourLast = hours;
				minLast = minutes;
			}
		}

		time(&m_finish);
		toWrite += ", ";
		toWrite += ctime(&m_finish);
		toWrite.pop_back();
		
		dif = difftime(m_finish, m_start);
		hours = dif / 3600, minutes = (dif % 3600) / 60, seconds = dif % 60;

		quit = false;
		while (!quit)
		{
			while (SDL_PollEvent(&e))
			{
				if (e.type == SDL_QUIT)
				{
					m_quitApp = true;
					return;
				}
			}
			tableFile.open("TASK_TIMES.csv", std::ios::app);
			if (tableFile.is_open())
			{
				quit = true;
			}
			else
			{
				// Alert user to name their current task
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
					"Error: TASK_TIMES will not open",
					"Please make sure TASK_TIMES is available to open and not being used by another program.",
					NULL);
			}
		}
		tableFile << inputText << ", "<< toWrite << ", " << hours << " hours, " << minutes << " minutes, " << seconds << " seconds\n";
		tableFile.close();
	}
}

void WTimer::drawClock(int _seconds, int _minutes, int _hours)
{
	// Update Seconds
	updateNum(0, _seconds % 10);
	updateNum(1, _seconds / 10);

	// Update Minutes
	if (_minutes >= 0)
	{
		updateNum(2, _minutes % 10);
		updateNum(3, _minutes / 10);

		// Update Hours
		if (_hours >= 0)
		{
			updateNum(4, _hours % 10);
			updateNum(5, _hours / 10);
		}
	}

	// Draw to screen
	SDL_RenderPresent(m_renderer);
}

void WTimer::updateNum(int _id, int _value)
{
	// Find Number Location
	int difX = 0;
	if (_id < 2 && _id >= 0)
	{
		difX = 300;
		if (_id == 0)
		{
			difX += 60;
		}
	}
	else if (_id < 4 && _id >= 2)
	{
		difX = 150;
		if (_id == 2)
		{
			difX += 60;
		}
	}
	else if (_id < 6 && _id >= 4)
	{
		if (_id == 4)
		{
			difX += 60;
		}
	}
	else
	{
		return;
	}

	{
		// Draw Number Top
		{
			if (_value != 1 && _value != 4)
			{
				SDL_SetRenderDrawColor(m_renderer, 0xFF, 0x0F, 0x0F, 0x0F);
			}
			else
			{
				SDL_SetRenderDrawColor(m_renderer, 0x0F, 0x0F, 0x0F, 0x0F);
			}
			m_displayRect = m_clockNumbers.m_top;
			m_displayRect.x += difX;
			SDL_RenderFillRect(m_renderer, &m_displayRect);
		}
		// Draw Number TopLeft
		{
			if (_value != 1 && _value != 2 && _value != 3 && _value != 7)
			{
				SDL_SetRenderDrawColor(m_renderer, 0xFF, 0x0F, 0x0F, 0x0F);
			}
			else
			{
				SDL_SetRenderDrawColor(m_renderer, 0x0F, 0x0F, 0x0F, 0x0F);
			}
			m_displayRect = m_clockNumbers.m_topLeft;
			m_displayRect.x += difX;
			SDL_RenderFillRect(m_renderer, &m_displayRect);
		}
		// Draw Number TopRight
		{
			if (_value != 5 && _value != 6)
			{
				SDL_SetRenderDrawColor(m_renderer, 0xFF, 0x0F, 0x0F, 0x0F);
			}
			else
			{
				SDL_SetRenderDrawColor(m_renderer, 0x0F, 0x0F, 0x0F, 0x0F);
			}
			m_displayRect = m_clockNumbers.m_topRight;
			m_displayRect.x += difX;
			SDL_RenderFillRect(m_renderer, &m_displayRect);
		}
		// Draw Number Middle
		{
			if (_value != 0 && _value != 1 && _value != 7)
			{
				SDL_SetRenderDrawColor(m_renderer, 0xFF, 0x0F, 0x0F, 0x0F);
			}
			else
			{
				SDL_SetRenderDrawColor(m_renderer, 0x0F, 0x0F, 0x0F, 0x0F);
			}
			m_displayRect = m_clockNumbers.m_middle;
			m_displayRect.x += difX;
			SDL_RenderFillRect(m_renderer, &m_displayRect);
		}
		// Draw Number BotLeft
		{
			if (_value == 2 || _value == 6 || _value == 8 || _value == 0)
			{
				SDL_SetRenderDrawColor(m_renderer, 0xFF, 0x0F, 0x0F, 0x0F);
			}
			else
			{
				SDL_SetRenderDrawColor(m_renderer, 0x0F, 0x0F, 0x0F, 0x0F);
			}
			m_displayRect = m_clockNumbers.m_botLeft;
			m_displayRect.x += difX;
			SDL_RenderFillRect(m_renderer, &m_displayRect);
		}
		// Draw Number BotRight
		{
			if (_value != 2)
			{
				SDL_SetRenderDrawColor(m_renderer, 0xFF, 0x0F, 0x0F, 0x0F);
			}
			else
			{
				SDL_SetRenderDrawColor(m_renderer, 0x0F, 0x0F, 0x0F, 0x0F);
			}
			m_displayRect = m_clockNumbers.m_botRight;
			m_displayRect.x += difX;
			SDL_RenderFillRect(m_renderer, &m_displayRect);
		}
		// Draw Number Bot
		{
			if (_value != 1 && _value != 4 && _value != 7)
			{
				SDL_SetRenderDrawColor(m_renderer, 0xFF, 0x0F, 0x0F, 0x0F);
			}
			else
			{
				SDL_SetRenderDrawColor(m_renderer, 0x0F, 0x0F, 0x0F, 0x0F);
			}
			m_displayRect = m_clockNumbers.m_bot;
			m_displayRect.x += difX;
			SDL_RenderFillRect(m_renderer, &m_displayRect);
		}
	}
}
