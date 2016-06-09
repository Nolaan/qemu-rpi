#include "qemu-common.h"
#include "qemu/error-report.h"
#include "hw/char/serial.h"
#include "hw/arm/arm.h"
#include "hw/timer/allwinner-a10-pit.h"
#include "hw/intc/allwinner-a10-pic.h"
#include "hw/net/allwinner_emac.h"

#include "sysemu/sysemu.h"
#include "exec/address-spaces.h"

