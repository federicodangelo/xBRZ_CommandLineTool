// xBRZ_CommandLineTool.cpp : Defines the entry point for the console application.
//

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_DEFINE

#define _CRT_SECURE_NO_WARNINGS
#include "xBRZ\xbrz.h"

#if _WIN32
#define _X86_
#include <WinDef.h>
#endif

#include <stdio.h>
#include <string.h>

#include "stb\stb.h"
#include "stb\stb_image.h"
#include "stb\stb_image_write.h"

int processFile(char* fromFile, char* toFile, int scaleFactor, int padding);

int main(int argc, char* argv[])
{
	bool use_file = false;
	bool use_directory = false;

	size_t scaleFactor = 2;
	char* sourcePath = NULL;
	char* targetPath = NULL;

	for (int i = 1; i < argc; i++)
	{
		char* option = argv[i];

		if (strcmp(option, "-f") == 0)
			use_file = true;
		else if (strcmp(option, "-d") == 0)
			use_directory = true;
		else if (strcmp(option, "-2") == 0)
			scaleFactor = 2;
		else if (strcmp(option, "-3") == 0)
			scaleFactor = 3;
		else if (strcmp(option, "-4") == 0)
			scaleFactor = 4;
		else if (strcmp(option, "-5") == 0)
			scaleFactor = 5;
		else if (strcmp(option, "-6") == 0)
			scaleFactor = 6;
		else if (sourcePath == NULL)
			sourcePath = option;
		else if (targetPath == NULL)
			targetPath = option;
	}

	if (sourcePath == NULL || targetPath == NULL || (!use_file && !use_directory))
	{
		printf("Usage:\n  Files: xbrz -f from_file.png to_file.png\n  Directories: xbrz -d fromDirectory toDirectory\n");
		printf("Optional argumentes: -2 (scale 2x) -3 (scale 3x) -4 (scale 4x) -5 (scale 5x) -6 (scale 6x)  \n");
		return 1;
	}

	printf("\n");

	if (use_file)
	{
		//Add 2 pixels padding if the file doesn't contain "floor" in the filename
		return processFile(sourcePath, targetPath, scaleFactor, strstr(sourcePath, "floor") == NULL ? 2 : 0);
	}
	else if (use_directory)
	{
		char **files = stb_readdir_files_mask(sourcePath, "*.png");
		char toFile[8192]; //dirty...

		for (int i = 0; i < stb_arr_len(files); ++i) 
		{
			char* fromFile = files[i];
			char* fromPathFileName = strrchr(fromFile, '/');

			if (fromPathFileName != NULL)
			{
				strcpy(toFile, targetPath); 
				if (toFile[strlen(toFile) - 1] != '/' && toFile[strlen(toFile) - 1] != '\\')
					strcat(toFile, "/");
				strcat(toFile, fromPathFileName + 1); //skip "/"

				//Add 2 pixels padding if the file doesn't contain "floor" in the filename
				processFile(fromFile, toFile, scaleFactor, strstr(fromPathFileName, "floor") == NULL ? 2 : 0);
			}

		}

		stb_readdir_free(files);
	}
}

int processFile(char* fromFile, char* toFile, int scaleFactor, int padding)
{
	int originalWidth,originalHeight,n;
	uint32_t *originalPixels = (uint32_t*) stbi_load(fromFile, &originalWidth, &originalHeight, &n, 4);

	if (padding > 0)
	{	
		int newWidth = originalWidth + padding * 2;
		int newHeight = originalHeight + padding * 2;
		uint32_t *newPixels = (uint32_t*) STBI_MALLOC(newWidth * newHeight * sizeof(uint32_t));

		memset(newPixels, 0, newWidth * newHeight * sizeof(uint32_t));

		for (int y = 0; y < originalHeight; y++)
			for (int x = 0; x < originalWidth; x++)
				newPixels[(y + padding) * newWidth + (x + padding)] = originalPixels[y * originalWidth + x];

		stbi_image_free(originalPixels);

		originalPixels = newPixels;
		originalWidth = newWidth;
		originalHeight = newHeight;
	}


	if (originalPixels != NULL)
	{
		uint32_t *resizedPixels = new uint32_t[originalWidth * scaleFactor * originalHeight * scaleFactor];

		xbrz::ColorFormat colorFormat = xbrz::ColorFormat::ARGB;

		xbrz::scale(scaleFactor, originalPixels, resizedPixels, originalWidth, originalHeight, colorFormat);

		originalWidth *= scaleFactor;
		originalHeight *= scaleFactor;
		padding *= scaleFactor;

		if (padding > 0)
		{
			int newWidth = originalWidth - padding * 2;
			int newHeight = originalHeight - padding * 2;
				 
			uint32_t *newPixels = (uint32_t*)STBI_MALLOC(newWidth * newHeight * sizeof(uint32_t));

			memset(newPixels, 0, newWidth * newHeight * sizeof(uint32_t));

			for (int y = 0; y < newHeight; y++)
				for (int x = 0; x < newWidth; x++)
					newPixels[y * newWidth + x] = resizedPixels[(y + padding) * originalWidth + (x + padding)];

			stbi_image_free(resizedPixels);

			resizedPixels = newPixels;
			originalWidth -= padding * 2;
			originalHeight -= padding * 2;
		}


		if (stbi_write_png(toFile, originalWidth, originalHeight, 4, resizedPixels, 0) == 0)
			printf("Error writing target file: %s\n", toFile);
		else
			printf("File %s scaled by %d and saved to file %s\n", fromFile, scaleFactor, toFile);

		delete[] resizedPixels;

		stbi_image_free(originalPixels);
	}
	else
	{
		printf("Source file not found: %s\n", fromFile);
	}

	return 0;
}

