/*
 * Copyright (c) 2018-2019, Pelion and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "common/bits.h"
#include "common/endian.h"
#include "common/log_legacy.h"
#include "common/ns_list.h"
#include "stack/mac/mac_common_defines.h"

#include "6lowpan/ws/ws_mpx_header.h"

// IEEE-802.15.9 Figure 10 Transaction Control field
#define MPX_IE_TRANSFER_TYPE_MASK  0b00000111
#define MPX_IE_TRANSACTION_ID_MASK 0b11111000

bool ws_llc_mpx_header_frame_parse(const uint8_t *ptr, uint16_t length, mpx_msg_t *msg)
{
    if (!length) {
        return false;
    }
    memset(msg, 0, sizeof(mpx_msg_t));
    bool fragmented_number_present = false;
    bool multiplex_id_present = false;
    bool fragment_total_size = false;

    msg->transfer_type  = FIELD_GET(MPX_IE_TRANSFER_TYPE_MASK,  *ptr);
    msg->transaction_id = FIELD_GET(MPX_IE_TRANSACTION_ID_MASK, *ptr++);
    length--;


    switch (msg->transfer_type) {
        case MPX_FT_FULL_FRAME:
            multiplex_id_present = true;
            break;
        case MPX_FT_FULL_FRAME_SMALL_MULTILEX_ID:
            break;
        case MPX_FT_FIRST_OR_SUB_FRAGMENT:
        case MPX_FT_LAST_FRAGMENT:
            fragmented_number_present = true;
            if (length < 2) {
                return false;
            }
            break;
        case MPX_FT_ABORT:
            if (length == 2) {
                fragment_total_size = true;
            } else if (length) {
                return false;
            }
            break;
        default:
            return false;
    }

    if (fragmented_number_present) {

        msg->fragment_number = *ptr++;
        length--;
        if (msg->fragment_number == 0) { //First fragment
            fragment_total_size = true;
            multiplex_id_present = true;
        }
    }

    if (fragment_total_size) {
        if (length < 2) {
            return false;
        }
        msg->total_upper_layer_size = read_le16(ptr);
        ptr += 2;
        length -= 2;
    }

    if (multiplex_id_present) {
        if (length < 3) {
            return false;
        }
        msg->multiplex_id = read_le16(ptr);
        ptr += 2;
        length -= 2;
    }

    msg->frame_ptr = ptr;
    msg->frame_length = length;
    return true;
}


uint8_t *ws_llc_mpx_header_write(uint8_t *ptr, const mpx_msg_t *msg)
{
    bool fragmented_number_present = false;
    bool multiplex_id_present = false;
    bool fragment_total_size = false;
    uint8_t tmp8;

    tmp8 = 0;
    tmp8 |= FIELD_PREP(MPX_IE_TRANSFER_TYPE_MASK,  msg->transfer_type);
    tmp8 |= FIELD_PREP(MPX_IE_TRANSACTION_ID_MASK, msg->transaction_id);
    *ptr++ = tmp8;

    switch (msg->transfer_type) {
        case MPX_FT_FULL_FRAME:
            multiplex_id_present = true;
            break;
        case MPX_FT_FULL_FRAME_SMALL_MULTILEX_ID:
            break;
        case MPX_FT_FIRST_OR_SUB_FRAGMENT:
        case MPX_FT_LAST_FRAGMENT:
            fragmented_number_present = true;
            if (msg->fragment_number == 0) {
                fragment_total_size = true;
                multiplex_id_present = true;
            }
            break;
        case MPX_FT_ABORT:
            if (msg->total_upper_layer_size) {
                fragment_total_size = true;
            }
            break;
        default:
            break;
    }

    if (fragmented_number_present) {
        *ptr++ = msg->fragment_number;
    }

    if (fragment_total_size) {
        ptr = write_le16(ptr, msg->total_upper_layer_size);
    }

    if (multiplex_id_present) {
        ptr = write_le16(ptr, msg->multiplex_id);
    }
    return ptr;
}
