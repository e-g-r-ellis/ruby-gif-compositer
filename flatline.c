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
	// Process
    int errorCode = 0;
    GifFileType* fileType = DGifOpenFileHandle(0, &errorCode);
    if (fileType == 0) {
        printf("File with name %s could not be openned, error code %d\n\tError message: %s\n", argv[1], errorCode, GifErrorString(errorCode));
        return 2;
    }
		
    int returnCode = DGifSlurp(fileType);
    if (returnCode == GIF_ERROR) {
        printf("Error occurred");
        return 3;
    }

    FILE *writeHandle = 1;
    if (writeHandle == NULL) {
        printf("Could not open %s for writing.\n",argv[2]);
        return 4;
    }
    
    GifFileType* gifWriteHandle = EGifOpenFileHandle(writeHandle, &errorCode);
    if (gifWriteHandle == NULL) {
        printf("EGifOpenFileHandle for %s errored.\n", argv[2]);
        //PrintGifError(errorCode);
        return 5;
    }
    
    gifWriteHandle->SWidth = fileType->SWidth;
    gifWriteHandle->SHeight = fileType->SHeight;
    gifWriteHandle->SColorResolution = fileType->SColorResolution;
    gifWriteHandle->SBackGroundColor = fileType->SBackGroundColor;
    gifWriteHandle->SColorMap = GifMakeMapObject(fileType->SColorMap->ColorCount, fileType->SColorMap->Colors);
    
    GifByteType* RasterBits = fileType->SavedImages->RasterBits;
    for (int i = 0; i < fileType->SWidth; i++) {
        *(RasterBits++) = 0;
    }
    
    for (int i = 0; i < fileType->ImageCount; i++) {
        GifMakeSavedImage(gifWriteHandle, &fileType->SavedImages[i]);
    }
    
    if (EGifSpew(gifWriteHandle) == GIF_ERROR) {
        //PrintGifError(gifWriteHandle->Error);
    }
    
    if (DGifCloseFile(fileType, &errorCode) == GIF_ERROR) {
        //PrintGifError(errorCode);
    }
    if (EGifCloseFile(gifWriteHandle, &errorCode) == GIF_ERROR) {
        //PrintGifError(errorCode);
    }
    
	return 0;
}
