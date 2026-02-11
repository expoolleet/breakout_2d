#pragma once

class Texture2D {
private:
	unsigned int m_ID = 0;
	unsigned int m_internalFormat;
	unsigned int m_imageFormat;
	int m_width = 0;
	int m_height = 0;
public:

	Texture2D();
	void bind() const;
	unsigned int getImageFormat() const;
	unsigned int getWidth() const;
	unsigned int getHeight() const;
	void generate(int width, int height, bool alpha, unsigned char *data);
};