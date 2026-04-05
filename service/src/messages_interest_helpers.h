#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "protocol_types.h"
#include "messages_player.h"
#include "protocol_constants.h"
#include "packet_writer.h"
#include "packet_reader.h"

typedef struct InterestSetBuilder {
    Vec3f    bubble_center;
    float    bubble_radius;
    uint16_t cell_count;
    CellId   cells[F4MP_MAX_CELL_IDS];
} InterestSetBuilder;

void interest_builder_init(InterestSetBuilder* b, Vec3f center, float radius);
bool interest_builder_add_cell(InterestSetBuilder* b, CellId cell_id);
bool interest_builder_contains_cell(const InterestSetBuilder* b, CellId cell_id);
bool encode_msg_interest_set_with_cells(PacketWriter* w, const MsgInterestSet* m, const CellId* cells, uint16_t cell_count);
bool decode_msg_interest_set_with_cells(PacketReader* r, MsgInterestSet* m, CellId* cells_out, uint16_t max_cells, uint16_t* cells_read);
