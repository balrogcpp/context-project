// created by Andrey Vasiliev

#ifndef STARFIELD_GLSL
#define STARFIELD_GLSL

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

#endif // CLOUDS_GLSL
