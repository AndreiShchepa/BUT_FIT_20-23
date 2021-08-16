// ppm.c
// Řešení IJC-DU1, příklad b), 20.3.2021
// Autor: Andrei Shchapaniak, FIT
// Přeloženo: gcc 10.2.0
/*
 * In this file I have added function ppm_write() for program steg-encode.
 * This function doesnt affect the work of the main components of the project.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"
#include "ppm.h"

#define ERROR_NAME_FILE "%s: %s: File with name \"%s\" is wrong!\n"
#define ERROR_CONTEXT_FILE "%s: %s: Context of the entered file is wrong!\n"
#define ERROR_ALLOCATE_DATA "%s: %s: Cannot allocate memory for array!\n"
#define ERROR_READ_FILE_TO_ARRAY "%s: %s: Problem with writing file context to array!\n"
#define ERROR_OPEN_FILE_FOR_WRITING "%s: %s: File with name \"%s\" couldnt be opened for writing!\n"
#define ERROR_WRITE_DATA_TO_FILE "%s: %s: Problem with writing data to file \"%s\"!\n"

/*
 * @brief 				Parse image_name.ppm into structure.
 *
 * @param	filename	Path to file
 *
 * @return				If success, pointer on structure, otherwise NULL
 */
struct ppm* ppm_read(const char* filename)
{
	FILE *fp;
	struct ppm* image;
	char error_msg[200] = ERROR_CONTEXT_FILE;
	char str[5] = {0};
	unsigned long x = 0, y = 0;
	unsigned long readed_symbols = 0;
	int state = 0;

	if((fp = fopen(filename, "r")) == NULL)
	{
		warning_msg(ERROR_NAME_FILE, __FILE__, __func__, filename);
 		return NULL;
	}

	while (state != 7)
	{
		switch (state)
		{
			case 0:
			{
				if (fgets(str, 4, fp) == NULL)					{ state = 6; }
				else if(strcmp("P6\n", str) != 0) 				{ state = 6; }
				else 											{ state = 1; }
				break;
			}
			case 1:
			{
				if(fscanf(fp, "%lu %lu\n", &x, &y) != 2)		{ state = 6; }
				else 											{ state = 2; }
				break;
			}
			case 2:
			{
				if(x <= 0 || y <= 0 || x > 8000 || y > 8000)	{ state = 6; }
				else 											{ state = 3; }
				break;
			}
			case 3:
			{
				if (fgets(str, 5, fp) == NULL)					{ state = 6; }
				else if(strcmp("255\n", str) != 0) 				{ state = 6; }
				else 											{ state = 4; }
				break;
			}
			case 4:
			{
				image = malloc(sizeof(struct ppm) + x*y*3);
				if (image == NULL)
				{
					strcpy(error_msg, ERROR_ALLOCATE_DATA);
					state = 6;
				}
				else { state = 5; }
				break;
			}
			case 5:
			{
				image->xsize = x; image->ysize = y;
				x *= y*3;
				readed_symbols = fread(&image->data, sizeof(char), x, fp);
				if (readed_symbols != x)
				{
					ppm_free(image);
					strcpy(error_msg, ERROR_READ_FILE_TO_ARRAY);
					state = 6;
				}
				else { state = 7; }
				break;
			}
			case 6:
			{
				fclose(fp);
				warning_msg(error_msg, __FILE__, __func__);
				return NULL;
			}
		}
	}

	fclose(fp);
	return image;
}

/*
 * @brief 				Write data to name_image.ppm
 *
 * @param	image		Structure with data from image
 * @param	filename	Name of file.ppm
 *
 * @return				If success, 0, otherwise 1
 */
int ppm_write(struct ppm* image, const char* filename)
{
	FILE *fp;
	int state = 0;
	unsigned long x = 0, y = 0;
	unsigned long writed_symbols = 0;

	if ((fp = fopen(filename,"wb")) == NULL)
	{
		warning_msg(ERROR_OPEN_FILE_FOR_WRITING, __FILE__, __func__, filename);
 		return 1;
	}

	while (state != 3)
	{
		switch (state)
		{
			case 0:
			{
				x = image->xsize;
				y = image->ysize;
				if (fprintf(fp, "P6\n%lu %lu\n255\n", x, y) < 0)	{ state = 2; }
				else 												{ state = 1; }
				break;
			}
			case 1:
			{
				x *= y*3;
				writed_symbols = fwrite(image->data, sizeof(char), x, fp);
				if (writed_symbols != x)  							{ state = 2; }
				else 												{ state = 3; }
				break;
			}
			case 2:
			{
				fclose(fp);
				warning_msg(ERROR_WRITE_DATA_TO_FILE, __FILE__, __func__, filename);
				return 1;
			}
		}
	}

	fclose(fp);
	return 0;
}

/*
 * @brief 		Clear allocated structure
 *
 * @param	p	Structure with data from image
 *
 * @return		Nothing
 */
void ppm_free(struct ppm *p)
{
    free(p);
}
