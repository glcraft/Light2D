#include "Input.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <GL/glew.h>
std::map<Uint32, Input::WindowData> Input::m_windows;

Input::Input() : m_windowID(0)
{
}

Input::Input(const std::string& title, Uint32 flags, glm::vec2 pos, glm::ivec2 size) : Input()
{
	createWindow(title, flags, pos, size);
}

Input::~Input()
{
}

void Input::createWindow(const std::string& title, Uint32 flags, glm::vec2 pos, glm::ivec2 size)
{
	// Version d'OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Double Buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	// SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	// SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	if (size.x < 0)
		size.x = DM.w * 3 / 4;
	if (size.y < 0)
		size.y = DM.h * 3 / 4;
	SDL_Window* wPtr=SDL_CreateWindow(title.c_str(), pos.x, pos.y, size.x, size.y, flags);
	if (wPtr == 0)
        throw std::runtime_error("SDL : Impossible de créer une fenêtre: "+std::string(SDL_GetError()));

	SDL_GLContext glcont = SDL_GL_CreateContext(wPtr);
	if (glcont == 0)
	{
        std::ostringstream ostr;
        ostr << "SDL : Impossible de créer le contexte OpenGL de la fenêtre: " << std::endl;
        ostr << "\t" << SDL_GetError()<< std::endl;
		ostr << "Notez qu'il faut être compatible OpenGL 3.3 minimum." << std::endl;
        auto test=glGetString(GL_VERSION);
        //ostr << "OpenGL Version: " << << std::endl;
        std::cout << ostr.str();
        
		throw std::runtime_error("SDL : Impossible de créer le contexte OpenGL de la fenêtre: ");
	}

	Uint32 wID = SDL_GetWindowID(wPtr);
	m_windowID = wID;
	auto mypair = m_windows.insert(std::make_pair<Uint32,WindowData>(std::move(wID), WindowData(wPtr)));
	mypair.first->second.glContext = std::move(glcont);
	mypair.first->second.size = size;
	mypair.first->second.position = pos;
	mypair.first->second.reset(true);
}
void Input::destroyWindow()
{
	SDL_GL_DeleteContext(m_windows[m_windowID].glContext);
	SDL_DestroyWindow(m_windows[m_windowID].windowPtr);
	m_windows.erase(m_windowID);
}
void Input::update()
{
	SDL_Event evt;
	for (auto& win : m_windows)
		win.second.reset();
	if (SDL_PollEvent(&evt))
	{
		do
		{
			switch (evt.type)
			{
			case SDL_FINGERDOWN:
			case SDL_FINGERMOTION:
			case SDL_FINGERUP:
			{
				bool nostate = evt.type == SDL_FINGERMOTION, pressed = evt.type == SDL_FINGERDOWN, released = evt.type == SDL_FINGERUP;
				std::for_each(m_windows.begin(), m_windows.end(), [nostate, pressed, released, evt](std::pair<const Uint32, WindowData>& windata) {
					WindowData::IO::Finger& fing = windata.second.io.fingers.props[evt.tfinger.fingerId];
					if (!nostate)
					{
						fing.state.pressed = pressed;
						fing.state.pressing = pressed;
						fing.state.released = released;
					}
					fing.pressure = evt.tfinger.pressure;
					fing.move.position = glm::vec2(evt.tfinger.x, evt.tfinger.y);
					fing.move.relative = glm::vec2(evt.tfinger.dx, evt.tfinger.dy);
					windata.second.io.fingers.pressed |= pressed;
					windata.second.io.fingers.released |= released;
					windata.second.io.fingers.moved |= nostate;
				});
				break;
			}
			case SDL_KEYDOWN:
				m_windows[evt.key.windowID].io.keys[evt.key.keysym.scancode].pressed = true;
				m_windows[evt.key.windowID].io.keys[evt.key.keysym.scancode].pressing = true;
				break;
			case SDL_KEYUP:
				m_windows[evt.key.windowID].io.keys[evt.key.keysym.scancode].released = true;
				m_windows[evt.key.windowID].io.keys[evt.key.keysym.scancode].pressing = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				m_windows[evt.button.windowID].io.mouse.buttons[evt.button.button].pressed = true;
				m_windows[evt.button.windowID].io.mouse.buttons[evt.button.button].pressing = true;
				break;
			case SDL_MOUSEBUTTONUP:
				m_windows[evt.button.windowID].io.mouse.buttons[evt.button.button].released = true;
				m_windows[evt.button.windowID].io.mouse.buttons[evt.button.button].pressing = false;
				break;
			case SDL_MOUSEMOTION:
				m_windows[evt.motion.windowID].io.mouse.move.relative.x += evt.motion.xrel;
				m_windows[evt.motion.windowID].io.mouse.move.relative.y += evt.motion.yrel;
				m_windows[evt.motion.windowID].io.mouse.move.position.x = evt.motion.x;
				m_windows[evt.motion.windowID].io.mouse.move.position.y = evt.motion.y;
				break;
			case SDL_MOUSEWHEEL:
				m_windows[evt.wheel.windowID].io.mouse.wheel.x = evt.wheel.x;
				m_windows[evt.wheel.windowID].io.mouse.wheel.y = evt.wheel.y;
				break;
			case SDL_WINDOWEVENT:
				switch (evt.window.event)
				{
				case SDL_WINDOWEVENT_CLOSE:
					m_windows[evt.window.windowID].closed = true;
					break;
				case SDL_WINDOWEVENT_RESIZED:
					m_windows[evt.window.windowID].resized = true;
					m_windows[evt.window.windowID].size = glm::ivec2(evt.window.data1, evt.window.data2);
					break;
				}
			}
		} while (SDL_PollEvent(&evt));
	}
}

void Input::WindowData::reset(bool starter)
{
	for (int i = 0; i < SDL_NUM_SCANCODES; i++)
	{
		io.keys[i].pressed = false;
		io.keys[i].released= false;
	}
	
	for (int i = 0; i < 6; i++)
	{
		io.mouse.buttons[i].pressed = false;
		io.mouse.buttons[i].released = false;
	}
	for (auto itFinger = io.fingers.props.begin(); itFinger != io.fingers.props.end();)
	{
		if (itFinger->second.state.released)
		{
			itFinger = io.fingers.props.erase(itFinger);
		}
		else
		{
			itFinger->second.state.pressed = false;
			itFinger->second.move.relative = glm::vec2(0.f);
			itFinger++;
		}
	}
	for (auto finger : io.fingers.props)
	{
		finger.second.state.pressed = false;
		finger.second.state.released = false;
		finger.second.move.relative = glm::vec2(0.f);
	}
	if (starter)
	{
		for (int i = 0; i < SDL_NUM_SCANCODES; i++)
			io.keys[i].pressing = false;
		for (int i = 0; i < 6; i++)
			io.mouse.buttons[i].pressing = false;

	}
	io.fingers.pressed = false;
	io.fingers.released= false;
	io.fingers.moved= false;
	io.mouse.wheel = glm::ivec2(0.f);
	io.mouse.move.relative = glm::vec2(0.f);
	resized = false;
	closed= false;
}

void Input::WindowData::swapBuffers() const 
{
	SDL_GL_SwapWindow(windowPtr);
}

bool Input::getKey(int key) const
{
	return getWindowData().io.keys[key].pressing;
}

bool Input::getKeyPressed(int key) const
{
	return getWindowData().io.keys[key].pressed;
}

bool Input::getKeyReleased(int key) const
{
	return getWindowData().io.keys[key].released;
}

Input::WindowData::IO::State Input::getKeyState(int key) const
{
	return getWindowData().io.keys[key];
}

bool Input::getMouseButton(int mb) const
{
	return getWindowData().io.mouse.buttons[mb].pressing;
}

bool Input::getMouseButtonPressed(int mb) const
{
	return getWindowData().io.mouse.buttons[mb].pressed;
}

bool Input::getMouseButtonReleased(int mb) const
{
	return getWindowData().io.mouse.buttons[mb].released;
}

Input::WindowData::IO::State Input::getMouseButtonState(int mb) const
{
	return getWindowData().io.mouse.buttons[mb];
}
glm::vec2 Input::getMousePosition() const
{
	return getWindowData().io.mouse.move.position;
}
glm::vec2 Input::getMouseMotion() const
{
	return getWindowData().io.mouse.move.relative;
}
void Input::closeWindow()
{
	impl_getWindowData().closed = true;
}
void Input::makeCurrent() const
{
	SDL_GL_MakeCurrent(getWindowData().windowPtr, getWindowData().glContext);
}

const Input::WindowData & Input::getWindowData(Uint32 windowID)
{
	return m_windows[windowID];
}

void Input::setRelativeMode(bool enabled)
{
	SDL_SetRelativeMouseMode(static_cast<SDL_bool>(enabled));
}
bool Input::isRelativeMode()
{
	return SDL_GetRelativeMouseMode();
}

//void addAction(std::string, Data&, od::Signal<Data>&&);
//{
//	m_actions.
//}
//
//const Input::Data & Input::getAction(std::string name) const
//{
//	// TODO: insérer une instruction return ici
//}
