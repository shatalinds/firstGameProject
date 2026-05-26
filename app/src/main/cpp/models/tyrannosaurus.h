//
// Created by dave on 26.05.2026.
//
#pragma once

static const char* TYRANNOSAURUS_VERT = R"(
#version 300 es
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 a_Tangent;

out vec2 TexCoords;
out vec3 vNormal;
out vec3 vTangent;
out vec3 vBitangent;
out vec3 vFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 worldPos = model * vec4(a_Position, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vNormal = normalize(normalMatrix * a_Normal);
    vTangent = normalize(normalMatrix * a_Tangent.xyz);
    vBitangent = normalize(cross(vNormal, vTangent) * a_Tangent.w);
    vFragPos = vec3(view * worldPos);
    gl_Position = projection * view * worldPos;
    TexCoords = vec2(aTexCoords.x, 1.0 - aTexCoords.y);
}
)";

static const char* TYRANNOSAURUS_FRAG = R"(
#version 300 es
precision mediump float;

out vec4 FragColor;
in vec2 TexCoords;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBitangent;
in vec3 vFragPos;

uniform sampler2D diffuseTex;
uniform sampler2D specGlossTex;
uniform sampler2D normalTex;
uniform sampler2D occlusionTex;

uniform int hasDiffuse;
uniform int hasNormal;
uniform int hasSpecGloss;
uniform int hasOcclusion;

uniform vec3 lightDir;
uniform vec3 ambientColor;
uniform vec3 diffuseColor;

vec3 sampleNormal() {
    if (hasNormal == 0) {
        return normalize(vNormal);
    }
    vec3 mapNormal = texture(normalTex, TexCoords).rgb * 2.0 - 1.0;
    mat3 tbn = mat3(normalize(vTangent), normalize(vBitangent), normalize(vNormal));
    return normalize(tbn * mapNormal);
}

void main() {
    vec3 albedo = hasDiffuse != 0
        ? texture(diffuseTex, TexCoords).rgb
        : vec3(0.75, 0.72, 0.68);

    float occlusion = hasOcclusion != 0 ? texture(occlusionTex, TexCoords).r : 1.0;
    vec4 specGloss = hasSpecGloss != 0
        ? texture(specGlossTex, TexCoords)
        : vec4(0.15, 0.15, 0.15, 0.35);

    vec3 norm = sampleNormal();
    vec3 light = normalize(lightDir);
    vec3 viewDir = normalize(-vFragPos);

    float ndotl = max(dot(norm, light), 0.0);
    vec3 ambient = ambientColor * albedo * occlusion;
    vec3 diffuse = diffuseColor * ndotl * albedo;

    vec3 halfDir = normalize(light + viewDir);
    float shininess = mix(8.0, 64.0, specGloss.a);
    float specTerm = pow(max(dot(norm, halfDir), 0.0), shininess);
    vec3 specular = specGloss.rgb * specTerm * specGloss.a;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
)";