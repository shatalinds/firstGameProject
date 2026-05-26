//
// Created by dave on 29.04.2026.
//

#pragma once

static const char* TRIANGLE_VERT = R"(
#version 300 es
layout(location = 0) in vec2 a_position;

void main() {
    gl_Position = vec4(a_position, 0.0, 1.0);
}
)";

static const char* TRIANGLE_FRAG = R"(
#version 300 es
precision mediump float;
out vec4 fragColor;

void main() {
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";


static float triangleVertices[] = {0.0f,  0.5f,  -0.5f, -0.5f,  0.5f, -0.5f};
static long sizeTriangleVertices = sizeof(triangleVertices);