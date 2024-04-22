// created by Andrey Vasiliev

#define HAS_MRT

// Clamps color between 0 and 1 smoothly
vec3 SkyLightExpose(const vec3 color)
{
    return 1.0 - exp(-0.1 * color);
}
//vec3 SkyLightExpose(const vec3 color, float exposure)
//{
//    return 2.0 / (1.0 + exp(-exposure * color)) - 1.0;
//}

vec3 HosekWilkie(float gamma, float cos_gamma, float cos_theta, const vec3 coeffs[10])
{
    vec3 A = coeffs[0];
    vec3 B = coeffs[1];
    vec3 C = coeffs[2];
    vec3 D = coeffs[3];
    vec3 E = coeffs[4];
    vec3 F = coeffs[5];
    vec3 G = coeffs[6];
    vec3 H = coeffs[7];
    vec3 I = coeffs[8];
    vec3 Z = coeffs[9];

    float cos_gamma2 = cos_gamma * cos_gamma;
    vec3 I2 = I * I;
    vec3 chi = ((1.0 + cos_gamma2) / pow(1.0 + I2 - 2.0 * (cos_gamma * I), vec3(1.5, 1.5, 1.5)));
    vec3 color =  Z * ((1.0 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E * gamma) + F * cos_gamma2 + G * chi + H * sqrt(cos_theta)));
    return SkyLightExpose(color);
}

vec3 HosekWilkie(const highp vec3 uv, const highp vec3 lightDir, const vec3 coeffs[10])
{
    vec3 A = coeffs[0];
    vec3 B = coeffs[1];
    vec3 C = coeffs[2];
    vec3 D = coeffs[3];
    vec3 E = coeffs[4];
    vec3 F = coeffs[5];
    vec3 G = coeffs[6];
    vec3 H = coeffs[7];
    vec3 I = coeffs[8];
    vec3 Z = coeffs[9];

    highp vec3 V = normalize(uv);
    vec3 N = normalize(lightDir);
    V.y = abs(V.y);
    if (abs(V.y) < 0.001) V.y = 0.001;
    float cos_theta = clamp(V.y, 0.0, 1.0);
    float cos_gamma = dot(V, N);
    float gamma = acos(cos_gamma);
    float cos_gamma2 = cos_gamma * cos_gamma;
    vec3 I2 = I * I;

    vec3 chi = ((1.0 + cos_gamma2) / pow(1.0 + I2 - 2.0 * (cos_gamma * I), vec3(1.5, 1.5, 1.5)));
    vec3 color =  Z * ((1.0 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E * gamma) + F * cos_gamma2 + G * chi + H * sqrt(cos_theta)));
    return SkyLightExpose(color);
}

//group_uniforms sky;
const vec3 day_top_color = vec3( 0.1, 0.6, 1.0 );
const vec3 day_bottom_color = vec3( 0.4, 0.8, 1.0 );
const vec3 sunset_top_color = vec3( 0.7, 0.75, 1.0 );
const vec3 sunset_bottom_color = vec3( 1.0, 0.5, 0.7 );
const vec3 night_top_color = vec3( 0.02, 0.0, 0.04 );
const vec3 night_bottom_color = vec3( 0.1, 0.0, 0.2 );

//group_uniforms horizon;
const vec3 horizon_color = vec3( 0.0, 0.7, 0.8 );
const float horizon_blur = 0.05;

//group_uniforms sun; // First DirectionalLight3D will be the sun
const vec3 sun_color = vec3( 10.0, 8.0, 1.0 );
const vec3 sun_sunset_color = vec3( 10.0, 0.0, 0.0 );
const float sun_size = 0.2;
const float sun_blur = 10.0;

//group_uniforms moon; // Second DirectionalLight3D will be the moon
const vec3 moon_color = vec3( 1.0, 0.95, 0.7 );
const float moon_size = 0.06;
const float moon_blur = 0.1;

//group_uniforms stars;
// Stars should be at black background
uniform sampler2D StarsTex;
const float stars_speed = 1.0;

//group_uniforms settings;
//uniform float overwritten_time = 0.0;

// Replaced by noise functions, unncomment if you want to use graphical textures
//	uniform sampler2D clouds_top_texture : filter_linear_mipmap, hint_default_black;
//	uniform sampler2D clouds_middle_texture : filter_linear_mipmap, hint_default_black;
//	uniform sampler2D clouds_bottom_texture : filter_linear_mipmap, hint_default_black;
const vec3 clouds_edge_color = vec3( 0.8, 0.8, 0.98 );
const vec3 clouds_top_color = vec3( 1.0, 1.0, 1.00 );
const vec3 clouds_middle_color = vec3( 0.92, 0.92, 0.98 );
const vec3 clouds_bottom_color = vec3( 0.83, 0.83, 0.94 );
const float clouds_speed  = 2.0;
const float clouds_direction  = 0.2;
const float clouds_scale = 1.0;
const float clouds_cutoff = 0.3;
const float clouds_fuzziness = 0.5;
// More weight is simply a darker color, usefull for rain/storm
const float clouds_weight  = 0.0;
const float clouds_blur  = 0.25;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Function for clouds noises. You can replace using "gen_fractal_ping_pong" with a simple texture reading.
// I was frustrated with the repeating texture that's why I included the algorithm in the code.
// Source: https://github.com/Auburn/FastNoiseLite/tree/master
const int PRIME_X = 501125321;
const int PRIME_Y = 1136930381;
float _lerp( float a, float b,  float t )
{
    return a + t * ( b - a );
}

float cubic_lerp(float a, float b, float c, float d, float t)
{
    float p = d - c - ( a - b );
    return t * t * t * p + t * t * ( a - b - p ) + t * ( c - a ) + b;
}

float ping_pong(float t)
{
    t -= trunc( t * 0.5 ) * 2.0;
    return t < 1.0 ? t : 2.0 - t;
}

int hash(int seed, int x_primed, int y_primed)
{
    return ( seed ^ x_primed ^ y_primed ) * 0x27d4eb2d;
}

precision highp float;
float val_coord(int seed, int x_primed, int y_primed )
{
    int hash = hash( seed, x_primed, y_primed );
    hash *= hash;
    hash ^= hash << 19;
    return float( hash ) * ( 1.0 / 2147483648.0 );
}
precision mediump float;

float single_value_cubic(int seed, float x, float y)
{
    int x1 = int( floor( x ));
    int y1 = int( floor( y ));

    float xs = x - float( x1 );
    float ys = y - float( y1 );

    x1 *= PRIME_X;
    y1 *= PRIME_Y;
    int x0 = x1 - PRIME_X;
    int y0 = y1 - PRIME_Y;
    int x2 = x1 + PRIME_X;
    int y2 = y1 + PRIME_Y;
    int x3 = x1 + ( PRIME_X << 1 );
    int y3 = y1 + ( PRIME_Y << 1 );

    return cubic_lerp(
    cubic_lerp( val_coord( seed, x0, y0 ), val_coord( seed, x1, y0 ), val_coord( seed, x2, y0 ), val_coord( seed, x3, y0 ), xs ),
    cubic_lerp( val_coord( seed, x0, y1 ), val_coord( seed, x1, y1 ), val_coord( seed, x2, y1 ), val_coord( seed, x3, y1 ), xs ),
    cubic_lerp( val_coord( seed, x0, y2 ), val_coord( seed, x1, y2 ), val_coord( seed, x2, y2 ), val_coord( seed, x3, y2 ), xs ),
    cubic_lerp( val_coord( seed, x0, y3 ), val_coord( seed, x1, y3 ), val_coord( seed, x2, y3 ), val_coord( seed, x3, y3 ), xs ),
    ys ) * ( 1.0 / ( 1.5 * 1.5 ));
}

// Params can be change in the same way as in noise settings in Godot
const float FRACTAL_BOUNDING = 1.0 / 1.75;
const int OCTAVES = 5;
const float PING_PONG_STRENGTH = 2.0;
const float WEIGHTED_STRENGTH = 0.0;
const float GAIN = 0.5;
const float LACUNARITY = 2.0;
float gen_fractal_ping_pong(const highp vec2 pos, int seed, float frequency )
{
    float x = pos.x * frequency;
    float y = pos.y * frequency;
    float sum = 0.0;
    float amp = FRACTAL_BOUNDING;
    for( int i = 0; i < OCTAVES; ++i )
    {
        float noise = ping_pong(( single_value_cubic( seed++, x, y ) + 1.0 ) * PING_PONG_STRENGTH );
        sum += ( noise - 0.5 ) * 2.0 * amp;
        amp *= _lerp( 1.0, noise, WEIGHTED_STRENGTH );
        x *= LACUNARITY;
        y *= LACUNARITY;
        amp *= GAIN;
    }
    return sum * 0.5 + 0.5;
}
////////////////////////////////////////////////////////////////////////////////////////////////////

vec3 clouds(const highp vec3 uv, const vec3 _sky_color, const vec3 lightColor, const vec3 lightDir, const float time)
{
    // Clouds UV movement direction
    vec2 _sky_uv = uv.xz / sqrt( uv.y );
    //vec3 lightDir = -LightDir0;
    float _sunset_amount = clamp( 0.5 - abs( lightDir.y ), 0.0, 0.5 ) * 2.0;
    float _sun_distance = distance( uv, lightDir.xyz );
    float _night_amount = clamp( -lightDir.y + 0.7, 0.0, 1.0 );

    float _clouds_speed = time * clouds_speed * 0.01;
    float _sin_x = sin( clouds_direction * M_PI * 2.0 );
    float _cos_y = cos( clouds_direction * M_PI * 2.0 );
    // I using 3 levels of clouds. Top is the lightes and botom the darkest.
    // The speed of movement (and direction a little) is different for the illusion of the changing shape of the clouds.
    vec2 _clouds_movement = vec2( _sin_x, _cos_y ) * _clouds_speed;
    //		float _noise_top = texture( clouds_top_texture, ( _sky_uv + _clouds_movement ) * clouds_scale ).r;
    float _noise_top = gen_fractal_ping_pong( ( _sky_uv + _clouds_movement ) * clouds_scale, 0, 0.5 );
    _clouds_movement = vec2( _sin_x * 0.97, _cos_y * 1.07 ) * _clouds_speed * 0.89;
    //		float _noise_middle = texture( clouds_middle_texture, ( _sky_uv + _clouds_movement ) * clouds_scale ).r;
    float _noise_middle = gen_fractal_ping_pong( ( _sky_uv + _clouds_movement ) * clouds_scale, 1, 0.75 );
    _clouds_movement = vec2( _sin_x * 1.01, _cos_y * 0.89 ) * _clouds_speed * 0.79;
    //		float _noise_bottom = texture( clouds_bottom_texture, ( _sky_uv + _clouds_movement ) * clouds_scale ).r;
    float _noise_bottom = gen_fractal_ping_pong( ( _sky_uv + _clouds_movement ) * clouds_scale, 2, 1.0 );
    // Smoothstep with the addition of a noise value from a lower level gives a nice, deep result
    _noise_bottom = smoothstep( clouds_cutoff, clouds_cutoff + clouds_fuzziness, _noise_bottom );
    _noise_middle = smoothstep( clouds_cutoff, clouds_cutoff + clouds_fuzziness, _noise_middle + _noise_bottom * 0.2 ) * 1.1;
    _noise_top = smoothstep( clouds_cutoff, clouds_cutoff + clouds_fuzziness, _noise_top + _noise_middle * 0.4 ) * 1.2;
    float _clouds_amount = clamp( _noise_top + _noise_middle + _noise_bottom, 0.0, 1.0 );
    // Fading clouds near the horizon
    _clouds_amount *= clamp( abs( uv.y ) / clouds_blur, 0.0, 1.0 );

    vec3 _clouds_color = mix( vec3( 0.0 ), clouds_top_color, _noise_top );
    _clouds_color = mix( _clouds_color, clouds_middle_color, _noise_middle );
    _clouds_color = mix( _clouds_color, clouds_bottom_color, _noise_bottom );
    // The edge color gives a nice smooth edge, you can try turning this off if you need sharper edges
    _clouds_color = mix( clouds_edge_color, _clouds_color, _noise_top );
    // The sun passing through the clouds effect
    _clouds_color = mix( _clouds_color, clamp( lightColor, 0.0, 1.0 ), pow( 1.0 - clamp( _sun_distance, 0.0, 1.0 ), 5.0 ));
    // Color combined with sunset condition
    _clouds_color = mix( _clouds_color, sunset_bottom_color, _sunset_amount * 0.75 );
    // Color depending on the "progress" of the night.
    _clouds_color = mix( _clouds_color, _sky_color, clamp( _night_amount, 0.0, 0.98 ));
    _clouds_color = mix( _clouds_color, vec3( 0.0 ), clouds_weight * 0.9 );
    return mix( _sky_color, _clouds_color, _clouds_amount );
}

// Function needed to calculate the phase of the moon
// Source: https://kelvinvanhoorn.com/2022/03/17/skybox-tutorial-part-1/
float sphere_intersect( const vec3 view_dir, const vec3 sphere_pos, float radius )
{
    float b = dot( -sphere_pos, view_dir );
    float c = dot( -sphere_pos, -sphere_pos ) - pow( radius, 2.0 );
    float h = pow( b, 2.0 ) - c;
    return h < 0.0 ? -1.0 : -b - sqrt( h );
}

float _moon_amount = 0.0;
vec3 sky(const vec3 uv, const vec3 lightDir)
{
    //float time = overwritten_time != 0.0 ? overwritten_time : TIME;
    //vec3 lightDir = -LightDir0;
    vec3 color;

    //////////////////// SKY ///////////////////////////////////////////////////////////////////////
    float _eyedir_y = abs(sin(uv.y * M_PI * 0.5));

    // The day color will be our base color
    vec3 _sky_color = mix(day_bottom_color, day_top_color, _eyedir_y);
    _sky_color = mix(_sky_color, vec3(0.0), clamp((0.7 - clouds_cutoff) * clouds_weight, 0.0, 1.0));

    float _sunset_amount = clamp(0.5 - abs(lightDir.y), 0.0, 0.5) * 2.0;
    // The sky should be more red around the west, on the opposite side you don't see it as much
    float _sunset_distance = clamp(1.0 - pow(distance(uv, lightDir), 2.0), 0.0, 1.0);
    vec3 _sky_sunset_color = mix(sunset_bottom_color, sunset_top_color, _eyedir_y + 0.5);
    _sky_sunset_color = mix(_sky_sunset_color, sunset_bottom_color, _sunset_amount * _sunset_distance);
    _sky_color = mix(_sky_color, _sky_sunset_color, _sunset_amount);

    float _night_amount = clamp(-lightDir.y + 0.7, 0.0, 1.0);
    vec3 _sky_night_color = mix(night_bottom_color, night_top_color, _eyedir_y);
    _sky_color = mix(_sky_color, _sky_night_color, _night_amount);
    return _sky_color;
}

vec3 starfield(const vec3 uv, const vec3 lightDir, const float time)
{
    //float time = overwritten_time != 0.0 ? overwritten_time : TIME;
    //vec3 lightDir = -LightDir0;
    vec3 color;

    //////////////////// STARS /////////////////////////////////////////////////////////////////
    vec2 _sky_uv = uv.xz / sqrt(uv.y);
    if (uv.y > -0.01 && lightDir.y < 0.0)
    {
        // Stars UV rotation
        float _stars_speed_cos = cos(stars_speed * time * 0.0003);
        float _stars_speed_sin = sin(stars_speed * time * 0.0003);
        //        float _stars_speed_cos = cos(stars_speed * 0.005);
        //        float _stars_speed_sin = sin(stars_speed * 0.005);
        vec2 _stars_uv = vec2(
        _sky_uv.x * _stars_speed_cos - _sky_uv.y * _stars_speed_sin,
        _sky_uv.x * _stars_speed_sin + _sky_uv.y * _stars_speed_cos
        );
        // Stars texture
        vec3 _stars_color = texture2D(StarsTex, _stars_uv).rgb;
        if (max3(_stars_color) < 0.4) return vec3(0.0, 0.0, 0.0);
        _stars_color *= -lightDir.y;
        //_stars_color = pow(_stars_color, vec3(0.5));
        // Hiding stars behind the moon
        _stars_color *= 1.0 - _moon_amount;
        color += _stars_color;
    }

    return color;
}
