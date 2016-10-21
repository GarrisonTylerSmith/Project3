#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

# define T_SPHERE 1
# define T_PLANE 2
# define T_CYLINDER 3
# define T_CAMERA 4
# define T_LIGHT 5
int line = 1;
typedef struct{
  int kind; // 0 = cylinder, 1 = sphere, 2 = plane
  float color[3];
  float position[3];
  float direction[3];
  float specular_color[3];
  float a;
  float b;
  float c;
  float d;
  union{
    struct {
      float center[3];
      float radius;
      float height;
    }cylinder;
    struct{
      float center[3];
      float radius;
      float diffuse_color[3];
    }sphere;
    struct{
      float position;
      float normal;
    }plane;
    struct{
      float position;
      float width;
      float height;
    }camera;
    struct{
      float position;
      float direction;
      float color;
      float radial_a0;
      float radial_a1;
      float radial_a2;
      float angular_a0;
      float theta;
    }light;
  };
}Object;

typedef struct {
      float position;
      float direction;
      float color;
      float radial_a0;
      float radial_a1;
      float radial_a2;
      float angular_a0;
}Light;
typedef struct {
  float vect_point[3];
  int  object_id;

}Intersection;
typedef struct{
  int num_objects;
  Object objects[128];
  float camera_width;
  float camera_height;
  float camera_position[3];
  float camera_facing[3];
  float background_color[3];
  float ambient_color[3];
  int num_lights;
  Object lights[128];
}Scene;

// next_c() wraps the getc() function and provides error checking and line
// number maintenance
int next_c(FILE* json) {
  int c = fgetc(json);
#ifdef DEBUG
  printf("next_c: '%c'\n", c);
#endif
  if (c == '\n') {
    line += 1;
  }
  if (c == EOF) {
    fprintf(stderr, "Error: Unexpected end of file on line number %d.\n", line);
    exit(1);
  }
  return c;
}

// expect_c() checks that the next character is d.  If it is not it emits
// an error.
void expect_c(FILE* json, int d) {
  int c = next_c(json);
  if (c == d) return;
  fprintf(stderr, "Error: Expected '%c' on line %d.\n", d, line);
  exit(1);    
}


// skip_ws() skips white space in the file.
void skip_ws(FILE* json) {
  int c = next_c(json);
  while (isspace(c)) {
    c = next_c(json);
  }
  ungetc(c, json);
}


// next_string() gets the next string from the file handle and emits an error
// if a string can not be obtained.
char* parse_string(FILE* json) {
  char buffer[129];
  int c = next_c(json);
  if (c != '"') {
    fprintf(stderr, "Error: Expected string on line %d.\n", line);
    exit(1);
  }  
  c = next_c(json);
  int i = 0;
  while (c != '"') {
    if (i >= 128) {
      fprintf(stderr, "Error: Strings longer than 128 characters in length are not supported.\n");
      exit(1);      
    }
    if (c == '\\') {
      fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
      exit(1);      
    }
    if (c < 32 || c > 126) {
      fprintf(stderr, "Error: Strings may contain only ascii characters.\n");
      exit(1);
    }
    buffer[i] = c;
    i += 1;
    c = next_c(json);
  }
  buffer[i] = 0;
  return strdup(buffer);
}
// this function checks teh next number in the ray
float next_number(FILE* json) {
  float value;
  fscanf(json, "%f", &value);
  // Error check this..
  return value;
}
// This is the function that checks the next vector of the object
float* next_vector(FILE* json) {
  float* v = malloc(3 * sizeof(float));
  expect_c(json, '[');
  skip_ws(json);
  v[0] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[1] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[2] = next_number(json);
  skip_ws(json);
  expect_c(json, ']');
  return v;
}


Scene read_scene(char* json_name){
  FILE * json = fopen(json_name, "r");
  Scene scene;

  int c;
  
  skip_ws(json);
  
  // We want to check the beginning of the json file
  expect_c(json, '[');
  
  skip_ws(json);
  
  c = next_c(json);
  if (c == ']'){
    fprintf(stderr, "Warning: empty scene file.\n");
    return scene;
  }

  ungetc(c, json);
  skip_ws(json);
  
  int set_camera_width = 0;
  float camera_width = 0;
  int set_camera_height = 0;
  float camera_height = 0;
  
  while(1){
    expect_c(json, '{');
    
    skip_ws(json);

    // here is where we want to parse through the object 
    // that is given
    char* key = parse_string(json);
    if (strcmp(key, "type") != 0) {
      fprintf(stderr, "Error: expected \"type\" key on line %d\n", line);
      exit(1);
    }
    
    skip_ws(json);
    expect_c(json, ':');
    skip_ws(json);
    char* type_value = parse_string(json);

    int objtype = 0;
    
    int set_radius = 0;
    float radius = 0;
    int set_radial_a0 = 0;
    float radial_a0 = 0; 
    int set_radial_a1 = 0;
    float radial_a1 = 0; 
    int set_radial_a2 = 0;
    float radial_a2 = 0;
    int set_angular_a0 = 0;
    float angular_a0  =0;
    int set_color = 0;
    float color[3];
    int set_normal = 0;
    float normal[3];
    int set_position = 0;
    float position[3];
    int set_direction =0;
    float direction[3];
    int set_diffuse_color =0;
    float diffuse_color[3];
    int set_specular_color =0;
    float specular_color[3];
    float set_theta = 0;
    float theta = 0;

    Object new_object = scene.objects[scene.num_objects];
    
    if(strcmp(type_value, "camera") == 0) {
      objtype = T_CAMERA;
    } else if(strcmp(type_value, "sphere") == 0) {
      objtype = T_SPHERE;
    } else if(strcmp(type_value, "plane") == 0) {
      objtype = T_PLANE;
    } else if(strcmp(type_value, "light") == 0){
      objtype = T_LIGHT;
    }else{
      
      fprintf(stderr, "Unknown type \"%s\" on line %d\n", type_value, line);
      exit(1);
    }
    
    // takes the information that we had and copies it to the new object
    new_object.kind = objtype;

    int finish = 0;

    while(1) {
      skip_ws(json);
      c = next_c(json);

      if (c == '}'){
        // here we have now broken out of the parsing of the current object
        break;
      }
      else{
        if(finish){
          fprintf(stderr, "Expected , and got end of object on line %d\n", line);
          exit(1);
        }
        // Now we read in the new field of the file
        skip_ws(json);
        char* key = parse_string(json);
        skip_ws(json);
        expect_c(json, ':');
        skip_ws(json);

        if(strcmp(key, "width") == 0){
          if(objtype == T_CAMERA){
            float value = next_number(json);
            camera_width = value;
            set_camera_width = 1;
          }
        }
        else if(strcmp(key, "height") == 0){
          if(objtype == T_CAMERA){
            float value = next_number(json);
            camera_height = value;
            set_camera_height = 1;
          }
        }
        else if(strcmp(key, "radius") == 0){
          if(objtype == T_SPHERE){
            float value = next_number(json);
            radius = value;
            set_radius = 1;
          }
        }
        else if(strcmp(key, "diffuse_color") == 0){
          float* v3 = next_vector(json);
          color[0] = v3[0];
          color[1] = v3[1];
          color[2] = v3[2];
          set_color = 1;
        }
        else if(strcmp(key, "position") == 0){
          float* v3 = next_vector(json);
          position[0] = v3[0];
          position[1] = v3[1];
          position[2] = v3[2];
          set_position = 1;
        }
        else if(strcmp(key, "normal") == 0){
          float* v3 = next_vector(json);
          normal[0] = v3[0];
          normal[1] = v3[1];
          normal[2] = v3[2];
          set_normal = 1;
        }
        else if(strcmp(key, "direction")==0){
          float* v3 = next_vector(json);
          direction[0] = v3[0];
          direction[1] = v3[1];
          direction[2] = v3[2];
          set_direction = 1;
        }
        else if(strcmp(key, "diffuse_color")==0){
          if(objtype == T_SPHERE){
            float* v3 = next_vector(json);
            diffuse_color[0] = v3[0];
            diffuse_color[1] = v3[1];
            diffuse_color[2] = v3[2];
            set_diffuse_color = 1;
          }

        }
        else if(strcmp(key, "specular_color")==0){
          if(objtype == T_SPHERE){
            float* v3 = next_vector(json);
            specular_color[0] = v3[0];
            specular_color[1] = v3[1];
            specular_color[2] = v3[2];
            set_specular_color = 1;
          }
        }
        else if(strcmp(key, "color")==0){
          if(objtype == T_LIGHT){
            float* v3 = next_vector(json);
            color[0] = v3[0];
            color[1] = v3[1];
            color[2] = v3[2];
            set_color = 1;
          }
        }
        else if(strcmp(key, "radial-a0") == 0){
          if(objtype == T_LIGHT){
            float value = next_number(json);
            radial_a0 = value;
            set_radial_a0 = 1;
          }
        }
        else if(strcmp(key, "radial-a1") == 0){
          if(objtype == T_LIGHT){
            float value = next_number(json);
            radial_a1 = value;
            set_radial_a1 = 1;
          }
        }
        else if(strcmp(key, "radial-a2") == 0){
          if(objtype == T_LIGHT){
            float value = next_number(json);
            radial_a2 = value;
            set_radial_a2 = 1;
          }
        }
        else if(strcmp(key, "angular-a0") == 0){
          if(objtype == T_LIGHT){
            float value = next_number(json);
            angular_a0 = value;
            set_angular_a0 = 1;
          }
        }
        else if(strcmp(key, "theta") == 0){
          if(objtype == T_LIGHT){
            float value = next_number(json);
            theta = value;
            set_theta = 1;
          }
        }
        else{
          fprintf(stderr, "Error: unknown property: %s on line %d\n", key, line);
          exit(1);
        }

        skip_ws(json);
        c = next_c(json);

        if(c != ',')
          finish = 1;

        ungetc(c, json);
      }
    }
    // error checking to make sure the correct attributes were read in
    // and set the attributes to the last object in the buffers
    if(objtype == T_CAMERA){
      if(set_camera_height != 1){
        fprintf(stderr, "Camera must have a height! Line %d\n", line);
        exit(1);
      }
      if(set_camera_width != 1){
        fprintf(stderr, "Camera must have a width! Line %d\n", line);
        exit(1);
      }
      if(set_position == 1){
        scene.camera_position[0] = position[0];
        scene.camera_position[1] = position[1];
        scene.camera_position[2] = position[2];
      }
      if(set_normal == 1)
        printf("Warning: Camera does not use \"normal\" at this time!\n");
      
      scene.camera_width = camera_width;
      scene.camera_height = camera_height;
      
    }
    if(objtype == T_SPHERE){
      if(set_radius != 1){
        fprintf(stderr, "Sphere must have a defined radius! Line %d\n", line);
        exit(1);
      }
      if(radius < 0){
        fprintf(stderr, "Sphere must have a non-negative radius! Line %d\n", line);
        exit(1);
      }
      if(set_color != 1){
        fprintf(stderr, "Sphere must have a color! Line %d\n", line);
        exit(1);
      }
      if(set_position != 1){
        fprintf(stderr, "Sphere must have a position! Line %d\n", line);
        exit(1);
      }
      // This gets our properties that we use for the sphere object

      new_object.color[0] = color[0];
      new_object.color[1] = color[1];
      new_object.color[2] = color[2];
      
      new_object.a = position[0];
      new_object.b = position[1];
      new_object.c = position[2];
      new_object.d = radius;
      
    }
    if(objtype == T_LIGHT){
      if(set_color != 1){
        fprintf(stderr, "Light must have a color! Line %d\n", line);
        exit(1);
      }
      if(set_position != 1){
        fprintf(stderr, "Light must have a position! Line %d\n", line);
        exit(1);
      }
      if(radial_a0 < 0){
        fprintf(stderr, "light must have a non-negative radial_a0! Line %d\n", line);
        exit(1);
      }
      if(radial_a1 < 0){
        fprintf(stderr, "light must have a non-negative radial_a1! Line %d\n", line);
        exit(1);
      }
      if(radial_a2 < 0){
        fprintf(stderr, "light must have a non-negative radial_a2! Line %d\n", line);
        exit(1);
      }
      if(angular_a0 < 0){
        fprintf(stderr, "light must have a non-negative angular_a0! Line %d\n", line);
        exit(1);
      }
      if(theta < 0 && theta >= 180){
        fprintf(stderr, "light must have a less than or equal to 180 but not a non-negative! Line %d\n", line);
        exit(1);
      }
      // get properties for a light
      new_object.color[0] = color[0];
      new_object.color[1] = color[1];
      new_object.color[2] = color[2];
      
      new_object.a = position[0];
      new_object.b = position[1];
      new_object.c = position[2];
      //new_object.d = direction;
    }
    if(objtype == T_PLANE){
      if(set_color != 1){
        fprintf(stderr, "Object must have a color! Line %d\n", line);
        exit(1);
      }
      if(set_position != 1){
        fprintf(stderr, "Object must have a position! Line %d\n", line);
        exit(1);
      }
      if(set_normal != 1){
        fprintf(stderr, "Plane must have a normal vector! Line %d\n", line);
        exit(1);
      }      
      // This get our properties for our plane object
      new_object.color[0] = color[0];
      new_object.color[1] = color[1];
      new_object.color[2] = color[2];
      
      new_object.a = normal[0];
      new_object.b = normal[1];
      new_object.c = normal[2];
      new_object.d = normal[0] * position[0] + normal[1] * position[1] + normal[2] * position[2];
      
    }
    // This gets to the next number for the object by incrementing 

    if(objtype != T_CAMERA){
      scene.objects[scene.num_objects] = new_object;
      scene.num_objects ++;
    } 
    
    if(objtype == T_LIGHT){
      scene.lights[scene.num_lights] = new_object;
      scene.num_lights ++;
    }
    skip_ws(json);
    c = next_c(json);
    
    if (c == ','){
      skip_ws(json);
    }
    else if (c == ']') {
      fclose(json);
      return scene;
    }
    else {
      fprintf(stderr, "Error: Expected ] or , on line %d\n", line);
      exit(1);
    }
    
    // With the new field of the file we now end the parsing once again
    skip_ws(json);
  }
  
  fclose(json);

  return scene;
}

// int main(int c, char** argv) {
//   read_scene(argv[1]);
//   return 0;
// }