/* SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2021, Silicon Labs
 * Main authors:
 *     - Jérôme Pouiller <jerome.pouiller@silabs.com>
 */
#ifndef WSBR_H
#define WSBR_H

#include <linux/if.h>

struct phy_device_driver_s;
struct eth_mac_api_s;
struct mac_api_s;
struct slist;

struct wsbr_ctxt {
    struct phy_device_driver_s *tun_driver;
    struct eth_mac_api_s *tun_mac_api;
    int  tun_driver_id;
    int  tun_if_id;
    int  tun_fd;
    char tun_dev[IFNAMSIZ];

    struct mac_api_s *rcp_mac_api;
    int  rcp_driver_id;
    int  rcp_if_id;
    int  rcp_trig_fd;
    int  rcp_fd;

    struct slist *timers;
};

// This global variable is necessary for various API of nanostack. Beside this
// case, please never use it.
extern struct wsbr_ctxt g_ctxt;

#endif
