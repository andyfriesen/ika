#include "importpng.h"
#include "png.h"
#include "fileio.h"

// ----------------- PNG ---------------------
// I think this was originally in the Sphere source, so, give Aegis credit for this.  I just hacked it up a bit. --tSB

// prototypes
static void __cdecl PNG_read_function(png_structp png_ptr, png_bytep data, png_size_t length);
static void __cdecl PNG_write_function(png_structp png_ptr, png_bytep data, png_size_t length);
static void __cdecl PNG_flush_function(png_structp png_ptr);

/**********************
*  bool Import_PNG()  *
***********************
* Group:   Graphics
* Job:     loads a PNG image into a png_image structure
* Returns: pointer to the image on success, NULL otherwise
**********************/
png_image* Import_PNG(const char* filename)
{
	png_image* pis=0;
	png_structp png_ptr=0;
	png_infop info_ptr=0;
	File file;

	try
	{
		pis=new png_image;
		
		if (!file.OpenRead(filename))
			throw;

		u8 sig[8];
		file.Read(sig,8);
		if (png_sig_cmp(sig,0,8))	throw;

		png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
		if (!png_ptr)				throw;

		info_ptr=png_create_info_struct(png_ptr);
		if (!info_ptr)				throw;
	}	// Exceptions are so cool. ;)
	catch (...)
	{
		if (pis)				delete pis;
		if (info_ptr)			png_destroy_info_struct(png_ptr,&info_ptr);
		if (png_ptr)			png_destroy_read_struct(&png_ptr,NULL,NULL);
		file.Close();
		return NULL;
	}
	
	png_set_read_fn(png_ptr, &file, PNG_read_function);
	
	png_set_sig_bytes(png_ptr, 8);
	int png_transform = PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_PACKSWAP;
	png_read_png(png_ptr, info_ptr, NULL, NULL);
	
	// initialize the png_image members
	pis->width  = png_get_image_width(png_ptr, info_ptr);
	pis->height = png_get_image_height(png_ptr, info_ptr);
	pis->pixels = new RGBA[pis->width * pis->height];
	
	if (png_get_rows(png_ptr, info_ptr) == NULL)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		file.Close();
		delete pis;
		return NULL;
	}
	
	// decode based on pixel depth
	int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	int num_channels = png_get_channels(png_ptr, info_ptr);
	void** row_pointers = (void**)png_get_rows(png_ptr, info_ptr);
	
	if (bit_depth == 8 && num_channels == 4)
	{
		for (int i = 0; i < pis->height; i++)
		{
			RGBA* row = (RGBA*)(row_pointers[i]);
			for (int j = 0; j < pis->width; j++)
				pis->pixels[i * pis->width + j] = row[j];
		}
	}
	else if (bit_depth == 8 && num_channels == 3)
	{
		for (int i = 0; i < pis->height; i++)
		{
			RGB* row = (RGB*)(row_pointers[i]);
			for (int j = 0; j < pis->width; j++)
			{
				RGBA p(row[j].r, row[j].g, row[j].b, 255);
				pis->pixels[i * pis->width + j] = p;
			}
		}
	}
	else if (bit_depth == 8 && num_channels == 2)
	{
		png_colorp palette;
		int num_palette = 0;
		png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
		
		// if there is no palette, use black and white
		if (num_palette == 0)
		{
			for (int i = 0; i < pis->height; i++)
			{
				u8* row = (u8*)(row_pointers[i]);
				for (int j = 0; j < pis->width; j++)
				{
					pis->pixels[i * pis->width + j].r   = row[j * 2];
					pis->pixels[i * pis->width + j].g = row[j * 2];
					pis->pixels[i * pis->width + j].b  = row[j * 2];
					pis->pixels[i * pis->width + j].a = row[j * 2 + 1];
				}
			}
		}
		else // otherwise use the palette
		{
			for (int i = 0; i < pis->height; i++)
			{
				u8* row = (u8*)(row_pointers[i]);
				for (int j = 0; j < pis->width; j++)
				{
					pis->pixels[i * pis->width + j].r = palette[row[j * 2]].red;
					pis->pixels[i * pis->width + j].g = palette[row[j * 2]].green;
					pis->pixels[i * pis->width + j].b = palette[row[j * 2]].blue;
					pis->pixels[i * pis->width + j].a = row[j * 2 + 1];
				}
			}
		}
	}
	else if (bit_depth == 8 && num_channels == 1)
	{
		png_colorp palette;
		int num_palette = 0;
		png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
		
		png_bytep trans;
		int num_trans = 0;
		png_color_16p trans_values;
		png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, &trans_values);
		
		// if there is no palette, use black and white
		if (num_palette == 0)
		{
			for (int i = 0; i < pis->height; i++)
			{
				u8* row = (u8*)(row_pointers[i]);
				for (int j = 0; j < pis->width; j++)
				{
					u8 alpha = 255;
					for (int k = 0; k < num_trans; k++) {
						if (trans[k] == row[j]) {
							alpha = 0;
						}
					}
					pis->pixels[i * pis->width + j].r = row[j];
					pis->pixels[i * pis->width + j].g = row[j];
					pis->pixels[i * pis->width + j].b = row[j];
					pis->pixels[i * pis->width + j].a = alpha;
				}
			}
		}
		else // otherwise use the palette
		{
			for (int i = 0; i < pis->height; i++)
			{
				u8* row = (u8*)(row_pointers[i]);
				for (int j = 0; j < pis->width; j++)
				{
					u8 alpha = 255;
					for (int k = 0; k < num_trans; k++) {
						if (trans[k] == row[j]) {
							alpha = 0;
						}
					}
					pis->pixels[i * pis->width + j].r = palette[row[j]].red;
					pis->pixels[i * pis->width + j].g = palette[row[j]].green;
					pis->pixels[i * pis->width + j].b = palette[row[j]].blue;
					pis->pixels[i * pis->width + j].a = alpha;
				}
			}
		}
	}
	else if (bit_depth == 4 && num_channels == 1)
	{
		png_colorp palette;
		int num_palette;
		png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
		
		for (int i = 0; i < pis->height; i++)
		{
			RGBA* dst = pis->pixels + i * pis->width;
			
			u8* row = (u8*)(row_pointers[i]);
			for (int j = 0; j < pis->width / 2; j++)
			{
				u8 p1 = *row >> 4;
				u8 p2 = *row & 0xF;
				
				dst->r = palette[p1].red;
				dst->g = palette[p1].green;
				dst->b = palette[p1].blue;
				dst->a = 255;
				dst++;
				
				dst->r = palette[p2].red;
				dst->g = palette[p2].green;
				dst->b = palette[p2].blue;
				dst->a = 255;
				dst++;
				
				row++;
			}
			
			if (pis->width % 2)
			{
				u8 p = *row >> 4;
				dst->r = palette[p].red;
				dst->g = palette[p].green;
				dst->b = palette[p].blue;
				dst->a = 255;
				dst++;
			}
		}
	}
	else if (bit_depth == 1 && num_channels == 1)
	{
		png_colorp palette;
		int num_palette;
		png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
		
		for (int i = 0; i < pis->height; i++)
		{
			RGBA* dst = pis->pixels + i * pis->width;
			
			int mask = 1;
			u8* p = (u8*)(row_pointers[i]);
			
			for (int j = 0; j < pis->width; j++)
			{
				dst->r = palette[(*p & mask) > 0].red;
				dst->g = palette[(*p & mask) > 0].green;
				dst->b = palette[(*p & mask) > 0].blue;
				dst->a = 255;
				dst++;
				
				mask <<= 1;
				if (mask == 256)
				{
					p++;
					mask = 1;
				}
			}
			
		}
	}
	else
	{
		delete[] pis->pixels;
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		file.Close();
		delete pis;
		return NULL;
	}
	
	// we're done
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	file.Close();
	return pis;
}

bool Export_PNG(png_image* src,const char* filename)
{
	//  IFile* file = fs.Open(filename, IFileSystem::write);
	File file;

	if (!file.OpenWrite(filename))
		return false;
	
	// create png struct
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		file.Close();
		return false;
	}
	
	// create info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_write_struct(&png_ptr, NULL);
		file.Close();
		return false;
	}
	
	// read the image
	png_set_write_fn(png_ptr, &file, PNG_write_function, PNG_flush_function);
	
	png_set_IHDR(png_ptr, info_ptr, src->width, src->height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	
	// put the image data into the PNG
	void** rows = (void**)png_malloc(png_ptr, sizeof(void*) * src->height);
	for (int i = 0; i < src->height; i++)
	{
		rows[i] = png_malloc(png_ptr, sizeof(RGBA) * src->width);
		memcpy(rows[i], src->pixels + i * src->width, src->width * sizeof(RGBA));
	}
	png_set_rows(png_ptr, info_ptr, (png_bytepp)rows);
	info_ptr->valid |= PNG_INFO_IDAT;
	
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
	
	png_destroy_write_struct(&png_ptr, &info_ptr);
	file.Close();
	return true;
}

/******************************************
*  static void CDECL PNG_read_function()  *
*******************************************
* Group:   Graphics
* Job:     Reads a PNG file
* Returns: nothing.
******************************************/
static void __cdecl PNG_read_function(png_structp png_ptr, png_bytep data, png_size_t length)
{
	File* file = (File*)png_get_io_ptr(png_ptr);
	file->Read(data,length);
}

static void __cdecl PNG_write_function(png_structp png_ptr, png_bytep data, png_size_t length)
{
  File* file = (File*)png_get_io_ptr(png_ptr);
  file->Write(data, length);
}

static void __cdecl PNG_flush_function(png_structp png_ptr)
{
  // assume that Files automatically flush
}
