#include "packet_writer.h"
#include <string.h>

static bool pw_reserve(PacketWriter* w, size_t len) {
    if (w->overflowed || w->offset + len > w->capacity) {
        w->overflowed = true;
        return false;
    }
    return true;
}

void pw_init(PacketWriter* w, uint8_t* buffer, size_t capacity) {
    w->buffer = buffer;
    w->capacity = capacity;
    w->offset = 0;
    w->overflowed = false;
}

bool pw_write_bytes(PacketWriter* w, const void* src, size_t len) {
    if (!pw_reserve(w, len)) return false;
    memcpy(w->buffer + w->offset, src, len);
    w->offset += len;
    return true;
}

bool pw_write_u8(PacketWriter* w, uint8_t value) { return pw_write_bytes(w, &value, sizeof(value)); }
bool pw_write_u16(PacketWriter* w, uint16_t value) { return pw_write_bytes(w, &value, sizeof(value)); }
bool pw_write_u32(PacketWriter* w, uint32_t value) { return pw_write_bytes(w, &value, sizeof(value)); }
bool pw_write_i8(PacketWriter* w, int8_t value) { return pw_write_bytes(w, &value, sizeof(value)); }
bool pw_write_i16(PacketWriter* w, int16_t value) { return pw_write_bytes(w, &value, sizeof(value)); }
bool pw_write_i32(PacketWriter* w, int32_t value) { return pw_write_bytes(w, &value, sizeof(value)); }
bool pw_write_f32(PacketWriter* w, float value) { return pw_write_bytes(w, &value, sizeof(value)); }
bool pw_write_bool(PacketWriter* w, bool value) { uint8_t v = value ? 1 : 0; return pw_write_u8(w, v); }

bool pw_write_string_u16(PacketWriter* w, const char* utf8) {
    size_t len = 0;
    while (utf8[len] != '\0') len++;
    if (len > 65535) { w->overflowed = true; return false; }
    return pw_write_u16(w, (uint16_t)len) && pw_write_bytes(w, utf8, len);
}

size_t pw_bytes_written(const PacketWriter* w) { return w->offset; }
bool pw_ok(const PacketWriter* w) { return !w->overflowed; }
