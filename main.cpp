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

int processFile(char* fromFile, char* toFile, int scaleFactor);

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
		return processFile(sourcePath, targetPath, scaleFactor);
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

				processFile(fromFile, toFile, scaleFactor);
			}

		}

		stb_readdir_free(files);
	}
}

int processFile(char* fromFile, char* toFile, int scaleFactor)
{
	int originalWidth,originalHeight,n;
	unsigned char *originalPixels = stbi_load(fromFile, &originalWidth, &originalHeight, &n, 4);

	if (originalPixels != NULL)
	{
		uint32_t *resizedPixels = new uint32_t[originalWidth * scaleFactor * originalHeight * scaleFactor];

		xbrz::ColorFormat colorFormat = xbrz::ColorFormat::ARGB;

		xbrz::scale(scaleFactor, (uint32_t*)originalPixels, resizedPixels, originalWidth, originalHeight, colorFormat);

		if (stbi_write_png(toFile, originalWidth * scaleFactor, originalHeight * scaleFactor, 4, resizedPixels, 0) == 0)
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

