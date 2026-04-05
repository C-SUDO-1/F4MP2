#include <assert.h>
#include "packet_codec.h"

int main(void) {
    uint8_t buf[64];
    PacketWriter w;
    PacketReader r;
    MsgPing in_ping = {1234}, out_ping = {0};
    MsgPong in_pong = {5678}, out_pong = {0};

    pw_init(&w, buf, sizeof(buf));
    assert(encode_msg_ping(&w, &in_ping));
    pr_init(&r, buf, pw_bytes_written(&w));
    assert(decode_msg_ping(&r, &out_ping));
    assert(out_ping.time_ms == 1234);

    pw_init(&w, buf, sizeof(buf));
    assert(encode_msg_pong(&w, &in_pong));
    pr_init(&r, buf, pw_bytes_written(&w));
    assert(decode_msg_pong(&r, &out_pong));
    assert(out_pong.time_ms == 5678);
    return 0;
}
