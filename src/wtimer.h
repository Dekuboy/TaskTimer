#include <SDL.h>
#include <SDL_ttf.h>
#include <ctime>
#include <string>

struct NumValues
{
	const SDL_Rect m_top =		{ 45,  90, 40,  5 };
	const SDL_Rect m_topLeft =	{ 40,  95,  5, 40 };
	const SDL_Rect m_topRight = { 85,  95,  5, 40 };
	const SDL_Rect m_middle =	{ 45, 135, 40,  5 };
	const SDL_Rect m_botLeft =	{ 40, 140,  5, 40 };
	const SDL_Rect m_botRight = { 85, 140,  5, 40 };
	const SDL_Rect m_bot =		{ 45, 180, 40,  5 };

	const SDL_Rect m_colonTop = { 168, 125, 5,  5 };
	const SDL_Rect m_colonBot = { 168, 145, 5,  5 };
};

class WTimer
{
public:
	// Constructor
	WTimer();
	// Destructor
	~WTimer();

	// Run Timer
	void run();

private:
	SDL_Window* m_window;
	SDL_Surface* m_surface;
	SDL_Renderer* m_renderer;

	SDL_Texture* m_textTexture;
	SDL_Texture* m_guideTexture;

	TTF_Font* m_font;
	SDL_Color m_textColor;
	const std::string m_insertText = "Task Name: ";

	SDL_Rect m_bgRect;
	SDL_Rect m_displayRect;
	SDL_Rect m_guideRect;
	SDL_Rect m_button;
	NumValues m_clockNumbers;

	time_t m_start;
	time_t m_finish;

	bool m_quitApp;

	void drawClock(int _seconds, int _minutes = -1, int _hours = -1);
	void updateNum(int _id, int _value);

};