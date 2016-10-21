// polymorphism in c
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Imagebuffer.c"
#include "Parsingjson.c"
#include "3dmath.h"

#define SPEC_SHINE 50
#define SPEC_K 1

static inline void normalize(float* v){
	float len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
}

float dot(float v[], float u[]){
	return v[0]*u[0] + v[1]*u[1] + v[2]*u[2];

}

void vector_multiply(float v[], float u[], float n[]){
	n[0] = v[0]*u[0];
	n[1] = v[1]*u[1];
	n[2] = v[2]*u[2];
}


double clamp(double number, double min, double max){
	if(number > max) {
		return max;
		
	} else if (number < min){
		return min;
		
	} else {
		return number;
		
	}
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
// I created the send ray function to send a ray and get the pixel needed for the intersection
void send_ray(Intersection* intersection, Scene* scene, float* r0, float* rd){
	int k; 
	float best_t = INFINITY;
	float Ro_new[3];
	float Rd_new[3];

			for(k = 0; k < scene->num_objects; k ++){
				float t = -1;
		
				Object o = scene->objects[k];
				
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
					intersection->object_id =k;
					//printf("setting object to %d %d\n", o.kind, closest.kind);
				}
			}


			vector_scale(rd,best_t, Ro_new);
			vector_add(Ro_new,r0,Ro_new);


			intersection->vect_point[0] = Ro_new[0];
			intersection->vect_point[1] = Ro_new[1];
			intersection->vect_point[2] = Ro_new[2];

 }
// I created a get color function which gets our color for the pixel
void get_color(float* color, Scene* scene , float* r0, float* rd){
	Intersection intersect;
	intersect.object_id = -1;
	send_ray(&intersect, scene, r0, rd);
	if(intersect.object_id == -1){
		return;
	}
Object* closest = &scene->objects[intersect.object_id];	
	//float lighting[3];
	int k;

	for(k = 0; k < scene->num_lights; k++){

		Object light;
		light = scene->lights[k];
		//float direction_to_light[3];

		float normal[3];

		// distance to light
		float dist[3];
		vector_subtract(light.position, intersect.vect_point, dist);
		float distance_to_light = length(dist);

		// light direction
		float light_direction[3];
		vector_subtract(intersect.vect_point, light.position, light_direction);
		vector_scale(light_direction, -1, light_direction);
		normalize(light_direction);

		 //shadow test intersection

		// Intersection shadow; 
		// send_ray(&shadow, scene, intersect.vect_point, light_direction);

		// if(shadow.object_id != -1){

		// 	vector_subtract(shadow.vect_point, intersect.vect_point, dist);
		// 	float* distance_to_object = length(dist);

		// 	if(light_direction > distance_to_object){
		// 		continue;
		// 	}


		// }
		// here we get the normal for the sphere and the normal of the objects
		if(closest->kind == T_SPHERE){
					float c[3];
					c[0] = closest->a;
					//printf("%lf\n", c[0]);
					c[1] = closest->b;
					//printf("%lf\n", c[1]);
					c[2] = closest->c;
					//printf("%s\n", c[2]);
			vector_subtract(intersect.vect_point, c, normal);
			normalize(normal);
		}else if(closest->kind == T_PLANE){
			normal[0] = closest->a;
			normal[1] = closest->b;
			normal[2] = closest->c;
			normalize(normal);
		}

		float incident_light_level = dot(normal, light_direction);
		if(incident_light_level > 0){
			float Incident[3];
			Incident[0] = 0;
			Incident[1] = 0;
			Incident[2] = 0;
				// below I commented out the code but it is a step towards radial attenuation
				// float rad_attenuation = 0;

				// pow(dot(light_direction, light.direction), light.d)

				// rad_attenuation = 1 / (light.a * sqr(distance_to_light) + light.b * distance_to_light + light.c);

				// rad_attenuation = clamp(rad_attenuation, 0.0, 1.0);

				// here is the r vector out of the light or object
				float r[3];
				vector_scale(normal, dot(light_direction, normal) * 2, r);
				vector_subtract(light_direction, r, r);
				// heere is the v vector that comes out of the light as well
				float v[3];
				vector_scale(rd, -1, v);

				// This is where we get the specular color for the light

				float spec[3];

				float spec_1 = powf(dot(r,v), SPEC_SHINE) * SPEC_K;
				vector_scale(light.color, spec_1, spec);
				vector_multiply(closest->specular_color, spec, spec);

				vector_add(color, spec, color);

				// this is where we get diffuse color for our light
				float diff[3];

				vector_scale(closest->color, incident_light_level, diff);

				vector_add(color, diff, color);




		}
	}

}	
void raycast(Scene* scene, char* outfile, PPMImage* image){
	image->data = malloc(sizeof(PPMPixel) * image->width * image->height);
	
	// raycasting here
	
	int N = image->width;
	printf("N=%d\n", N);
	int M = image->height;
	printf("M=%d\n", M);
	float w = scene->camera_width;
	float h = scene->camera_height;
	
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
			
			
			// write to Pixel* data
			
//			PPMImage image = data[i * N +j];
			float color[3] = {0, 0, 0};

			get_color(color, scene, r0, rd);
		

			image->data[i * N + j].red = color[0] * 255;
			image->data[i * N + j].green = color[1] * 255;
			image->data[i * N + j].blue = color[2] * 255;
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

	raycast(&scene, argv[4], &fileinfo);

	return 0;
}

