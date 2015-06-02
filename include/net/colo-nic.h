/*
 * COarse-grain LOck-stepping Virtual Machines for Non-stop Service (COLO)
 * (a.k.a. Fault Tolerance or Continuous Replication)
 *
 * Copyright (c) 2015 HUAWEI TECHNOLOGIES CO.,LTD.
 * Copyright (c) 2015 FUJITSU LIMITED
 * Copyright (c) 2015 Intel Corporation
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or
 * later.  See the COPYING file in the top-level directory.
 *
 */

#ifndef COLO_NIC_H
#define COLO_NIC_H
#include "migration/migration-colo.h"

typedef struct COLONicState {
    char nicname[128]; /* forward dev */
    char script[1024]; /* colo script */
    char ifname[128];  /* e.g. tap name */
} COLONicState;

int colo_proxy_init(enum colo_mode mode);
void colo_proxy_destroy(enum colo_mode mode);
void colo_add_nic_devices(COLONicState *cns);
void colo_remove_nic_devices(COLONicState *cns);

int colo_proxy_compare(void);
int colo_proxy_failover(void);
int colo_proxy_checkpoint(enum colo_mode mode);

#endif
