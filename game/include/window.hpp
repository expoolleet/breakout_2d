#pragma once

class Window {
private:
	static int m_width;
	static int m_height;

	// Removing default constructor
	Window() = delete;
	// Removing copy constructor
	Window(const Window&) = delete;
	// Removing the assignment operator
	Window& operator=(const Window&) = delete;

public:
	inline static int getWidth() { return m_width; }
	inline static int getHeight() { return m_height; }
	inline static void setWidth(int width) { m_width = width; }
	inline static void setHeight(int height) { m_height = height; }
};