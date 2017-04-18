#include <iostream>
#include <fstream>
#include "Image.h"

using namespace std;

bool Image::SetPixelColour(int x, int y, float r, float g, float b)
{
	if (x >= 0 && x < m_w)
	{
		if (y >= 0 && y < m_h && m_imageData)
		{
			int buffPos = (x*m_bpp)+(y * m_w * m_bpp);
			m_imageData[buffPos+0] = (char)(r*255.f);
			m_imageData[buffPos+1] = (char)(g*255.f);
			m_imageData[buffPos+2] = (char)(b*255.f);
		}
	}
	return false;
}

struct BitmapHeader
{
	int32_t size_of_file;
	int32_t reserve;
	int32_t offset_of_pixel_data;
	int32_t size_of_header;
	int32_t width;
	int32_t height;
	int16_t num_of_colour_plane;
	int16_t num_of_bit_per_pix;
	int32_t compression;
	int32_t size_of_pix_data;
	int32_t h_resolution;
	int32_t v_resolution;
	int32_t num_of_colour_in_palette;
	int32_t important_colours;
} ;

bool Image::SaveImage(const char* outputName)
{
	if (m_imageData)
	{
		// assuming its a bmp for now...
		int xPad = (4-(m_w*m_bpp)%4)%4;
		unsigned int padding = 0;
		short BM = 0x4d42;
		int rowLenInBytes =  ((m_w * m_bpp) + xPad);
		BitmapHeader header;
		header.size_of_file =  sizeof(header) + sizeof(BM) + (rowLenInBytes * m_h);
		header.reserve = 0000;
		header.offset_of_pixel_data = 54;
		header.size_of_header = 40;
		header.width = m_w;
		header.height = m_h;
		header.num_of_colour_plane = 1;
		header.num_of_bit_per_pix = 8 * m_bpp;
		header.compression = 0;
		header.size_of_pix_data = (rowLenInBytes * m_h);
		header.h_resolution = 1000;
		header.v_resolution = 1000;
		header.num_of_colour_in_palette = 0;
		header.important_colours = 0;

		ofstream file;
		file.open (outputName, ios::out | ios::trunc | ios::binary);

		if (file.is_open())
		{
			file.write ((char*)(&BM), 2);
			file.write ((char*)(&header), sizeof(header));

			for (int y=m_h-1; y>=0; y--)
			{
				int rowLen = (m_w * m_bpp);
				for (int x=0;x<m_w;x++)
				{
					int buffPos = (x*m_bpp)+(rowLen * y);
					file.write((char*)&m_imageData[buffPos+2], 1);
					file.write((char*)&m_imageData[buffPos+1], 1);
					file.write((char*)&m_imageData[buffPos+0], 1);
				}
				if (xPad)
				{
					file.write((char*)(&padding), xPad);
				}
			}

			file.close();

			return true;
		}
		else
		{
			cout << "Couldn't open " << outputName << " for writing." << endl;
			return false;
		}
	}

	cout << "No image data to write!" << endl;
	return false;
}
