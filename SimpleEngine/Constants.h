#pragma once

constexpr int CB_00 = 0b0000'0000'0000'0001; //CBObjectCameraData
constexpr int CB_01 = 0b0000'0000'0000'0010; //CBMaterials
constexpr int CB_02 = 0b0000'0000'0000'0100; //CBTextures
constexpr int CB_03 = 0b0000'0000'0000'1000; //CBLighting
constexpr int CB_04 = 0b0000'0000'0001'0000; //UNUSED
constexpr int CB_05 = 0b0000'0000'0010'0000; //UNUSED
constexpr int CB_06 = 0b0000'0000'0100'0000; //UNUSED
constexpr int CB_07 = 0b0000'0000'1000'0000; //UNUSED
constexpr int CB_08 = 0b0000'0001'0000'0000; //UNUSED
constexpr int CB_09 = 0b0000'0010'0000'0000; //UNUSED
constexpr int CB_10 = 0b0000'0100'0000'0000; //UNUSED
constexpr int CB_11 = 0b0000'1000'0000'0000; //UNUSED
constexpr int CB_12 = 0b0001'0000'0000'0000; //UNUSED
constexpr int CB_13 = 0b0010'0000'0000'0000; //UNUSED
constexpr int CB_14 = 0b0100'0000'0000'0000; //UNUSED

constexpr int MAX_BUFFERS = 15; //This is the Resource Limit for Constant Buffers

#define FILL_STATE_KEY "RSFillState"
#define REVERSE_FILL_STATE_KEY "RSReverseFillState"
#define WIREFRAME_STATE_KEY "RSWireframeState"
#define CULLED_WIREFRAME_STATE_KEY "RSCulledWireframeState"

#define BILINEAR_SAMPLER_KEY "SBilinear"

#define SKYBOX_DEPTH_STENCIL_KEY "DSSkybox"