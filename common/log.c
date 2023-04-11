/*
 * Copyright (c) 2021-2022 Silicon Laboratories Inc. (www.silabs.com)
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of the Silicon Labs Master Software License
 * Agreement (MSLA) available at [1].  This software is distributed to you in
 * Object Code format and/or Source Code format and is governed by the sections
 * of the MSLA applicable to Object Code, Source Code and Modified Open Source
 * Code. By using this software, you agree to the terms of the MSLA.
 *
 * [1]: https://www.silabs.com/about-us/legal/master-software-license-agreement
 */
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "bits.h"
#include "log.h"

FILE *g_trace_stream = NULL;
unsigned int g_enabled_traces = 0;
bool g_enable_color_traces = true;

char *str_bytes(const void *in_start, size_t in_len, const void **in_done, char *out_start, size_t out_len, int opt)
{
    const char *delim = "";
    const char *ellipsis = "";
    const char *fmt = "%s%02x";
    const uint8_t *in = in_start;
    const uint8_t *in_end = in + in_len;
    char *out = out_start;
    char *out_end = out + out_len;
    char *ellipsis_ptr;
    int entry_len;

    BUG_ON(!out);
    BUG_ON(!out_len);

    if (opt & DELIM_SPACE)
        delim = " ";
    if (opt & DELIM_COLON)
        delim = ":";
    if (opt & DELIM_COMMA)
        delim = ", ";
    if (opt & FMT_LHEX)
        fmt = "%s%02x";
    if (opt & FMT_UHEX)
        fmt = "%s%02X";
    if (opt & FMT_DEC)
        fmt = "%s%d";
    if (opt & FMT_DEC_PAD)
        fmt = "%s%3d";
    if (opt & FMT_ASCII_PRINT)
        fmt = "%s\\x%02x";
    if (opt & FMT_ASCII_ALNUM)
        fmt = "%s\\x%02x";
    if (opt & ELLIPSIS_STAR)
        ellipsis = "*";
    if (opt & ELLIPSIS_DOTS)
        ellipsis = "...";

    if (in_done)
        *in_done = in;

    if (!in) {
        snprintf(out, out_len, "<null>");
        return out;
    }

    if (!in_len) {
        out[0] = '\0';
        return out;
    }

    ellipsis_ptr = NULL;
    while (in < in_end) {
        if ((opt & FMT_ASCII_ALNUM && isalnum(*in)) ||
            (opt & FMT_ASCII_PRINT && isprint(*in) && *in != '\\'))
            entry_len = snprintf(out, out_end - out, "%s%c", in == in_start ? "" : delim, *in);
        else
            entry_len = snprintf(out, out_end - out, fmt, in == in_start ? "" : delim, *in);
        if (out + entry_len + strlen(ellipsis) >= out_end && !ellipsis_ptr) {
            if (in_done)
                *in_done = in;
             ellipsis_ptr = out;
        }
        if (out + entry_len >= out_end) {
            if (opt & ELLIPSIS_ABRT)
                BUG("buffer is too small");
            snprintf(ellipsis_ptr, out_end - ellipsis_ptr, "%s", ellipsis);
            return out;
        }
        out += entry_len;
        in++;
    }
    if (in_done)
        *in_done = in;
    return out;
}

char *str_key(const uint8_t *in, int in_len, char *out, int out_len)
{
    return str_bytes(in, in_len, NULL, out, out_len, DELIM_COLON);
}

char *str_eui48(const uint8_t in[static 6], char out[static STR_MAX_LEN_EUI48])
{
    return str_bytes(in, 6, NULL, out, STR_MAX_LEN_EUI64, DELIM_COLON);
}

char *str_eui64(const uint8_t in[static 8], char out[static STR_MAX_LEN_EUI64])
{
    return str_bytes(in, 8, NULL, out, STR_MAX_LEN_EUI64, DELIM_COLON);
}

char *str_ipv4(uint8_t in[static 4], char out[static STR_MAX_LEN_IPV4])
{
    sprintf(out, "%d.%d.%d.%d", in[0], in[1], in[2], in[3]);
    return out;
}

char *str_ipv6(const uint8_t in[static 16], char out[static STR_MAX_LEN_IPV6])
{
    int zero_start = -1;
    int zero_len = 0;
    int last_zero_start = -1;
    int last_zero_len = 0;
    int i, j;

    // Find largest 0 sequence
    for (i = 0; i <= 8; i++) {
        if (i == 8 || in[i * 2] || in[i * 2 + 1]) {
            if (last_zero_len > zero_len) {
                zero_len = last_zero_len;
                zero_start = last_zero_start;
            }
            last_zero_start = -1;
            last_zero_len = 0;;
        } else {
            if (last_zero_start < 0)
                last_zero_start = i;
            last_zero_len++;
        }
    }

    i = j = 0;
    while (i < 8) {
        if (i == zero_start) {
            out[j++] = ':';
            i += zero_len;
        } else {
            if (i)
                out[j++] = ':';
            j += sprintf(out + j, "%x", in[i * 2] * 256 + in[i * 2 + 1]);
            i++;
        }
    }
    out[j] = '\0';
    return out;
}

char *str_ipv4_prefix(uint8_t in[], int prefix_len, char out[static STR_MAX_LEN_IPV4_NET])
{
    uint8_t tmp[4] = { };

    bitcpy(tmp, in, prefix_len);
    str_ipv4(tmp, out);
    sprintf(out + strlen(out), "/%d", prefix_len);
    return out;
}

char *str_ipv6_prefix(const uint8_t in[], int prefix_len, char out[static STR_MAX_LEN_IPV6_NET])
{
    uint8_t tmp[16] = { };

    bitcpy(tmp, in, prefix_len);
    str_ipv6(tmp, out);
    sprintf(out + strlen(out), "/%d", prefix_len);
    return out;
}

static __thread char trace_buffer[256];
static __thread int trace_idx = 0;
/*
 * trace_nested_counter allow to handle nested trace calls. For exemple:
 *  char *a() {
 *      DEBUG();
 *      ...;
 *  }
 *  ...
 *  DEBUG("%d", tr_bytes(...), tr_bytes(a()));
 */
static __thread int trace_nested_counter = 0;

void __tr_enter()
{
    if (!g_trace_stream) {
        g_trace_stream = stdout;
        setlinebuf(stdout);
        g_enable_color_traces = isatty(fileno(g_trace_stream));
    }
    trace_nested_counter++;
}

void __tr_exit()
{
    trace_nested_counter--;
    if (!trace_nested_counter)
        trace_idx = 0;
}

const char *tr_bytes(const void *in, int len, const void **in_done, int max_out, int opt)
{
    char *out = trace_buffer + trace_idx;

    if (trace_idx + max_out > sizeof(trace_buffer))
        return "[OVERFLOW]";
    str_bytes(in, len, in_done, out, max_out, opt);
    trace_idx += strlen(out) + 1;
    BUG_ON(trace_idx > sizeof(trace_buffer));
    return out;
}

const char *tr_key(const uint8_t in[], int in_len)
{
    char *out = trace_buffer + trace_idx;

    if (trace_idx + in_len * 3 > sizeof(trace_buffer))
        return "[OVERFLOW]";
    str_key(in, in_len, out, in_len * 3);
    trace_idx += strlen(out) + 1;
    BUG_ON(trace_idx > sizeof(trace_buffer));
    return out;
}

const char *tr_eui48(const uint8_t in[static 6])
{
    char *out = trace_buffer + trace_idx;

    if (trace_idx + STR_MAX_LEN_EUI48 > sizeof(trace_buffer))
        return "[OVERFLOW]";
    str_eui48(in, out);
    trace_idx += strlen(out) + 1;
    BUG_ON(trace_idx > sizeof(trace_buffer));
    return out;
}

const char *tr_eui64(const uint8_t in[static 8])
{
    char *out = trace_buffer + trace_idx;

    if (trace_idx + STR_MAX_LEN_EUI64 > sizeof(trace_buffer))
        return "[OVERFLOW]";
    str_eui64(in, out);
    trace_idx += strlen(out) + 1;
    BUG_ON(trace_idx > sizeof(trace_buffer));
    return out;
}

const char *tr_ipv4(uint8_t in[static 4])
{
    char *out = trace_buffer + trace_idx;

    if (trace_idx + STR_MAX_LEN_IPV4 > sizeof(trace_buffer))
        return "[OVERFLOW]";
    str_ipv4(in, out);
    trace_idx += strlen(out) + 1;
    BUG_ON(trace_idx > sizeof(trace_buffer));
    return out;
}

const char *tr_ipv6(const uint8_t in[static 16])
{
    char *out = trace_buffer + trace_idx;

    if (trace_idx + STR_MAX_LEN_IPV6 > sizeof(trace_buffer))
        return "[OVERFLOW]";
    str_ipv6(in, out);
    trace_idx += strlen(out) + 1;
    BUG_ON(trace_idx > sizeof(trace_buffer));
    return out;
}

const char *tr_ipv4_prefix(uint8_t in[], int prefix_len)
{
    char *out = trace_buffer + trace_idx;

    if (trace_idx + STR_MAX_LEN_IPV4_NET > sizeof(trace_buffer))
        return "[OVERFLOW]";
    str_ipv4_prefix(in, prefix_len, out);
    trace_idx += strlen(out) + 1;
    BUG_ON(trace_idx > sizeof(trace_buffer));
    return out;
}

const char *tr_ipv6_prefix(const uint8_t in[], int prefix_len)
{
    char *out = trace_buffer + trace_idx;

    if (trace_idx + STR_MAX_LEN_IPV6_NET > sizeof(trace_buffer))
        return "[OVERFLOW]";
    str_ipv6_prefix(in, prefix_len, out);
    trace_idx += strlen(out) + 1;
    BUG_ON(trace_idx > sizeof(trace_buffer));
    return out;
}
