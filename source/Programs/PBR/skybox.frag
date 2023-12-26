// created by Andrey Vasiliev

#define HAS_MRT
#include "header.glsl"
#include "srgb.glsl"
#include "fog.glsl"

uniform vec3 LightColor;
uniform vec3 LightDirection;
uniform float SunSize;
uniform vec4 FogColour;
uniform vec4 FogParams;
uniform vec3 Params[10];

uniform vec4 ViewportSize;
uniform float Time;

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
    return Z * ((1.0 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E * gamma) + F * cos_gamma2 + G * chi + H * sqrt(cos_theta)));
}

// Clamps color between 0 and 1 smoothly
vec3 SkyLightExpose(const vec3 color)
{
    return 1.0 - exp(-0.05 * color);
}
//vec3 SkyLightExpose(const vec3 color, float exposure)
//{
//    return 2.0 / (1.0 + exp(-exposure * color)) - 1.0;
//}

// cloudy skies shader
// inspired from shadertoy shader made by Drift (https://www.shadertoy.com/view/4tdSWr)

//shader_type canvas_item;

//const float cloudscale = 1.1;
//const float speed = 0.01;
//const float clouddark = 0.5;
//const float cloudlight = 0.3;
//const float cloudcover = 0.2;
//const float cloudalpha = 8.0;
//const float skytint = 0.5;
//const vec3 skycolour1 = vec3(0.2, 0.4, 0.6);
//const vec3 skycolour2 = vec3(0.4, 0.7, 1.0);
//const mat2 m = mat2(vec2(1.6,1.2),vec2(-1.2,1.6)); // changement
//
//// functions
//
//vec2 hash(vec2 p) {
//    p = vec2(dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)));
//    return -1.0 + 2.0*fract(sin(p)*43758.5453123);
//}
//
//float noise(const vec2 p) {
//    float K1 = 0.366025404; // (sqrt(3)-1)/2;
//    float K2 = 0.211324865; // (3-sqrt(3))/6;
//    vec2 i = floor(p + (p.x+p.y)*K1);
//    vec2 a = p - i + (i.x+i.y)*K2;
//    vec2 o = (a.x>a.y) ? vec2(1.0,0.0) : vec2(0.0,1.0); //vec2 of = 0.5 + 0.5*vec2(sign(a.x-a.y), sign(a.y-a.x));
//    vec2 b = a - o + K2;
//    vec2 c = a - 1.0 + 2.0*K2;
//    vec3 h = max(0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
//    vec3 n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
//    return dot(n, vec3(70.0));
//}
//
//float fbm(vec2 n) {
//    float total = 0.0, amplitude = 0.1;
//    for (int i = 0; i < 7; i++) {
//        total += noise(n) * amplitude;
//        n = m * n;
//        amplitude *= 0.4;
//    }
//    return total;
//}
//
//// fragment shader
//
//vec3 fragment() {
//    //vec2 res = 1.0 / SCREEN_PIXEL_SIZE;
//    vec2 res = ViewportSize.xy;
//    //vec2 p = FRAGCOORD.xy / res.xy; // changement
//    vec2 p = gl_FragCoord.xy / res.xy; // changement
////    vec2 p = vUV0.xz; // changement
//    vec2 uv = p*vec2(res.x/res.y,1.0);    // changement
//    float Time = Time * speed ;
//    float q = fbm(uv * cloudscale * 0.5);
//
//    //ridged noise shape
//    float r = 0.0;
//    uv *= cloudscale;
//    uv += q - Time;
//    float weight = 0.8;
//    for (int i=0; i<8; i++){
//        r += abs(weight*noise( uv ));
//        uv = m*uv + Time;
//        weight *= 0.7;
//    }
//
//    //noise shape
//    float f = 0.0;
//    uv = p*vec2(res.x/res.y,1.0); // changement
//    uv *= cloudscale;
//    uv += q - Time;
//    weight = 0.7;
//    for (int i=0; i<8; i++){
//        f += weight*noise( uv );
//        uv = m*uv + Time;
//        weight *= 0.6;
//    }
//
//    f *= r + f;
//
//    //noise colour
//    float c = 0.0;
//    Time = Time * speed * 2.0;
//    uv = p*vec2(res.x/res.y,1.0); // changement
//    uv *= cloudscale*2.0;
//    uv += q - Time;
//    weight = 0.4;
//    for (int i=0; i<7; i++){
//        c += weight*noise( uv );
//        uv = m*uv + Time;
//        weight *= 0.6;
//    }
//
//    //noise ridge colour
//    float c1 = 0.0;
//    Time = Time * speed * 3.0;
//    uv = p*vec2(res.x/res.y,1.0);
//    uv *= cloudscale*3.0;
//    uv += q - Time;
//    weight = 0.4;
//    for (int i=0; i<7; i++){
//        c1 += abs(weight*noise( uv ));
//        uv = m*uv + Time;
//        weight *= 0.6;
//    }
//
//    c += c1;
//
//    vec3 skycolour = mix(skycolour2, skycolour1, p.y);
//    vec3 cloudcolour = vec3(1.1, 1.1, 0.9) * clamp((clouddark + cloudlight*c), 0.0, 1.0);
//
//    f = cloudcover + cloudalpha*f*r;
//
//    vec3 result = mix(skycolour, clamp(skytint * skycolour + cloudcolour, 0.0, 1.0), clamp(f + c, 0.0, 1.0));
//
//    return result;
//}

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
const vec3 sunset_bottom_color = vec3( 1.0, 0.5, 0.7 );

////////////////////////////////////////////////////////////////////////////////////////////////////
// Function for clouds noises. You can replace using "gen_fractal_ping_pong" with a simple texture reading.
// I was frustrated with the repeating texture that's why I included the algorithm in the code.
// Source: https://github.com/Auburn/FastNoiseLite/tree/master
const int PRIME_X = 501125321;
const int PRIME_Y = 1136930381;
float lerp_( float a, float b, float t )
{
    return a + t * ( b - a );
}
float cubic_lerp( float a, float b, float c, float d, float t )
{
    float p = d - c - ( a - b );
    return t * t * t * p + t * t * ( a - b - p ) + t * ( c - a ) + b;
}
float ping_pong( float t )
{
    t -= trunc( t * 0.5 ) * 2.0;
    return t < 1.0 ? t : 2.0 - t;
}
int hash( int seed, int x_primed, int y_primed )
{
    return ( seed ^ x_primed ^ y_primed ) * 0x27d4eb2d;
}
float val_coord( int seed, int x_primed, int y_primed )
{
    int hash = hash( seed, x_primed, y_primed );
    hash *= hash;
    hash ^= hash << 19;
    return float( hash ) * ( 1.0 / 2147483648.0 );
}
float single_value_cubic( int seed, float x, float y )
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
float gen_fractal_ping_pong( vec2 pos, int seed, float frequency )
{
    float x = pos.x * frequency;
    float y = pos.y * frequency;
    float sum = 0.0;
    float amp = FRACTAL_BOUNDING;
    for( int i = 0; i < OCTAVES; i++ )
    {
        float noise = ping_pong(( single_value_cubic( seed++, x, y ) + 1.0 ) * PING_PONG_STRENGTH );
        sum += ( noise - 0.5 ) * 2.0 * amp;
        amp *= lerp_( 1.0, noise, WEIGHTED_STRENGTH );
        x *= LACUNARITY;
        y *= LACUNARITY;
        amp *= GAIN;
    }
    return sum * 0.5 + 0.5;
}
////////////////////////////////////////////////////////////////////////////////////////////////////

vec3 clouds(const vec3 uv, const vec3 _sky_color) {
    // Clouds UV movement direction
    vec2 _sky_uv = uv.xz / sqrt( uv.y );
    vec3 lightDir = -LightDirection;
    float _sunset_amount = clamp( 0.5 - abs( lightDir.y ), 0.0, 0.5 ) * 2.0;
    float _sun_distance = distance( uv, lightDir.xyz );
    float _night_amount = clamp( -lightDir.y + 0.7, 0.0, 1.0 );

    float _clouds_speed = Time * clouds_speed * 0.01;
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
    _clouds_color = mix( _clouds_color, clamp( LightColor, 0.0, 1.0 ), pow( 1.0 - clamp( _sun_distance, 0.0, 1.0 ), 5 ));
    // Color combined with sunset condition
    _clouds_color = mix( _clouds_color, sunset_bottom_color, _sunset_amount * 0.75 );
    // Color depending on the "progress" of the night.
    _clouds_color = mix( _clouds_color, _sky_color, clamp( _night_amount, 0.0, 0.98 ));
    _clouds_color = mix( _clouds_color, vec3( 0.0 ), clouds_weight * 0.9 );
    return mix( _sky_color, _clouds_color, _clouds_amount );
}

in highp vec3 vUV0;
void main()
{
    highp vec3 V = normalize(vUV0);
    vec3 N = normalize(-LightDirection.xyz);
    float sunZenith = V.y;
    V.y = abs(V.y);
    if (abs(V.y) < 0.001) V.y = 0.001;
    float cos_theta = clamp(V.y, 0.0, 1.0);
    float cos_gamma = dot(V, N);
    float gamma = acos(cos_gamma);
    vec3 color = HosekWilkie(gamma, cos_gamma, cos_theta, Params);

    if (gamma <= M_PI / 360.0 && sunZenith > 0.0) {
        color = mix(color, LightColor * 100.0, 0.1);
    }

    // Apply exposure.
    color = SkyLightExpose(color);

    //    color = fragment();
    color = clouds(vUV0, color);
    color = SRGBtoLINEAR(color);

    // Fog
    color = ApplyFog(color, FogParams.x, FogColour.rgb, 200.0 * pow8(1.0 - sign(V.y) * V.y), V, LightDirection.xyz, vec3(0.0, 0.0, 0.0));

    EvaluateBuffer(color);
}
