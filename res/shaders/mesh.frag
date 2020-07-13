//-----------------------------------------------------------------------------
// Mesh rendering shader
//
// Copyright 2016 Aleksey Egorov
//-----------------------------------------------------------------------------
#define PI 3.14159265359

uniform vec3 lightDir0;
uniform vec3 lightDir1;
uniform float lightInt0;
uniform float lightInt1;
uniform float ambient;

varying vec3 fragNormal;
varying vec4 fragColor;

// V = direction To the viewer (unit vector)
// N = surface normal (unit vector)
// L = direction TO the light source (unit vector)
// brightness = the brightness of the light source
// color = the RGB surface color

float geometry(float VdotN, float k) {
    float den = VdotN - k*VdotN + k;
    float ret = VdotN / den;
    return ret;
}

vec3 FromLight(vec3 V, vec3 N, vec3 L, float brightness, vec3 color ) {
    float rough = 0.4;
    float specular = 1.0;
    // the halfway vector
    vec3 H = normalize(V+L);
    // reflected light vector
//    vec3 R = N*dot(N,L)*2.0 - L;
    float cosTheta = clamp(dot(V, H), 0.0, 1.0);
    // Schlick approximation to The Fresnel term
    float F0 = 0.04; // reflectivity at normal incidence
    float fresnel = F0 + ( (1.0 - F0) * pow(1.0 - cosTheta, 5.0) );

    float vi = clamp(dot(V, N), 0.0, 1.0);
    float vl = clamp(dot(L, N), 0.0, 1.0);
    float G = geometry(vi, rough) * geometry(vl, rough);
    G = clamp(G,0.0,1.0);

    float diffuse = clamp(dot(L, N), 0.0, 1.0) * brightness;

    vec3 ret = color * diffuse;
    ret += vec3(fresnel) * specular * brightness;
//    ret += vec3(G/(4.0*vi*vl));   // this term is wrong
    return ret; 
}

void main() {
    vec3 result = fragColor.xyz * ambient;
    vec3 normal = normalize(fragNormal);
    vec3 view = vec3(0.0, 0.0, 1.0);
    view = normalize(view);
    vec3 LD0 = normalize(lightDir0);
    vec3 LD1 = normalize(lightDir1);

    result += FromLight(view, normal, LD0, lightInt0, fragColor.rgb);
    result += FromLight(view, normal, LD1, lightInt1, fragColor.rgb);

    gl_FragColor = vec4(result, fragColor.a);
}

