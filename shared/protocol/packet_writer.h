#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct PacketWriter {
    uint8_t* buffer;
    size_t capacity;
    size_t offset;
    bool overflowed;
} PacketWriter;

void pw_init(PacketWriter* w, uint8_t* buffer, size_t capacity);
bool pw_write_u8(PacketWriter* w, uint8_t value);
bool pw_write_u16(PacketWriter* w, uint16_t value);
bool pw_write_u32(PacketWriter* w, uint32_t value);
bool pw_write_i8(PacketWriter* w, int8_t value);
bool pw_write_i16(PacketWriter* w, int16_t value);
bool pw_write_i32(PacketWriter* w, int32_t value);
bool pw_write_f32(PacketWriter* w, float value);
bool pw_write_bool(PacketWriter* w, bool value);
bool pw_write_bytes(PacketWriter* w, const void* src, size_t len);
bool pw_write_string_u16(PacketWriter* w, const char* utf8);
size_t pw_bytes_written(const PacketWriter* w);
bool pw_ok(const PacketWriter* w);

