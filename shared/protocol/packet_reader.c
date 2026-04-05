#include "packet_reader.h"
#include <string.h>

static bool pr_reserve(PacketReader* r, size_t len) {
    if (r->overflowed || r->offset + len > r->length) {
        r->overflowed = true;
        return false;
    }
    return true;
}

void pr_init(PacketReader* r, const uint8_t* buffer, size_t length) {
    r->buffer = buffer;
    r->length = length;
    r->offset = 0;
    r->overflowed = false;
}

bool pr_read_bytes(PacketReader* r, void* dst, size_t len) {
    if (!pr_reserve(r, len)) return false;
    memcpy(dst, r->buffer + r->offset, len);
    r->offset += len;
    return true;
}

bool pr_read_u8(PacketReader* r, uint8_t* out) { return pr_read_bytes(r, out, sizeof(*out)); }
bool pr_read_u16(PacketReader* r, uint16_t* out) { return pr_read_bytes(r, out, sizeof(*out)); }
bool pr_read_u32(PacketReader* r, uint32_t* out) { return pr_read_bytes(r, out, sizeof(*out)); }
bool pr_read_i8(PacketReader* r, int8_t* out) { return pr_read_bytes(r, out, sizeof(*out)); }
bool pr_read_i16(PacketReader* r, int16_t* out) { return pr_read_bytes(r, out, sizeof(*out)); }
bool pr_read_i32(PacketReader* r, int32_t* out) { return pr_read_bytes(r, out, sizeof(*out)); }
bool pr_read_f32(PacketReader* r, float* out) { return pr_read_bytes(r, out, sizeof(*out)); }

bool pr_read_bool(PacketReader* r, bool* out) {
    uint8_t v = 0;
    if (!pr_read_u8(r, &v)) return false;
    *out = (v != 0);
    return true;
}

bool pr_read_string_u16(PacketReader* r, char* dst, size_t dst_capacity) {
    uint16_t len = 0;
    if (!pr_read_u16(r, &len)) return false;
    if (dst_capacity == 0 || len + 1 > dst_capacity) {
        r->overflowed = true;
        return false;
    }
    if (!pr_read_bytes(r, dst, len)) return false;
    dst[len] = '\0';
    return true;
}

size_t pr_bytes_remaining(const PacketReader* r) { return (r->offset <= r->length) ? (r->length - r->offset) : 0; }
bool pr_ok(const PacketReader* r) { return !r->overflowed; }
