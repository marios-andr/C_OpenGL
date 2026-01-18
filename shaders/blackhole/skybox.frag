#version 460 core
#define M_PI 3.141592653589793238462643383279

out vec4 FragColor;

in vec3 TexCoords;

uniform sampler2D equirectangularMap;
uniform vec3 camPos;

uniform mat4 invProjection;
uniform mat4 invView;
uniform vec2 resolution;

//uniform float time;

const int MAX_STEPS = 100;

vec2 sphere_map(vec3 p) {
    return vec2(atan(p.x,p.y)/M_PI*0.5+0.5, asin(p.z)/M_PI+0.5);
}

vec3 integrate(vec3 d0) {
    vec3 color;

    vec3 n = normalize(camPos);
    vec3 t = normalize(cross(cross(n, d0), n));

    float u  = 1.0 / length(camPos);        // = 1 / |p0|
    float v  = - u * dot(d0, n) / dot(d0, t);        // = u'(0)
    float u_start = u;
    float v_start = v;
    float phi = 0.0;
    float dphi = 0.01;

    for (int i = 0; i < MAX_STEPS; i++) {

        float a = -u * (1.0 - 1.5 * u * u);

        // half step velocity
        float v_half = v + 0.5 * dphi * a;

        // predicted change in u
        float du = dphi * v_half;

        // --- adaptive step control ---
        float max_rel_u_change = (1.0 - log(max(u, 1e-6))) * 10.0 / float(MAX_STEPS);

        if ((du > 0.0 || (v_start < 0.0 && u_start / u < 5.0)) &&
        abs(du) > abs(max_rel_u_change * u))
        {
            dphi = max_rel_u_change * u / abs(v_half);
            du = dphi * v_half;
        }

        // full step position
        u += dphi * v_half;
        phi += dphi;

        // new acceleration
        float a_new = -u * (1.0 - 1.5 * u * u);

        // finish velocity
        v = v_half + 0.5 * dphi * a_new;

        float u_next = u + dphi * v_half;
        if (u_next <= 0.0) break;
        if (u > 1.0) return vec3(0.0); // fell into BH
    }

    vec3 pos = normalize(n * cos(phi) + t * sin(phi));
    vec2 uv = sphere_map(pos);
    return texture(equirectangularMap, uv).rgb;
}

void main() {
    vec3 envVector = normalize(TexCoords);

    //vec2 uv = sphere_map(envVector);
    //vec3 color = texture(equirectangularMap, uv).rgb;
    vec3 color = integrate(envVector);
    FragColor = vec4(color, 1.0);
}