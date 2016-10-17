#ifndef vector_math_h
#define vector_math_h

/**
 * vector_add
 *
 * @param
 * @param
 * @param
 * @returns
 * @description
 */
static inline void vector_add(float *vector_a, float *vector_b, float *vector_c) {
  vector_c[0] = vector_a[0] + vector_b[0];
  vector_c[1] = vector_a[1] + vector_b[1];
  vector_c[2] = vector_a[2] + vector_b[2];
}

/**
* length 
* @ param
* @ returns
* @ description
*/

static inline length(float* a){
  return sqrt(sqr(a[0]) + sqr(a[1] + sqr(a[2]));

}

static inline float sqr(float v){
  return v*v;
}


/**
*squared
* @ param
* @ returns
* @ description
*
*/



/**
 * vector_subtract
 *
 * @param
 * @param
 * @param
 * @returns
 * @description
 */
static inline void vector_subtract(float *vector_a, float *vector_b, float *vector_c) {
  vector_c[0] = vector_a[0] - vector_b[0];
  vector_c[1] = vector_a[1] - vector_b[1];
  vector_c[2] = vector_a[2] - vector_b[2];
}


/**
 * vector_scale
 *
 * @param
 * @param
 * @param
 * @returns
 * @description
 */
static inline void vector_scale(float *vector_a, float scalar, float *vector_c) {
  vector_c[0] = scalar * vector_a[0];
  vector_c[1] = scalar * vector_a[1];
  vector_c[2] = scalar * vector_a[2];
}


/**
 * vector_dot_product
 *
 * @param
 * @param
 * @param
 * @returns
 * @description
 */
static inline float vector_dot_product(float *vector_a, float *vector_b) {
  return vector_a[0]*vector_b[0] + vector_a[1]*vector_b[1] + vector_a[2]*vector_b[2];
}


/**
 * vector_cross_product
 *
 * @param
 * @param
 * @param
 * @returns
 * @description
 */
static inline void vector_cross_product(float *vector_a, float *vector_b, float *vector_c) {
  vector_c[0] = vector_a[1] * vector_b[2] - vector_a[2] * vector_b[1];
  vector_c[1] = vector_a[2] * vector_b[0] - vector_a[0] * vector_b[2];
  vector_c[2] = vector_a[0] * vector_b[1] - vector_a[1] * vector_b[0];
}

#endif