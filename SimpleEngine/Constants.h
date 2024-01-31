#pragma once
constexpr double PHYSICS_TIMESTEP = 1.0f / 120.0f;
constexpr int MAX_LIGHTS = 32;

constexpr int T0_DIFFUSETEXTURE = 0;
constexpr int T1_SPECULARTEXTURE = 1;
constexpr int T2_SKYBOXTEXTURE = 2;

#define FILL_STATE_KEY "RSFillState"
#define REVERSE_FILL_STATE_KEY "RSReverseFillState"
#define WIREFRAME_STATE_KEY "RSWireframeState"
#define CULLED_WIREFRAME_STATE_KEY "RSCulledWireframeState"

#define BILINEAR_SAMPLER_KEY "SBilinear"

#define SKYBOX_DEPTH_STENCIL_KEY "DSSkybox"