
typedef struct _GiborNoiseParams
{
	real	lambda;
	real	r;
	uint	seed;
	real3	omega;
	real	a;
	int		type;
	bool	filter;
	
} GiborNoiseParams;


real3 GaborNoise_Perp (const real3 *v)
{
	real3 abs_v = fabs( *v );
	
	if ( (abs_v.x <= abs_v.y) and (abs_v.x <= abs_v.z) )
		return (real3)( 0.0, -v->z, v->y );
	else
	if ( abs_v.y <= abs_v.z )
		return (real3)( -v->z, 0.0, v->x );
	else
		return (real3)( -v->y, v->x, 0.0 );
}


real GaborNoise (const GiborNoiseParams *params, real3 position, real3 normal)
{
  noise n;
  noise_constructor(n, lambda, r, seed, omega, a, type, filter);

  real3	tangent  = normalize( GaborNoise_Perp( &normal ) );
  real3	binormal = cross( normal, tangent );

  real2x2 m_src_tan;
  {
	  real3x3  m3x3;
	  real2x2  m2x2, left;
	  real2x3  right;

	  Mat3x3( &m3x3, &tangent, &binormal, &normal );
	  Mat2x2_Mat3x3( &m2x2, &m3x3 );
	  Mat2x2_Transpose( &left, &m2x2 );
	  Mat2x3( &right, position, position );

  }

  mat2 M_scr_tan = mat2(transpose(mat3(t_tex, b_tex, n_tex)) * mat2x3(dFdx(x_tex), dFdy(x_tex)));
  float sigma_f_scr = 0.5;
  mat2 Sigma_f_scr = (sigma_f_scr * sigma_f_scr) * mat2(1.0);
  mat2 Sigma_f_tan = M_scr_tan * Sigma_f_scr * transpose(M_scr_tan);

  float noise = noise_evaluate(n, x_tex, n_tex, t_tex, b_tex, Sigma_f_tan);
  
	// noise_variance(const in noise this_)

	real	scale	= 0.5 / (3.0 * sqrt( 1.0 / (4.0 * sqrt(2.0) * (params->a * params->a * params->a)) ));
	real	bias	= 0.5;

	return bias + (scale * noise);
}


/*
struct noise_prng
{
  uint x_;
};

void noise_prng_srand(inout noise_prng this_, const in uint s)
{
  this_.x_ = s;
}

uint noise_prng_rand(inout noise_prng this_)
{
  return this_.x_ *= 3039177861u;
}
      
float noise_prng_uniform_0_1(inout noise_prng this_)
{
  return float(noise_prng_rand(this_)) / float(4294967295u);
}

float noise_prng_uniform(inout noise_prng this_, const in float min_, const in float max_)
{
  return min_ + (noise_prng_uniform_0_1(this_) * (max_ - min_));
}

uint noise_prng_poisson(inout noise_prng this_, const in float mean)
{
  return uint(mean);
}

// -----------------------------------------------------------------------------

float noise_gabor_kernel_2d(const in float w, const in vec2 f, const in float phi, const in float a, const in vec2 x)
{
  // see Eqn. 1
  float g = exp(-pi * (a * a) * dot(x, x));
  float h = cos((2.0 * pi * dot(f, x)) + phi);
  return w * g * h;
}

float noise_gabor_kernel_3d(const in float w, const in vec3 f, const in float phi, const in float a, const in vec3 x)
{
  // see Eqn. 1
  float g = exp(-pi * (a * a) * dot(x, x));
  float h = cos((2.0 * pi * dot(f, x)) + phi);
  return w * g * h;
}

void noise_transform_gabor_kernel_3d(const in mat3 m, const in float w, const in float a, const in vec3 omega, const in float phi, out float w_t, out float a_t, out vec3 omega_t, out float phi_t)
{
  // see Sec. 5 (Transforming the Phase-Augmented Gabor kernel)
  w_t = w;
  a_t = a;
  omega_t = m * omega;
  phi_t = phi;
}

void noise_slice_gabor_kernel_3d(float d, const in float w, const in float a, const in vec3 omega, const in float phi, out float w_s, out float a_s, out vec2 omega_s, out float phi_s)
{
  // see Eqn. 6
  w_s = w * exp(-(pi * (a * a) * (d * d)));
  a_s = a;
  omega_s = omega.xy;
  phi_s = phi - (2 * pi * d * omega.z);  
}

void noise_filter_gabor_kernel_2d(const in mat2 filter, const in float w, const in float a, const in vec2 omega, const in float phi, out float w_f, out float a_f, out vec2 omega_f, out float phi_f)
{
  // see Eqn. 10
  mat2 Sigma_f = filter;
  float c_G = w;
  vec2 mu_G = omega;
  mat2 Sigma_G = ((a * a) / (2.0 * pi)) * mat2(1.0);
  float c_F = 1.0 / (2.0 * pi * sqrt(determinant(Sigma_f)));
  mat2 Sigma_F = (1.0 / (4.0 * (pi * pi))) * inverse(Sigma_f);
  mat2 Sigma_G_Sigma_F = Sigma_G + Sigma_F;
  float c_GF = c_F * c_G * ((1.0 / (2.0 * pi * sqrt(determinant(Sigma_G_Sigma_F)))) * exp(-((1.0 / 2.0) * (dot(mu_G * inverse(Sigma_G_Sigma_F), mu_G)))));
  mat2 Sigma_GF = inverse(inverse(Sigma_G) + inverse(Sigma_F));
  vec2 mu_GF = Sigma_GF * inverse(Sigma_G) * mu_G;
  w_f = c_GF;
  a_f = sqrt(2.0 * pi * sqrt(determinant(Sigma_GF)));
  omega_f = mu_GF;
  phi_f = phi;
}

// -----------------------------------------------------------------------------

struct noise
{
  float lambda_;
  float r_;
  uint seed_;
  vec3 omega_;
  float a_;
  int type_;
  bool filter_;
};

void noise_constructor(out noise this_, const in float lambda, const in float r, const in uint seed, const in vec3 omega, const in float a, const in int type, const in bool filter)
{
  this_.lambda_ = lambda;
  this_.r_ = r;
  this_.seed_ = seed;
  this_.omega_ = omega;
  this_.a_ = a;
  this_.type_ = type;
  this_.filter_ = filter;
}

void noise_sample(const in noise this_, inout noise_prng prng, out float w, out vec3 omega, out float phi, out float a)
{
  // see Sec. 3.3 (Different Kinds of Solid Random-Phase Gabor Noise)
  w = 1.0;
  if (this_.type_ == 0) {
    // anisotropic
    omega = this_.omega_;
  }
  else if (this_.type_ == 1) {
     // isotropic
    float f = length(this_.omega_);
    float omega_r = f;
    float omega_t = noise_prng_uniform(prng, 0.0, 2.0 * pi);
    float omega_p = acos(noise_prng_uniform(prng, -1.0, +1.0));
    omega = omega_r * vec3(cos(omega_t) * sin(omega_p), sin(omega_t) * sin(omega_p), cos(omega_p));
  }
  else {
    // hybrid
    float f = length(this_.omega_);
    float omega_r = f;
    float omega_t = noise_prng_uniform(prng, 0.0, 2.0 * pi);
    omega = omega_r * vec3(cos(omega_t), sin(omega_t), 0.0);
  }
  phi = noise_prng_uniform(prng, 0.0, 2.0 * pi);
  a = this_.a_;
}

float noise_cell(const in noise this_, const in ivec3 c, const in vec3 x_c, const in vec3 n, const in vec3 t, const in vec3 b, const in mat2 filter)
{
  const uint period = 256u;
  uint seed = (((((uint(c[2]) % period) * period) + (uint(c[1]) % period)) * period) + (uint(c[0]) % period)) + this_.seed_;
  if (seed == 0u) seed = 1u;
  noise_prng prng;
  noise_prng_srand(prng, seed);
  uint number_of_impulses = noise_prng_poisson(prng, this_.lambda_ * this_.r_ * this_.r_ * this_.r_);
  float sum = 0.0;
  for (uint i = 0u; i < number_of_impulses; ++i) {
    vec3 x_i_c = vec3(noise_prng_uniform_0_1(prng), noise_prng_uniform_0_1(prng), noise_prng_uniform_0_1(prng));
    vec3 x_k_i = this_.r_ * (x_c - x_i_c);
    float w_i; vec3 omega_i; float phi_i, a_i;
    noise_sample(this_, prng, w_i, omega_i, phi_i, a_i);
    if (dot(x_k_i, x_k_i) < (this_.r_ * this_.r_)) {
      if (this_.filter_ == false) {
        sum += noise_gabor_kernel_3d(w_i, omega_i, phi_i, a_i, x_k_i);
      }
      else {
        float d_i = -dot(n, x_k_i);
        mat3 m = transpose(mat3(t, b, n));
        float w_i_t; float a_i_t; vec3 omega_i_t; float phi_i_t;
        noise_transform_gabor_kernel_3d(m, w_i, a_i, omega_i, phi_i, w_i_t, a_i_t, omega_i_t, phi_i_t);        
        float w_i_t_s; float a_i_t_s; vec2 omega_i_t_s; float phi_i_t_s;
        noise_slice_gabor_kernel_3d(d_i, 1.0, a_i_t, omega_i_t, phi_i_t, w_i_t_s, a_i_t_s, omega_i_t_s, phi_i_t_s);
        float w_i_t_s_f; float a_i_t_s_f; vec2 omega_i_t_s_f; float phi_i_t_s_f;
        noise_filter_gabor_kernel_2d(filter, w_i_t_s, a_i_t_s, omega_i_t_s, phi_i_t_s, w_i_t_s_f, a_i_t_s_f, omega_i_t_s_f, phi_i_t_s_f);
        sum += noise_gabor_kernel_2d(w_i_t_s_f, omega_i_t_s_f, phi_i_t_s_f, a_i_t_s_f, (m * x_k_i).xy);
      }
    }
  }
  return sum;
}

float noise_grid(const in noise this_, const in vec3 x_g, const in vec3 n, const in vec3 t, const in vec3 b, const in mat2 filter)
{
  vec3 int_x_g = floor(x_g);
  ivec3 c = ivec3(int_x_g);
  vec3 x_c = x_g - int_x_g;
  float sum = 0.0;
  ivec3 i;
  for (i[2] = -1; i[2] <= +1; ++i[2]) {
    for (i[1] = -1; i[1] <= +1; ++i[1]) {
      for (i[0] = -1; i[0] <= +1; ++i[0]) {
        ivec3 c_i = c + i;
        vec3 x_c_i = x_c - i;
        sum += noise_cell(this_, c_i, x_c_i, n, t, b, filter);
      }
    }
  }
  return sum / sqrt(this_.lambda_);
}

float noise_evaluate(const in noise this_, const in vec3 x, const in vec3 n, const in vec3 t, const in vec3 b, const in mat2 filter)
{
  vec3 x_g = x / this_.r_;
  return noise_grid(this_, x_g, n, t, b, filter);
}

float noise_variance(const in noise this_)
{
  return (1.0 / (4.0 * sqrt(2.0) * (this_.a_ * this_.a_ * this_.a_)));
}

in vec3 x_tex;
in vec3 n_tex_;

out vec4 gl_FragColor;

vec3 perp(in vec3 v)
{
  vec3 abs_v = abs(v);
  if ((abs_v.x <= abs_v.y) and (abs_v.x <= abs_v.z)) {
    return vec3(0, -v.z, v.y);
  }
  else if (abs_v.y <= abs_v.z) {
    return vec3(-v.z, 0, v.x);
  }
  else {
    return vec3(-v.y, v.x, 0);
  }
}

void main()
{  
  noise n;
  noise_constructor(n, lambda, r, seed, omega, a, type, filter);
  vec3 n_tex = normalize(n_tex_);

  // see Sec. 5 (Continuity of the Local Coordinate System)
  vec3 t_tex = normalize(perp(n_tex));
  vec3 b_tex = cross(n_tex, t_tex);

  // see Sec. 5 (Definition of the Filtering Gaussian)
  mat2 M_scr_tan = mat2(transpose(mat3(t_tex, b_tex, n_tex)) * mat2x3(dFdx(x_tex), dFdy(x_tex)));
  float sigma_f_scr = 0.5;
  mat2 Sigma_f_scr = (sigma_f_scr * sigma_f_scr) * mat2(1.0);
  mat2 Sigma_f_tan = M_scr_tan * Sigma_f_scr * transpose(M_scr_tan);

  float noise = noise_evaluate(n, x_tex, n_tex, t_tex, b_tex, Sigma_f_tan);

  float scale = 0.5 / (3.0 * sqrt(noise_variance(n)));
  float bias = 0.5;
  float color = bias + (scale * noise);
  gl_FragColor = vec4(vec3(color), 1);
}
*/