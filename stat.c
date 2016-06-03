/*
 * Copyright (c) 2016, Moonflow <me@zhc105.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include "stat.h"
#include "util.h"
#include "config.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static int stat_num = 0;
litedt_stat_t stat_total;
litedt_stat_t stat_max;
litedt_stat_t stat_min;

void inc_stat(litedt_stat_t *stat)
{
    unsigned int offset = 0;
    while (offset < sizeof(litedt_stat_t)) {
        uint32_t *elem = (uint32_t *)((char *)stat + offset);
        uint32_t *total = (uint32_t *)((char *)&stat_total + offset);
        uint32_t *min = (uint32_t *)((char *)&stat_min + offset);
        uint32_t *max = (uint32_t *)((char *)&stat_max + offset);
        *total += *elem;
        *min = MIN(*min, *elem);
        *max = MAX(*max, *elem);
        offset += sizeof(uint32_t);
    }
    ++stat_num;
}

void clear_stat()
{
    stat_num = 0;
    memset(&stat_total, 0, sizeof(litedt_stat_t));
    memset(&stat_max, 0, sizeof(litedt_stat_t));
    memset(&stat_min, 0xFF, sizeof(litedt_stat_t));
}

void print_stat()
{
    if (!stat_num)
        ++stat_num;
    LOG("\n------------------------------------------------------------------\n"
        "|Network Statistics\n"
        "------------------------------------------------------------------\n"
        "|%-12s|%-12s|%-12s|%-12s|%-12s|\n"
        "------------------------------------------------------------------\n"
        "|%-12s|%-12u|%-12u|%-12u|%-12u|\n"
        "|%-12s|%-12u|%-12u|%-12u|%-12u|\n"
        "|%-12s|%-12u|%-12u|%-12u|%-12u|\n"
        "|%-12s|%-12u|%-12u|%-12u|%-12u|\n"
        "|%-12s|%-12s|%-12u|%-12u|%-12u|\n"
        "|%-12s|%-12s|%-12u|%-12u|%-12u|\n"
        "------------------------------------------------------------------\n",
        "Name", "Total", "Avg", "Max", "Min",
        "Flow Out", stat_total.send_bytes_stat, stat_total.send_bytes_stat / stat_num, stat_max.send_bytes_stat, stat_min.send_bytes_stat,
        "Flow In", stat_total.recv_bytes_stat, stat_total.recv_bytes_stat / stat_num, stat_max.recv_bytes_stat, stat_min.recv_bytes_stat,
        "Packet Send", stat_total.data_packet_post, stat_total.data_packet_post / stat_num, stat_max.data_packet_post, stat_min.data_packet_post,
        "Retrans", stat_total.retrans_packet_post, stat_total.retrans_packet_post / stat_num, stat_max.retrans_packet_post, stat_min.retrans_packet_post,
        "Connections", "-", stat_total.connection_num / stat_num, stat_max.connection_num, stat_min.connection_num,
        "RTT", "-", stat_total.rtt / stat_num, stat_max.rtt, stat_min.rtt);
}
