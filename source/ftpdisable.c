#ifndef PLATFORM_PC
#include <switch.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "ftpdisable.h"

static Service g_txSrv;
static u64 g_refCnt;


Result txinit(void) {
    atomicIncrement64(&g_refCnt);

    if (serviceIsActive(&g_txSrv))
        return 0;

    Result rc = smGetService(&g_txSrv, "tx");
    if (R_FAILED(rc))
        return rc;

    if (R_FAILED(rc))
        txexit();

    return rc;
}


void txexit(void) {
    if (atomicDecrement64(&g_refCnt) == 0)
    {
        serviceClose(&g_txSrv);
    }
}


Result txforcedisableftp(void) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
        u64 enable;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 130;
    raw->enable = 0;

    Result rc = serviceIpcDispatch(&g_txSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;
    }

    return rc;
}




#endif
