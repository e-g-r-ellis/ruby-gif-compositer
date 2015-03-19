#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "./giflib-5.1.1/lib/gif_lib.h"

void copyGifToWriteHandle(GifFileType* image, GifFileType* output) {
    output->SWidth = image->SWidth;
    output->SHeight = image->SHeight;
    output->SColorResolution = image->SColorResolution;
    output->SBackGroundColor = image->SBackGroundColor;
    output->SColorMap = GifMakeMapObject(image->SColorMap->ColorCount, image->SColorMap->Colors);
    
    for (int i = 0; i < image->ImageCount; i++) {
        fprintf(stderr, "Copied %d images.\n", image->ImageCount);
        GifMakeSavedImage(output, &image->SavedImages[i]);
    }
    
    /*
    GifByteType* RasterBits = image->SavedImages->RasterBits;
    for (int i = 0; i < image->SWidth; i++) {
        *(RasterBits++) = 0;
    }
    */
}

void printStats(GifFileType *fileType) {
    fprintf(stderr, "\tWidth: %d Height: %d ImageCount: %d SavedImages: %d\n", fileType->SWidth, fileType->SHeight, fileType->ImageCount, fileType->SavedImages);
}

int openFileHandleOrExit(char* fname, int mode, int returnCode) {
    int result;
    fprintf(stderr,"Openning file %s\n", fname);
    if ((result = open(fname, mode)) == -1) {
        fprintf(stderr, "Error occurred while opening file handle %s.\n", fname);
    }
    // Check for errors openning file handle.
    return result;
}

GifFileType *readFileOrExit(char* fname, int *fileHandle, int *errorCode) {
    GifFileType *result;
    fprintf(stderr,"Openning file %s\n", fname);
    if ((*fileHandle = open(fname, O_RDONLY)) == -1) {
        fprintf(stderr, "Error occurred while opening file handle %s.\n", fname);
    }
    
    fprintf(stderr,"Openning read handle %s\n", fname);
    if ((result = DGifOpenFileHandle(*fileHandle, &errorCode)) == NULL) {
        fprintf(stderr, "Could not read gif in file %s.\n", fname);
        exit(-3);
    }
    
    fprintf(stderr,"Reading %s\n", fname);
    if (DGifSlurp(result) == GIF_ERROR) {
        fprintf(stderr,"Slurp while reading %s\n", fname);
        exit(-4);
    }
    printStats(result);
    return result;
}

GifByteType *skipToStart(int x, int y, GifFileType *file) {
    int toSkip = file->SWidth * y + x;
    return (GifByteType *)file->SavedImages->RasterBits + toSkip;
}

void composite(GifFileType *image, int x, int y, GifFileType *background) {
    if (x < 0 || y < 0 || x + image->SWidth > background->SWidth || image->SHeight > background->SHeight) {
        fprintf(stderr, "Composite image is out of range!\n\tImage width: %d height: %d\n\tx: %d y: %d\n\tBackground width: %d height: %d", image->SWidth, image->SHeight, x, y, background->SWidth, background->SHeight);
    }
    
    GifByteType *imageByte, *backgroundByte;
    imageByte = image->SavedImages->RasterBits;
    backgroundByte = skipToStart(x, y, background);
    for (int i = 0; i < image->SHeight; i++) {
        // Copy row
        for (int j = 0; j < image->SWidth; j++) {
            *(backgroundByte++) = *(imageByte++);
        }
        
        backgroundByte += background->SWidth - image->SWidth;
        //imageByte++;
    }
}

int main(int argc, char* argv[]) {
    // Open read files
    char* backgroundFilename = argv[1], *imageFilename = argv[2], *resultFilename = argv[3];
    fprintf(stderr,"Background file: %s\nImage file: %s\nResult file: %s\n", argv[1], argv[2], argv[3]);
    
    int fbackground, fimage, fresult; // File handle, not file descriptor
    //fresult = openFileHandleOrExit(resultFilename, O_WRONLY, -3);
    fresult = 1;
    
    // Read gif files to memory
    int errorCode = 0;
    GifFileType *gbackground, *gimage;
    gbackground = readFileOrExit(backgroundFilename, &fbackground, &errorCode);
    gimage = readFileOrExit(imageFilename, &fimage, &errorCode);
    
    // Open gif handle for writing
    fprintf(stderr, "Preparing write file handle %s\n", resultFilename);
    GifFileType* gifWriteHandle = EGifOpenFileHandle(fresult, &errorCode);
    if (gifWriteHandle == NULL) {
        fprintf(stderr, "EGifOpenFileHandle for stdout errored.\n");
        return 5;
    }
    fprintf(stderr, "Copying background to write handle.\n");
    copyGifToWriteHandle(gbackground, gifWriteHandle);
    printStats(gifWriteHandle);
    
    // DO STUFF HERE
    int x = 41;
    int y = 205;
    composite(gimage, x, y, gifWriteHandle);
    
    // Write output gif to file
    fprintf(stderr, "Writing output contents to disk.\n");
    if (EGifSpew(gifWriteHandle) == GIF_ERROR) {
        fprintf(stderr, "Spew errored: %s\n", GifErrorString(errorCode));
    }
    
    // Close and deallocate memory
    fprintf(stderr, "Closing.\n");
    if (DGifCloseFile(gbackground, &errorCode) == GIF_ERROR) {
        fprintf(stderr, "Error occurred while closing background file.\n\t%s\n", GifErrorString(errorCode));
    }
    /*
    if (DGifCloseFile(gimage, &errorCode) == GIF_ERROR) {
        fprintf(stderr, "Error occurred while closing image file.\n\t%s\n", GifErrorString(errorCode));
    }
    */
    if (EGifCloseFile(gifWriteHandle, &errorCode) == GIF_ERROR) {
        fprintf(stderr, "Error occurred while closing output file.\n\t%s\n", GifErrorString(errorCode));
    }
    
    return 0;
}
