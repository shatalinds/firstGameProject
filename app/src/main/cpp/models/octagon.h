//
// Created by dave on 30.04.2026.
//

#pragma once

static const char* OCTAGON_VERT = R"(
#version 300 es
layout(location = 0) in vec2 a_position;

void main() {
    gl_Position = vec4(a_position, 0.0, 1.0);
}
)";

static const char* OCTAGON_FRAG = R"(
#version 300 es
precision mediump float;
out vec4 fragColor;

void main() {
    fragColor = vec4(0.0, 0.5, 1.0, 1.0);
}
)";

// Октагон через triangle fan: центр + 8 вершин + замыкание
static float octagonVertices[] = {
        0.0f,  0.0f,   // центр
        0.0f,  0.5f,   // верх
        0.354f,  0.354f,
        0.5f,  0.0f,
        0.354f, -0.354f,
        0.0f, -0.5f,
        -0.354f, -0.354f,
        -0.5f,  0.0f,
        -0.354f,  0.354f,
        0.0f,  0.5f,   // замыкание
};

static long sizeOctagonVertices = sizeof(octagonVertices);