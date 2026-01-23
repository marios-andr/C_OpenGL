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

uniform float time;
uniform vec2 resolution;

const int MAX_STEPS = 1500;

vec2 sphere_map(vec3 p) {
    return vec2(atan(p.x,p.y)/M_PI*0.5+0.5, asin(p.z)/M_PI+0.5);
}

// Tanner-Halland algorithm
vec3 get_blackbody_color(float Temp) {
    Temp /= 100.0;
    float r, g, b;

    // Red component
    if (Temp <= 66.0) r = 255.0;
    else {
        r = Temp - 60.0;
        r = 329.69 * pow(r, -0.133);
    }

    // Green component
    if (Temp <= 66.0) {
        g = 99.47 * log(Temp) - 161.12;
    } else {
        g = Temp - 60.0;
        g = 288.12 * pow(g, -0.075);
    }

    // Blue component
    if (Temp >= 66.0) b = 255.0;
    else if (Temp <= 19.0) b = 0.0;
    else {
        b = Temp - 10.0;
        b = 138.51 * log(b) - 305.04;
    }

    return clamp(vec3(r, g, b) / 255.0, 0.0, 1.0);
}

// Standard 2D Hash function
float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

// 2D Value Noise (Smoothly interpolates between hashes)
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    // Smoothstep interpolation (Hermite curve)
    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(mix(hash(i + vec2(0.0, 0.0)),
                   hash(i + vec2(1.0, 0.0)), u.x),
               mix(hash(i + vec2(0.0, 1.0)),
                   hash(i + vec2(1.0, 1.0)), u.x), u.y);
}

//Fractional Brownian Motion
float fbm(vec2 p) {
    float v = 0.0;
    float amp = 0.5;
    for (int i = 0; i < 3; i++) {
        v += noise(p) * amp;
        p *= 2.0;
        amp *= 0.5;
    }
    return v;
}

float ddu(float u) {
    return -u * (1.0 - 1.5 * u * u);
}

vec4 integrate(vec3 d0) {
    vec4 accDiskColor = vec4(0.0, 0.0, 0.0, 1.0);
    float accDiskOpacity = 0.0;
    float accDiskRIn = 1.5;
    float accDiskROut = 5;
    bool escaped = true;

    vec3 old_pos = cam_pos;
    vec3 ray_step;

    vec3 n = normalize(cam_pos);
    vec3 t = normalize(cross(cross(n, d0), n));

    float u  = 1.0 / length(cam_pos);
    float v = - u * dot(d0, n) / dot(d0, t);

    float u0 = u;
    float du0 = v;

    float phi = 0.0;
    float dphi = 0.01;

    for (int i = 0; i < MAX_STEPS; i++) { //Using leapfrog inetgration scheme
        // half step velocity
        float du_half = v + 0.5 * dphi * ddu(u);

        // predicted change in u
        float du = dphi * du_half;

        // --- adaptive step control ---
        float max_rel_u_change = (1.0 - log(max(u, 1e-6))) * 10.0 / float(MAX_STEPS);

        if ((du > 0.0 || (du0 < 0.0 && u0 / u < 5.0)) &&
        abs(du) > abs(max_rel_u_change * u))
        {
            dphi = max_rel_u_change * u / abs(du_half);
            du = dphi * du_half;
        }

        // full step position
        u   += du;
        phi += dphi;

        if (u <= 0.0) break;
        if (u > 1.0) { // fell into BH
            escaped = false;
            break;
        }

        vec3 pos = (cos(phi)*n + sin(phi)*t) / u;

        if (old_pos.y * pos.y < 0) { // crossed the plane
            float dist = distance(pos, vec3(0.0));

            if (dist > accDiskRIn && dist < accDiskROut) {
                float innerFade = smoothstep(accDiskRIn - 0.1, accDiskRIn + 0.2, dist);
                float outerFade = 1.0 - smoothstep(accDiskROut - 1.0, accDiskROut, dist);
                float opacity = clamp(1.5 / dist, 0.0, 1.0) * innerFade * outerFade;

                //Calculating Doppler Factor:
                vec3 diskDir = normalize(vec3(-pos.z, 0.0, pos.x));
                // Calculate local velocity (Keplerian)
                float v = sqrt(1.0 / dist);
                float cosTheta = dot(diskDir, normalize(ray_step));
                float doppler = sqrt(1.0 - v*v) / (1.0 - v * cosTheta);

                //Gravitational Redshift Factor
                float grav = sqrt(1.0 - 1.0 / dist);

                // Calculate temperature based on distance from bh.
                float temp = 2800 * pow(accDiskRIn / dist, 0.75);
                // Convert temperature to color. Artificially increase color intesity for better visuals
                vec3 color = get_blackbody_color(temp * doppler * grav) * 4.0;

                //Differential Rotation speed
                float rotationSpeed = pow(dist, -1.5) * 2.0;
                vec2 noiseUV = vec2(dist * 0.5, (phi + time * rotationSpeed) * (2.0 / M_PI));


                float beaming = pow(doppler * grav, 2.5); // Should be to the power of 4 but that doesn't look nice
                color *= (0.5 + 0.5 * fbm(3.0 * noiseUV)) * beaming;

                float weight = (1.0 - accDiskOpacity);
                accDiskColor.rgb += color * opacity * weight;
                accDiskOpacity += opacity * weight;
            };
        }

        if (accDiskOpacity > 0.99) break;

        ray_step = pos - old_pos;
        old_pos = pos;


        // finish velocity
        v = du_half + 0.5 * dphi * ddu(u);
    }

    vec2 uv = sphere_map(ROT_Y(radians(-45.0)) * normalize(ray_step));
    if (!escaped) return vec4(0.0, 0.0, 0.0, 1.0) * (1.0 - accDiskOpacity) + (accDiskColor);
    return texture(equirectangularMap, uv).rgba * (1.0 - accDiskOpacity) + (accDiskColor);
}

void main() {
    float fov_mult = 1.0/tan(radians(fov) * 0.5);

    vec2 p = -1.0 + 2.0 * gl_FragCoord.xy / resolution.xy;
    p.y *= resolution.y / resolution.x;

    vec3 pos = cam_pos;
    vec3 ray = normalize(p.x*cam_x + p.y*cam_y + fov_mult*cam_z);

    //vec2 uv = sphere_map(ray);
    //vec3 color = texture(equirectangularMap, uv).rgb;
    vec3 color = integrate(ray).rgb;
    FragColor = vec4(color, 1.0);
}