#ifndef __TXCLIENT_H__
#define __TXCLIENT_H__

#include <stdint.h>
#include <switch/services/sm.h>

#ifdef __cplusplus
extern "C" {
#endif

Result txinit(void);
void txexit(void);

Result txforcedisableftp(void);


#ifdef __cplusplus
}
#endif

#endif
