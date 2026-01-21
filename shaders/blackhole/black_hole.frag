#version 460 core
#define M_PI 3.141592653589793238462643383279

#define ROT_Y(a) mat3(0, cos(a), sin(a), 1, 0, 0, 0, sin(a), -cos(a))

out vec4 FragColor;

uniform sampler2D equirectangularMap;
uniform vec3 cam_pos;
uniform vec3 cam_x;
uniform vec3 cam_y;
uniform vec3 cam_z;
uniform float fov;

uniform vec2 resolution;

//uniform float time;

const int MAX_STEPS = 200;

vec2 sphere_map(vec3 p) {
    return vec2(atan(p.x,p.y)/M_PI*0.5+0.5, asin(p.z)/M_PI+0.5);
}

vec3 integrate(vec3 d0) {
    vec3 color;

    vec3 old_pos = cam_pos;
    vec3 ray_step;

    vec3 n = normalize(cam_pos);
    vec3 t = normalize(cross(cross(n, d0), n));

    float u  = 1.0 / length(cam_pos);        // = 1 / |p0|
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

        vec3 pos = (cos(phi)*n + sin(phi)*t) / u;
        ray_step = pos - old_pos;
        old_pos = pos;

        // new acceleration
        float a_new = -u * (1.0 - 1.5 * u * u);

        // finish velocity
        v = v_half + 0.5 * dphi * a_new;

        float u_next = u + dphi * v_half;
        if (u_next <= 0.0) break;
        if (u > 1.0) return vec3(0.0); // fell into BH
    }

    //vec3 pos = (n * cos(phi) + t * sin(phi)) / u;
    vec2 uv = sphere_map(ROT_Y(radians(-45.0)) * normalize(ray_step));
    return texture(equirectangularMap, uv).rgb;
}

void main() {
    float fov_mult = 1.0/tan(radians(fov) * 0.5);

    vec2 p = -1.0 + 2.0 * gl_FragCoord.xy / resolution.xy;
    p.y *= resolution.y / resolution.x;

    vec3 pos = cam_pos;
    vec3 ray = normalize(p.x*cam_x + p.y*cam_y + fov_mult*cam_z);

    //vec2 uv = sphere_map(ray);
    //vec3 color = texture(equirectangularMap, uv).rgb;
    vec3 color = integrate(ray);
    FragColor = vec4(color, 1.0);
}