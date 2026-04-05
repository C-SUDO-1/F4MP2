#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct PacketReader {
    const uint8_t* buffer;
    size_t length;
    size_t offset;
    bool overflowed;
} PacketReader;

void pr_init(PacketReader* r, const uint8_t* buffer, size_t length);
bool pr_read_u8(PacketReader* r, uint8_t* out);
bool pr_read_u16(PacketReader* r, uint16_t* out);
bool pr_read_u32(PacketReader* r, uint32_t* out);
bool pr_read_i8(PacketReader* r, int8_t* out);
bool pr_read_i16(PacketReader* r, int16_t* out);
bool pr_read_i32(PacketReader* r, int32_t* out);
bool pr_read_f32(PacketReader* r, float* out);
bool pr_read_bool(PacketReader* r, bool* out);
bool pr_read_bytes(PacketReader* r, void* dst, size_t len);
bool pr_read_string_u16(PacketReader* r, char* dst, size_t dst_capacity);
size_t pr_bytes_remaining(const PacketReader* r);
bool pr_ok(const PacketReader* r);

