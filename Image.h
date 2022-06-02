#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <stdlib.h>

class Image
{
public:
	Image(int w, int h, int bpp)
	{
		m_imageData = (unsigned char*)malloc(bpp * w * h);
		m_w = w;
		m_h = h;
		m_bpp = bpp;
	}
	virtual ~Image()
	{
		free(m_imageData);
	}

	bool		SetPixelColour		(int x, int y, float r, float g, float b);
	bool		SaveImage			(const char* outputName);

private:

	void		WriteBitmap			();

	unsigned char*		m_imageData;
	int					m_w;
	int					m_h;
	int					m_bpp;
};

#endif