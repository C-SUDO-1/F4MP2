
#include <assert.h>
#include <string.h>
#include "packet_codec.h"
int main(void){
    uint8_t buf[256]; PacketWriter w; PacketReader r; MsgPlayerState a,b;
    memset(&a,0,sizeof(a)); a.player_id=2; a.position.x=1.0f; a.rotation.yaw=90.0f; a.state_sequence=42; a.observed_tick=99; a.equipped_form_id=1234;
    pw_init(&w,buf,sizeof(buf)); assert(encode_msg_player_state(&w,&a));
    pr_init(&r,buf,pw_bytes_written(&w)); memset(&b,0,sizeof(b)); assert(decode_msg_player_state(&r,&b));
    assert(b.player_id==2); assert(b.state_sequence==42); assert(b.observed_tick==99); assert(b.equipped_form_id==1234);
    return 0;
}
