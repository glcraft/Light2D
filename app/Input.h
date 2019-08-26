#pragma once
#include <memory>
#include <string>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <map>
//#include <onidev/signal.h>
#include <array>
class Input
{
public:
	
	struct WindowData
	{
		WindowData(SDL_Window* win = nullptr) : windowPtr(win)
		{}
		struct IO
		{
			template <typename vecType>
			struct Movement
			{
				vecType position, relative;
			};
			struct State
			{
				/*
				bool pressed();
				bool released();
				bool pressing();
				char state;*/
				bool pressed;
				bool released;
				bool pressing;
			};
			struct Finger
			{
				State state;
				Movement<glm::vec2> move;
				float pressure;
			};
			struct Mouse
			{
				glm::ivec2 wheel;
				Movement<glm::ivec2> move;
				std::array<State, 6> buttons;
			}mouse;
			/*struct Keys
			{
				State& operator[](SDL_Scancode keycode);
				const State& operator[](SDL_Scancode keycode) const;
				State& any(SDL_Scancode keycode);
				const State& any(SDL_Scancode keycode) const;

				size_t size();
				bool empty();
				std::vector<State>::iterator begin();
				std::vector<State>::iterator end();
			protected:
				std::vector<State> m_keys;
				State m_any;
				static State Null;
			} keys;*/
			std::array<State, SDL_NUM_SCANCODES> keys;
			struct Fingers
			{
				bool pressed;
				bool released;
				bool moved;
				std::map<SDL_FingerID, Finger> props;
			}fingers;
			 
		}io;

		void reset(bool starter = false);
		void swapBuffers() const;
		
		//void pollEvent(SDL_Event evt);
		//trigger resized
		bool resized;
		//trigger closed
		bool closed;
		glm::ivec2 position;
		glm::ivec2 size;
		SDL_Window* windowPtr;
		SDL_GLContext glContext;
	};

	Input();
	Input(const std::string& title, Uint32 flags, glm::vec2 pos = glm::vec2(SDL_WINDOWPOS_CENTERED), glm::ivec2 size = glm::ivec2(-1, -1));
	~Input();

	static void update();
	//void reset();

	void createWindow(const std::string& title, Uint32 flags = SDL_WINDOW_OPENGL, glm::vec2 pos = glm::vec2(SDL_WINDOWPOS_CENTERED), glm::ivec2 size = glm::ivec2(-1, -1));
	void destroyWindow();

	bool getKey(int key) const;
	bool getKeyPressed(int key) const;
	bool getKeyReleased(int key) const;
	WindowData::IO::State getKeyState(int key) const;

	bool getMouseButton(int mb) const;
	bool getMouseButtonPressed(int mb) const;
	bool getMouseButtonReleased(int mb) const;
	WindowData::IO::State getMouseButtonState(int mb) const;
	glm::vec2 getMousePosition() const;
	//Get Relative position
	glm::vec2 getMouseMotion() const;

	const Input::WindowData & getWindowData() const { return m_windows[m_windowID]; }
	void closeWindow();
	void makeCurrent() const;
	static const Input::WindowData & getWindowData(Uint32 windowID);
	void setRelativeMode(bool enabled);

	bool isRelativeMode();

	
	
	/*void addAction(Data&, od::Signal<Data>&&);
	const Data& getAction(std::string)const;*/
private:
	Input::WindowData& impl_getWindowData(){ return m_windows[m_windowID]; }
	static std::map<Uint32,WindowData> m_windows;
	Uint32 m_windowID;
	
	/*std::map<Data*, od::Signal<Data>> m_actions;*/
};
