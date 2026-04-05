#pragma once

#include <stdint.h>

#pragma pack(push, 1)
typedef struct Vec3f { float x, y, z; } Vec3f;
typedef struct Rot3f { float pitch, yaw, roll; } Rot3f;
#pragma pack(pop)

typedef uint32_t SessionId;
typedef uint16_t PlayerId;
typedef uint32_t NetId;
typedef uint32_t ActorNetId;
typedef uint32_t ObjectNetId;
typedef uint32_t RequestId;
typedef uint32_t CellId;
typedef uint32_t DialogueId;
typedef uint32_t QuestId;
typedef uint32_t ObjectiveId;

