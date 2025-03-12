// Mostly lifted from https://git.codelinaro.org/linaro/qcomlt/u-boot/-/blob/caleb/rbx-integration/drivers/spmi/spmi-msm.c
#ifndef _SPMI_H_
#define _SPMI_H_

#include "utils.h"

#define SPMI_USID (0)

int pmic_qcom_write(u32 reg, u8 val);
int pmic_qcom_read(u32 reg);

int spmi_write(int usid, int pid, int off, u8 val);
int spmi_read(int usid, int pid, int off);
int spmi_init();

#endif // _SPMI_H_