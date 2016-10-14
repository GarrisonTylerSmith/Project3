#include <stdio.h>
#include <stdlib.h>

typedef struct {
	unsigned char red,green,blue;
}PPMPixel;

typedef struct {
	unsigned char red,green,blue;
	int width;
	int height;
	int max;
	int type;
	PPMPixel *data;
}PPMImage;
// typedef struct{
// 	int width;
// 	int height;
// 	int max;
// 	int type;
// }PPMmeta;
#define CREATOR "GarrisonSmith"
#define RGB_COMPONENT_COLOR 255

static PPMImage *readPPM(const char *filename){
	printf(filename);
	char buff[16];
	PPMImage *img;
	FILE *fp;
	int c, rgb_comp_color;

	// this is where we would open the file for reading
	// the PPM file to be exact
	//printf("readppm %s\n", filename);
	fp = fopen(filename, "rb");
	if(!fp){
		fprintf(stderr, "Unable to open file '%s'\n", filename);
		exit(1);
	}
	// now we will read the format of the image
	if(!fgets(buff, sizeof(buff), fp)){
		perror(filename);
		exit(1);
	} 

// check the format of the image
	if(buff[0] != 'P' && (buff[1] != '6' || buff[1] != '3')){
		fprintf(stderr, "Invalid image format (MUST BE P6 or P3)'%c'\n", buff[0]);
		exit(1);
	}
	// alloc memory from image
	img = (PPMImage *)malloc(sizeof(PPMImage));
	if(!img){
		fprintf(stderr, "Unable to allocate memory '%s'\n" );
		exit(1);

	}
	// next we need to check for comments
	c = getc(fp);
	while(c == '#'){
	while(getc(fp) != '\n');
		c = getc(fp);
	}
	ungetc(c,fp);
	// here we will read the image size and information about it
	if(fscanf(fp, "%d %d", &img->width, &img->height) != 2){
		fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
		exit(1);
	}
	// read rgb component
	if (fscanf(fp, "%d", &rgb_comp_color)!= 1){
		fprintf(stderr, "Invalid rgb component(error Loading '%s')\n", filename);
		exit(1);
	}
	// check rgb component depth 
	if(rgb_comp_color != RGB_COMPONENT_COLOR){
		fprintf(stderr, "'%s'does not have 8-bits components\n", filename);
		exit(1);
	}
	while(fgetc(fp) != '\n');
	// memory allocation for pixel data
	img->data = (PPMPixel*)malloc(img->width * img->height * sizeof(PPMPixel));

	if(!img){
		fprintf(stderr, "Unable to allocate memory\n" );
		exit(1);
	}

	//read pixel data from file
	if(fread(img->data, 3, img->width * img->height, fp) != img->width * img->height){
		fprintf(stderr, "Error loading image '%s'\n", filename);
		exit(1);
	}
	fclose(fp);
	return img;
}
void writePPM(const char *filename, PPMImage *image){
	FILE *fp;
	int type = image->type;
	// open file for output
	fp = fopen(filename, "wb");
	if(!fp){
		fprintf(stderr, "Unable to open file '%s'\n", filename);
		exit(1);
	}
	// write the header file
	// image format
	fprintf(fp, "P3\n");
	// comments
	fprintf(fp, "# Created by %s\n", CREATOR);
	// image size
	fprintf(fp, "%d %d\n", image->width,image->height);
	// rgb component depth
	fprintf(fp, "%d\n", RGB_COMPONENT_COLOR);
	//fprintf(fp, "%s", img->data);

	// pixel data
	//if(type == 3){
		//fprintf(fp, "%s", img->data);
		for(int i=0;i<image->width*image->height;i++) {
			fprintf(fp, "%d\n", image->data[i].red);
			fprintf(fp, "%d\n", image->data[i].green);
			fprintf(fp, "%d\n", image->data[i].blue);
		}
	//}
	//else {
//		fwrite(image->data, sizeof(PPMPixel), image->height * image->width, fp);
	//}
	fclose(fp);
}
void changeColorPPM(PPMImage *img){
	if(img){
		for(int i=0;i<img->width*img->height;i++){
			img->data[i].red=RGB_COMPONENT_COLOR-img->data[i].red;
			img->data[i].green=RGB_COMPONENT_COLOR-img->data[i].green;
			img->data[i].blue=RGB_COMPONENT_COLOR-img->data[i].blue;

		}
	}
}
// int main(int argc, char *argv[]){
// 	printf("starting..\n");
//  	char* target = argv[1];
//  	char* input = argv[2];
//  	char* output = argv[3];
//  	printf("target='%s' input='%s' output='%s'\n", target, input, output);
//  	PPMImage *image;
// 	image = readPPM(input);
//  	//changeColorPPM(image);
//  	writePPM(output, image, target);
//  	return 0;
//  }