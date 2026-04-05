#include <assert.h>
#include <string.h>
#include "packet_codec.h"
#include "packet_writer.h"
#include "packet_reader.h"

int main(void) {
    uint8_t buf[64];
    PacketWriter w;
    PacketReader r;
    MsgPlayerLeft in_msg, out_msg;
    memset(&in_msg, 0, sizeof(in_msg));
    memset(&out_msg, 0, sizeof(out_msg));
    in_msg.player_id = 7;
    pw_init(&w, buf, sizeof(buf));
    assert(encode_msg_player_left(&w, &in_msg));
    pr_init(&r, buf, pw_bytes_written(&w));
    assert(decode_msg_player_left(&r, &out_msg));
    assert(out_msg.player_id == 7);
    return 0;
}
