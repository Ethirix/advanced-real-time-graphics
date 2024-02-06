#pragma once

constexpr double PHYSICS_TIMESTEP = 1.0f / 120.0f;
constexpr float AIR_DENSITY = 1.204f;

#define GRAVITY Vector3(0, -9.81, 0)

constexpr int MAX_LIGHTS = 32;

#define FILL_STATE_KEY "RSFillState"
#define REVERSE_FILL_STATE_KEY "RSReverseFillState"
#define WIREFRAME_STATE_KEY "RSWireframeState"
#define CULLED_WIREFRAME_STATE_KEY "RSCulledWireframeState"

#define BILINEAR_SAMPLER_KEY "SBilinear"

#define SKYBOX_DEPTH_STENCIL_KEY "DSSkybox"

#define PI 3.14159265358979323846