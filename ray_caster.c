// polymorphism in c
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Imagebuffer.c"
#include "Parsingjson.c"
#include "3dmath.h"

static inline float sqr(float v){
	return v*v;
}
static inline void normalize(float* v){
	float len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
}
float dot(float v[], float u[], int n){
	float result = 0;
	for(int i=0; i < n; i++){
		result += v[i]*u[i];
	}
	return result;
}

float cylinder_intersecion(float* r0, float* Rd, float* c, float r){
	// Step 1 . Find the equation for the object 
	// we are interested in (this case a cylinder)
	// this is essential for the camera view point
	// x^2 +z^2 = r^2
	// Step 2. Parameterize the equation with a center point (if needeed)
	// (x-Cx)^2 + (z-Cz)^2 = r^2
	// step 3. Substitute the eq for a ray into our object eq.
	//
	//(r0X + RDX*t - Cx)^2 + ( r0z + t*Rdz - Cz)^2 - r^2 = 0
	//
	// Step 4. Solve for t .
	//
	//Step 4.a Rewrite the equation(flatten).
	//
	// -r^2 + t^2 * Rdx^2 + t^2 * Rdz^2 + 2*t * r0x * Rdx - 2*t * Rdx * Cx + 2*t * r0z * Rdz - 2*t * Rdz *cz + r0x^2 - 2*r0x*cx + cx^2 + r0z^2 - 2* r0z* Cz + Cz^2 = 0
	//
	// Step 4.b Rewrite the equation in terms of t.
	//
	// t^2 * (Rdx^2 + Rdz^2) + 
	// t* (2 * (r0x * Rdx - Rdx * cx + r0z * Rdz - Rdz * Cz)) + 
	// r0x^2 - 2*r0x*cx + cx^2 + r0z^2 - 2* r0z* Cz + Cz^2 - r^2 = 0
	// Use quadratic equation to solvew for t..
	//
	float a = (sqr(Rd[0]) + sqr(Rd[2]));
	float b = (2 * (r0[0] * Rd[0] - Rd[0] * c[0] + r0[2] * Rd[2] - Rd[2] * c[2]));
	float d = (sqr(r0[0]) - 2*r0[0]*c[0] + sqr(c[0]) + sqr(r0[2]) - 2* r0[2]*c[2] + sqr(c[2]) - sqr(r));
	float det = sqr(b) - 4*a*d; 
	if( det < 0 ) return -1;

	det = sqrt(det);

	float t0 = (-b - det) / (2*a);
	if (t0 > 0 ) return t0;

	float t1 = (-b + det) / (2*a);
	if(t1 > 0) return t1;

	return -1;

}
float ray_plane_intersection(float a, float b, float c, float d, float* r0, float* rd){
	return (a*r0[0] + b*r0[1] + c*r0[2] + d) / (a*rd[0] + b*rd[1] + c*rd[2]);
	//Ray: R(t) = r0 + Rd*t
	//Plane: ax + by + cz + d = 0
	//solve for t
	//n is the normal plane
	// D is the distance fr0m the origin
	// do subsituting we get 
	// t = -(n*r0 + d)/ (n * Rd) 

	// float a = -(n*r0 + d)
	// float b = (n*Rd)
	// float det = (a/b)
	// if(det < 0) return -1;
}
void printv(char* str, float* v)
{
	printf("%s <%f %f %f>\n", str, v[0], v[1], v[2]);
}
float ray_sphere_intersection(float* c, float r, float* r0, float* Rd){
	// Ray: P = r0 + Rd*t
	// Sphere: (x-xc)^2 + (y-yc)^2 + (z-zc)^2 - r^2 = 0
	// Subsituting R(t) into sphere equation
	// (Xo + Xd*t - Xc)^2 + (Yo + Yd*t - Yc)^2 + (Zo + Zd*t - Zc)^2 - r^2 = 0
	// rearranging we get
	// At^2 + Bt + C = 0 where we get two solutions
	// this is a vector fr0m p to c
	float A = (sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]));
	float B = 2*(Rd[0]*(r0[0]-c[0]) + Rd[1]*(r0[1]-c[1]) + Rd[2]*(r0[2]-c[2]));
	float C = sqr(r0[0]-c[0])+sqr(r0[1]-c[1])+sqr(r0[2]-c[2]) - sqr(r);
	float det = sqr(B) - 4*A*C; 
	//printv("c", c);
	//printv("r0", r0);
	//printv("rd", Rd);
	//printf("\n");
	if( det < 0 ) return -1;

	det = sqrt(det);

	float t0 = (-B - det) / (2*A);
	if (t0 > 0 ) return t0;

	float t1 = (-B + det) / (2*A);
	if(t1 > 0) return t1;

	return -1;



}
// float* send_ray(float* color, Scene scene, float* r0, float rd){
// 	int k; 
// 	Object closest;



// 			for(k = 0; k < scene.num_objects; k ++){
// 				float t = -1;
		
// 				Object o = scene.objects[k];
				
// 				if(o.kind == T_SPHERE){
// 					float c[3];
// 					c[0] = o.a;
// 					//printf("%lf\n", c[0]);
// 					c[1] = o.b;
// 					//printf("%lf\n", c[1]);
// 					c[2] = o.c;
// 					//printf("%s\n", c[2]);
// 					t = ray_sphere_intersection(c, o.d, r0, rd);
// 				}else if(o.kind == T_PLANE){
// 					t = ray_plane_intersection(o.a,o.b,o.c,o.d,r0,rd);
// 				}
				
// 				if(t > 0 && t < best_t){
// 					best_t = t;
// 					closest = o;
// 					//printf("setting object to %d %d\n", o.kind, closest.kind);
// 				}
// 			}
// 			double* color = malloc(sizeof(double)*3);
// 			double* lights;
// 			double Ro_new;
// 			double Rd_new;

// 			// for(int l = 0; lights[l] != NULL; l++){

// 			// }
// 			// Iterate through the lights
// 				for(int index = 0; index < num_objects; index++) {
// 					if(strcmp(objects[index].type, "light") == 0) {
// 						vector_scale(rd, best_t, Ro_new);
// 						vector_add(Ro_new, r0, Ro_new);
// 						vector_subtract(objects[index].properties.light.position, Ro_new, Rd_new);
						
// 						for(k = 0; k < num_objects; k++){
// 							// Repeat intersection code test
// 							// If best_t > distance to light, continue...
// 							if(object[k] == closest_shadow_obj){
// 								continue;
// 							}
							
// 						}
						
// 						if(closest_shadow_obj == null) {
// 						}
// 		return -1;
// }	
void raycast(Scene scene, char* outfile, PPMImage* image){
	//PPMImage *image = malloc(sizeof(PPMImage));
	image->data = malloc(sizeof(PPMPixel) * image->width * image->height);
	
	// raycasting here
	
	int N = image->width;
	printf("N=%d\n", N);
	int M = image->height;
	printf("M=%d\n", M);
	float w = scene.camera_width;
	float h = scene.camera_height;
	
	float pixel_height = h / M;
	float pixel_width = w / N;
	
	float p_z = 0;
	
	float c_x = 0;
	float c_y = 0;
	float c_z = 0;
	
	float r0[3];
	r0[0] = c_x;
	r0[1] = c_y;
	r0[2] = c_z;
	
	float rd[3];
	rd[2] = 1.0;
	
	int i;
	int j;
	int k;
	
	for(i = 0; i < M; i ++){
		rd[1] = c_y + h/2.0 - pixel_height * (i + 0.5);
		
		for(j = 0; j < N; j ++){
			rd[0] = c_x - w/2.0 + pixel_width * (j + 0.5);
			
			float best_t = INFINITY;
			Object closest;

			for(k = 0; k < scene.num_objects; k ++){
				float t = -1;
				Object o = scene.objects[k];
				
				if(o.kind == T_SPHERE){
					float c[3];
					c[0] = o.a;
					//printf("%lf\n", c[0]);
					c[1] = o.b;
					//printf("%lf\n", c[1]);
					c[2] = o.c;
					//printf("%s\n", c[2]);
					t = ray_sphere_intersection(c, o.d, r0, rd);
				}else if(o.kind == T_PLANE){
					t = ray_plane_intersection(o.a,o.b,o.c,o.d,r0,rd);
				}
				
				if(t > 0 && t < best_t){
					best_t = t;
					closest = o;
					//printf("setting object to %d %d\n", o.kind, closest.kind);
				}
			}
			
			// write to Pixel* data
			
//			PPMImage image = data[i * N +j];
			PPMPixel pixel;
			//if(closest.kind != 0)
			//printf("kind: %d\n", closest.kind);
			if(best_t < INFINITY && closest.kind > 0){
				// makes the colors for the obejcts 
				pixel.red = (char) (closest.color[0] * 255);
				pixel.green = (char) (closest.color[1] * 255);
				pixel.blue = (char) (closest.color[2] * 255);
				//printf("Found intersection %lf %lf %lf\n", closest.color[0], closest.color[1], closest.color[2]);
			}else{
				// creates shadow effect
				pixel.red = (char) (255* scene.background_color[0]);
				pixel.green = (char) (255* scene.background_color[1]);
				pixel.blue = (char) (255* scene.background_color[2]);
			}

			image->data[i * N + j].red = pixel.red;
			image->data[i * N + j].blue = pixel.blue;
			image->data[i * N + j].green = pixel.green;
			//printf("pixel %d\n", i*N+j);
		}
		
	}
	
	writePPM(outfile, image);
}

// create my own raycasting fucntion
int main(int argc, char** argv){
	if(argc < 5){
		fprintf(stderr, "Usage: width, height input.json output.ppm\n");
		exit(1);
	}
	PPMImage fileinfo;
	fileinfo.width = atoi(argv[1]);
	fileinfo.height = atoi(argv[2]);
	fileinfo.max = 255;
	fileinfo.type = 6;

	Scene scene = read_scene(argv[3]); 


	printf("Read in %d objects\n", scene.num_objects);

	scene.background_color[0] = 0.5;
	scene.background_color[1] = 0.51;
	scene.background_color[2] = 0.6;

	raycast(scene, argv[4], &fileinfo);

	return 0;
}

