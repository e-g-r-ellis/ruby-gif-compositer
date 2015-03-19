#include <stdio.h>
#include <stdlib.h>

#include "./giflib-5.1.1/lib/gif_lib.h"

ColorMapObject* getColourMap(GifFileType* fileType) {
    ColorMapObject* result;
    if (fileType->Image.ColorMap == 0) {
        result = fileType->SColorMap;
    } else {
        result = fileType->Image.ColorMap;
    }
    return result;
}

void displayColours(GifFileType* fileType, ColorMapObject* colorMapPointer, GifByteType* GifByte, int length) {
    printf("DisplayColours(%d)", GifByte);
    for (int i  = 0; i < length; i++) {
        GifColorType colorType = colorMapPointer->Colors[*GifByte];
        printf("[%d]%d RGB(%d, %d, %d)\n", i, *GifByte, colorType.Red, colorType.Green, colorType.Blue);
    }
}

void displayGifFileType(GifFileType* fileType) {
	printf("Image count: %d\n", fileType->ImageCount);
	printf("Extension block count: %d\n", fileType->ExtensionBlockCount);
    printf("Extension blocks: %d\n", fileType->ExtensionBlocks);
	printf("Error: %d\n", fileType->Error);
    printf("Virtual palette SWidth: %d\tSHeight: %d\n", fileType->SWidth, fileType->SHeight);
    printf("GifImageBorder: Left: %d Top: %d Width: %d Heght: %d\n",    fileType->Image.Left,
                                                                        fileType->Image.Top,
                                                                        fileType->Image.Width,
                                                                        fileType->Image.Height);
    printf("Image ColourMap present ? %d\n", fileType->Image.ColorMap);
    printf("ColourMap present? %d\n", fileType->SColorMap);
    ColorMapObject* colorMap = getColourMap(fileType);
    printf("getColourMap ? %d\n", getColourMap(fileType));
    printf("ColourCount: %d BitsPerPixel: %d\n", colorMap->ColorCount,
                                                 colorMap->BitsPerPixel);
    GifColorType* colour = colorMap->Colors;
    for (int i = 0; i < colorMap->ColorCount; i++) {
        printf("\tColour(%d): R(%d) G(%d) B(%d)\n", i, colour->Red, colour->Green, colour->Blue);
        colour++;
    }
}

int main(int argc, char* argv[]) {
	// Validate program arguments
	if (argc < 2) {
		printf("Too few arguments (%d), please supply a gif filename.\n", argc);
		return 1;
	}

	// Process
	for (int i = 1; i < argc; i++) {
		int errorCode = 0;
		GifFileType* fileType = DGifOpenFileName(argv[i], &errorCode);
		if (fileType == 0) {
			printf("File with name %s could not be openned, error code %d\n\tError message: %s\n", argv[i], errorCode, GifErrorString(errorCode));
			return 2;
		}
		
        int returnCode = DGifSlurp(fileType);
        if (returnCode == GIF_ERROR) {
            printf("Error occurred");
            return 3;
        }
        //displayGifFileType(fileType);
        
        /*
        // Gif file has been read
        int minLength = fileType->SWidth < fileType->SHeight ? fileType->SWidth : fileType->SHeight;
        GifByteType* GifByte = fileType->SavedImages->RasterBits;
        ColorMapObject* colorMapPointer = getColourMap(fileType);
        
        displayColours(fileType, colorMapPointer, GifByte, minLength);
        for (int i  = 0; i < minLength; i++) {
            *(GifByte++) = 15;
        }
        
        GifByte = fileType->SavedImages->RasterBits;
        printf("Overwrote first %d RasterBits\n", minLength);
        displayColours(fileType, colorMapPointer, GifByte, minLength);
        */
        
        returnCode = EGifSpew(fileType);
        if (returnCode == GIF_ERROR) {
            printf("Error occurred while writing gif file (%d):\n\t%d\n", returnCode, fileType->Error);
            return 4;
        }
	}
	return 0;
}
