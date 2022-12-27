#include "timers.h"

#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_timers.h>
#include <gba_systemcalls.h>

#include <stdio.h>
#include <string.h>

#include "suite.h"

#define DELAYS 3
#define IRQS 3

struct TimerTestResult {
	u32 tightLoopSum;
	u32 looseLoopSum;
	u16 tightLoopValue;
	u16 looseLoopValue;
};

struct TimerTest {
	const char* testName;
	u32 timer;
	struct TimerTestResult results[DELAYS][IRQS];
};

static const struct TimerTest timerTests[] = {
	{ "0b, 0x0001",  0xC0FFFF, {
		{
			{ 0x00002, 0x00010, 0xFFFF, 0xFFFF },
			{ 0x00002, 0x00010, 0xFFFF, 0xFFFF },
			{ 0x00002, 0x00010, 0xFFFF, 0xFFFF },
		}, {
			{ 0x00002, 0x00010, 0xFFFF, 0xFFFF },
			{ 0x00002, 0x00010, 0xFFFF, 0xFFFF },
			{ 0x00002, 0x00010, 0xFFFF, 0xFFFF },
		}, {
			{ 0x00002, 0x00010, 0xFFFF, 0xFFFF },
			{ 0x00002, 0x00010, 0xFFFF, 0xFFFF },
			{ 0x00002, 0x00010, 0xFFFF, 0xFFFF },
		}
	} },
	{ "0b, 0x0005",  0xC0FFFB, {
		{
			{ 0x00003, 0x00010, 0xFFFF, 0xFFFD },
			{ 0x00003, 0x00010, 0xFFFB, 0xFFFE },
			{ 0x00003, 0x00010, 0xFFFD, 0xFFFB },
		}, {
			{ 0x00003, 0x00010, 0xFFFF, 0xFFFD },
			{ 0x00003, 0x00010, 0xFFFB, 0xFFFE },
			{ 0x00003, 0x00010, 0xFFFD, 0xFFFB },
		}, {
			{ 0x00003, 0x00010, 0xFFFF, 0xFFFD },
			{ 0x00003, 0x00010, 0xFFFB, 0xFFFE },
			{ 0x00003, 0x00010, 0xFFFD, 0xFFFB },
		}
	} },
	{ "0b, 0x000C",  0xC0FFF4, {
		{
			{ 0x00003, 0x00020, 0xFFFC, 0xFFFE },
			{ 0x00003, 0x00020, 0xFFFD, 0xFFFF },
			{ 0x00003, 0x00020, 0xFFFF, 0xFFF5 },
		}, {
			{ 0x00003, 0x00020, 0xFFFC, 0xFFFE },
			{ 0x00003, 0x00020, 0xFFFD, 0xFFFF },
			{ 0x00003, 0x00020, 0xFFFF, 0xFFF5 },
		}, {
			{ 0x00003, 0x00020, 0xFFFC, 0xFFFE },
			{ 0x00003, 0x00020, 0xFFFD, 0xFFFF },
			{ 0x00003, 0x00020, 0xFFFF, 0xFFF5 },
		}
	} },
	{ "0b, 0x000D",  0xC0FFF3, {
		{
			{ 0x00004, 0x00020, 0xFFF6, 0xFFF5 },
			{ 0x00004, 0x00020, 0xFFFA, 0xFFF9 },
			{ 0x00004, 0x00020, 0xFFF5, 0xFFF4 },
		}, {
			{ 0x00004, 0x00020, 0xFFF6, 0xFFF5 },
			{ 0x00004, 0x00020, 0xFFFA, 0xFFF9 },
			{ 0x00004, 0x00020, 0xFFF5, 0xFFF4 },
		}, {
			{ 0x00004, 0x00020, 0xFFF6, 0xFFF5 },
			{ 0x00004, 0x00020, 0xFFFA, 0xFFF9 },
			{ 0x00004, 0x00020, 0xFFF5, 0xFFF4 },
		}
	} },
	{ "0b, 0x0010",  0xC0FFF0, {
		{
			{ 0x00004, 0x00020, 0xFFFC, 0xFFFE },
			{ 0x00004, 0x00020, 0xFFF5, 0xFFF7 },
			{ 0x00004, 0x00020, 0xFFF7, 0xFFF9 },
		}, {
			{ 0x00004, 0x00020, 0xFFFC, 0xFFFE },
			{ 0x00004, 0x00020, 0xFFF5, 0xFFF7 },
			{ 0x00004, 0x00020, 0xFFF7, 0xFFF9 },
		}, {
			{ 0x00004, 0x00020, 0xFFFC, 0xFFFE },
			{ 0x00004, 0x00020, 0xFFF5, 0xFFF7 },
			{ 0x00004, 0x00020, 0xFFF7, 0xFFF9 },
		}
	} },
	{ "0b, 0x0014",  0xC0FFEC, {
		{
			{ 0x00004, 0x00020, 0xFFF8, 0xFFF8 },
			{ 0x00004, 0x00020, 0xFFF9, 0xFFF9 },
			{ 0x00004, 0x00020, 0xFFFB, 0xFFFB },
		}, {
			{ 0x00004, 0x00020, 0xFFF8, 0xFFF8 },
			{ 0x00004, 0x00020, 0xFFF9, 0xFFF9 },
			{ 0x00004, 0x00020, 0xFFFB, 0xFFFB },
		}, {
			{ 0x00004, 0x00020, 0xFFF8, 0xFFF8 },
			{ 0x00004, 0x00020, 0xFFF9, 0xFFF9 },
			{ 0x00004, 0x00020, 0xFFFB, 0xFFFB },
		}
	} },
	{ "0b, 0x0015",  0xC0FFEB, {
		{
			{ 0x00005, 0x00020, 0xFFF5, 0xFFF3 },
			{ 0x00005, 0x00020, 0xFFF0, 0xFFEE },
			{ 0x00005, 0x00020, 0xFFFB, 0xFFF9 },
		}, {
			{ 0x00005, 0x00020, 0xFFF5, 0xFFF3 },
			{ 0x00005, 0x00020, 0xFFF0, 0xFFEE },
			{ 0x00005, 0x00020, 0xFFFB, 0xFFF9 },
		}, {
			{ 0x00005, 0x00020, 0xFFF5, 0xFFF3 },
			{ 0x00005, 0x00020, 0xFFF0, 0xFFEE },
			{ 0x00005, 0x00020, 0xFFFB, 0xFFF9 },
		}
	} },
	{ "0b, 0x0020",  0xC0FFE0, {
		{
			{ 0x00006, 0x00020, 0xFFFC, 0xFFFC },
			{ 0x00006, 0x00020, 0xFFF5, 0xFFF5 },
			{ 0x00006, 0x00020, 0xFFE7, 0xFFE7 },
		}, {
			{ 0x00006, 0x00020, 0xFFFC, 0xFFFC },
			{ 0x00006, 0x00020, 0xFFF5, 0xFFF5 },
			{ 0x00006, 0x00020, 0xFFE7, 0xFFE7 },
		}, {
			{ 0x00006, 0x00020, 0xFFFC, 0xFFFC },
			{ 0x00006, 0x00020, 0xFFF5, 0xFFF5 },
			{ 0x00006, 0x00020, 0xFFE7, 0xFFE7 },
		}
	} },
	{ "0b, 0x0024",  0xC0FFDC, {
		{
			{ 0x00006, 0x00030, 0xFFF0, 0xFFF1 },
			{ 0x00006, 0x00030, 0xFFFD, 0xFFFE },
			{ 0x00006, 0x00030, 0xFFF3, 0xFFF4 },
		}, {
			{ 0x00006, 0x00030, 0xFFF0, 0xFFF1 },
			{ 0x00006, 0x00030, 0xFFFD, 0xFFFE },
			{ 0x00006, 0x00030, 0xFFF3, 0xFFF4 },
		}, {
			{ 0x00006, 0x00030, 0xFFF0, 0xFFF1 },
			{ 0x00006, 0x00030, 0xFFFD, 0xFFFE },
			{ 0x00006, 0x00030, 0xFFF3, 0xFFF4 },
		}
	} },
	{ "0b, 0x0025",  0xC0FFDB, {
		{
			{ 0x00007, 0x00030, 0xFFEF, 0xFFED },
			{ 0x00007, 0x00030, 0xFFF9, 0xFFF7 },
			{ 0x00007, 0x00030, 0xFFE8, 0xFFE6 },
		}, {
			{ 0x00007, 0x00030, 0xFFEF, 0xFFED },
			{ 0x00007, 0x00030, 0xFFF9, 0xFFF7 },
			{ 0x00007, 0x00030, 0xFFE8, 0xFFE6 },
		}, {
			{ 0x00007, 0x00030, 0xFFEF, 0xFFED },
			{ 0x00007, 0x00030, 0xFFF9, 0xFFF7 },
			{ 0x00007, 0x00030, 0xFFE8, 0xFFE6 },
		}
	} },
	{ "0b, 0x0040",  0xC0FFC0, {
		{
			{ 0x0000A, 0x00040, 0xFFDC, 0xFFDC },
			{ 0x0000B, 0x00040, 0xFFDC, 0xFFDC },
			{ 0x0000D, 0x00050, 0xFFDE, 0xFFDE },
		}, {
			{ 0x0000A, 0x00040, 0xFFDC, 0xFFDC },
			{ 0x0000B, 0x00040, 0xFFDC, 0xFFDC },
			{ 0x0000D, 0x00050, 0xFFDE, 0xFFDE },
		}, {
			{ 0x0000A, 0x00040, 0xFFDC, 0xFFDC },
			{ 0x0000B, 0x00040, 0xFFDC, 0xFFDC },
			{ 0x0000D, 0x00050, 0xFFDE, 0xFFDE },
		}
	} },
	{ "0b, 0x0080",  0xC0FF80, {
		{
			{ 0x00012, 0x00070, 0xFFDC, 0xFFDD },
			{ 0x00013, 0x00070, 0xFFDC, 0xFFDC },
			{ 0x00015, 0x00070, 0xFFDE, 0xFFDC },
		}, {
			{ 0x00012, 0x00070, 0xFFDC, 0xFFDD },
			{ 0x00013, 0x00070, 0xFFDC, 0xFFDC },
			{ 0x00015, 0x00070, 0xFFDE, 0xFFDC },
		}, {
			{ 0x00012, 0x00070, 0xFFDC, 0xFFDD },
			{ 0x00013, 0x00070, 0xFFDC, 0xFFDC },
			{ 0x00015, 0x00070, 0xFFDE, 0xFFDC },
		}
	} },
	{ "0b, 0x0800",  0xC0F800, {
		{
			{ 0x00102, 0x005A0, 0xF85C, 0xF85C },
			{ 0x001F3, 0x00AE0, 0xF85C, 0xF85C },
			{ 0x003D5, 0x01550, 0xF85E, 0xF85C },
		}, {
			{ 0x00102, 0x005A0, 0xF85C, 0xF85C },
			{ 0x001F3, 0x00AE0, 0xF85C, 0xF85C },
			{ 0x003D5, 0x01550, 0xF85E, 0xF85C },
		}, {
			{ 0x00102, 0x005A0, 0xF85C, 0xF85C },
			{ 0x001F3, 0x00AE0, 0xF85C, 0xF85C },
			{ 0x003D5, 0x01550, 0xF85E, 0xF85C },
		}
	} },
	{ "0b, 0x8000",  0xC08000, {
		{
			{ 0x01002, 0x05920, 0x805C, 0x805E },
			{ 0x01FF3, 0x0B1D0, 0x805C, 0x805C },
			{ 0x03FD5, 0x16340, 0x805E, 0x805C },
		}, {
			{ 0x01002, 0x05920, 0x805C, 0x805E },
			{ 0x01FF3, 0x0B1D0, 0x805C, 0x805C },
			{ 0x03FD5, 0x16340, 0x805E, 0x805C },
		}, {
			{ 0x01002, 0x05920, 0x805C, 0x805E },
			{ 0x01FF3, 0x0B1D0, 0x805C, 0x805C },
			{ 0x03FD5, 0x16340, 0x805E, 0x805C },
		}
	} },
	{ "6b, 0x0010",  0xC1FFF0, {
		{
			{ 0x0007B, 0x002B0, 0xFFF1, 0xFFF1 },
			{ 0x000EC, 0x00520, 0xFFF1, 0xFFF1 },
			{ 0x001CE, 0x00A10, 0xFFF1, 0xFFF1 },
		}, {
			{ 0x0007B, 0x002B0, 0xFFF1, 0xFFF1 },
			{ 0x000EB, 0x00520, 0xFFF1, 0xFFF1 },
			{ 0x001CD, 0x00A10, 0xFFF1, 0xFFF1 },
		}, {
			{ 0x00081, 0x002D0, 0xFFF1, 0xFFF1 },
			{ 0x000F2, 0x00550, 0xFFF1, 0xFFF1 },
			{ 0x001D4, 0x00A30, 0xFFF1, 0xFFF1 },
		}
	} },
	{ "6b, 0x0011",  0xC1FFEF, {
		{
			{ 0x00083, 0x002E0, 0xFFF0, 0xFFF0 },
			{ 0x000FC, 0x00580, 0xFFF0, 0xFFF0 },
			{ 0x001EE, 0x00AC0, 0xFFF0, 0xFFF0 },
		}, {
			{ 0x00083, 0x002E0, 0xFFF0, 0xFFF0 },
			{ 0x000FB, 0x00580, 0xFFF0, 0xFFF0 },
			{ 0x001ED, 0x00AC0, 0xFFF0, 0xFFF0 },
		}, {
			{ 0x00089, 0x00300, 0xFFF0, 0xFFF0 },
			{ 0x00102, 0x005A0, 0xFFF0, 0xFFF0 },
			{ 0x001F4, 0x00AE0, 0xFFF0, 0xFFF0 },
		}
	} },
	{ "6b, 0x0012",  0xC1FFEE, {
		{
			{ 0x0008B, 0x00310, 0xFFEF, 0xFFEF },
			{ 0x0010C, 0x005E0, 0xFFEF, 0xFFEF },
			{ 0x0020E, 0x00B70, 0xFFEF, 0xFFEF },
		}, {
			{ 0x0008B, 0x00310, 0xFFEF, 0xFFEF },
			{ 0x0010B, 0x005D0, 0xFFEF, 0xFFEF },
			{ 0x0020D, 0x00B70, 0xFFEF, 0xFFEF },
		}, {
			{ 0x00091, 0x00330, 0xFFEF, 0xFFEF },
			{ 0x00112, 0x00600, 0xFFEF, 0xFFEF },
			{ 0x00214, 0x00B90, 0xFFEF, 0xFFEF },
		}
	} },
	{ "6b, 0x0013",  0xC1FFED, {
		{
			{ 0x00093, 0x00340, 0xFFEE, 0xFFEE },
			{ 0x0011C, 0x00630, 0xFFEE, 0xFFEE },
			{ 0x0022E, 0x00C20, 0xFFEE, 0xFFEE },
		}, {
			{ 0x00093, 0x00330, 0xFFEE, 0xFFEE },
			{ 0x0011B, 0x00630, 0xFFEE, 0xFFEE },
			{ 0x0022D, 0x00C20, 0xFFEE, 0xFFEE },
		}, {
			{ 0x00099, 0x00360, 0xFFEE, 0xFFEE },
			{ 0x00122, 0x00650, 0xFFEE, 0xFFEE },
			{ 0x00234, 0x00C40, 0xFFEE, 0xFFEE },
		}
	} },
	{ "8b, 0x0010",  0xC2FFF0, {
		{
			{ 0x001EB, 0x00AB0, 0xFFF0, 0xFFF0 },
			{ 0x003DC, 0x01580, 0xFFF0, 0xFFF0 },
			{ 0x007BE, 0x02B20, 0xFFF0, 0xFFF0 },
		}, {
			{ 0x001EB, 0x00AB0, 0xFFF0, 0xFFF0 },
			{ 0x003DB, 0x01580, 0xFFF0, 0xFFF0 },
			{ 0x007BD, 0x02B10, 0xFFF0, 0xFFF0 },
		}, {
			{ 0x001E9, 0x00AB0, 0xFFF0, 0xFFF0 },
			{ 0x003DA, 0x01570, 0xFFF0, 0xFFF0 },
			{ 0x007BC, 0x02B10, 0xFFF0, 0xFFF0 },
		}
	} },
	{ "8b, 0x0011",  0xC2FFEF, {
		{
			{ 0x0020B, 0x00B60, 0xFFEF, 0xFFEF },
			{ 0x0041C, 0x016E0, 0xFFEF, 0xFFEF },
			{ 0x0083E, 0x02DE0, 0xFFEF, 0xFFEF },
		}, {
			{ 0x0020B, 0x00B60, 0xFFEF, 0xFFEF },
			{ 0x0041B, 0x016E0, 0xFFEF, 0xFFEF },
			{ 0x0083D, 0x02DE0, 0xFFEF, 0xFFEF },
		}, {
			{ 0x00209, 0x00B60, 0xFFEF, 0xFFEF },
			{ 0x0041A, 0x016E0, 0xFFEF, 0xFFEF },
			{ 0x0083C, 0x02DE0, 0xFFEF, 0xFFEF },
		}
	} },
	{ "8b, 0x0012",  0xC2FFEE, {
		{
			{ 0x0022B, 0x00C10, 0xFFEE, 0xFFEE },
			{ 0x0045C, 0x01850, 0xFFEE, 0xFFEE },
			{ 0x008BE, 0x030B0, 0xFFEE, 0xFFEE },
		}, {
			{ 0x0022B, 0x00C10, 0xFFEE, 0xFFEE },
			{ 0x0045B, 0x01840, 0xFFEE, 0xFFEE },
			{ 0x008BD, 0x030A0, 0xFFEE, 0xFFEE },
		}, {
			{ 0x00229, 0x00C10, 0xFFEE, 0xFFEE },
			{ 0x0045A, 0x01840, 0xFFEE, 0xFFEE },
			{ 0x008BC, 0x030A0, 0xFFEE, 0xFFEE },
		}
	} },
	{ "8b, 0x0013",  0xC2FFED, {
		{
			{ 0x0024B, 0x00CD0, 0xFFED, 0xFFED },
			{ 0x0049C, 0x019B0, 0xFFED, 0xFFED },
			{ 0x0093E, 0x03370, 0xFFED, 0xFFED },
		}, {
			{ 0x0024B, 0x00CC0, 0xFFED, 0xFFED },
			{ 0x0049B, 0x019B0, 0xFFED, 0xFFED },
			{ 0x0093D, 0x03370, 0xFFED, 0xFFED },
		}, {
			{ 0x00249, 0x00CC0, 0xFFED, 0xFFED },
			{ 0x0049A, 0x019A0, 0xFFED, 0xFFED },
			{ 0x0093C, 0x03370, 0xFFED, 0xFFED },
		}
	} },
	{ "10b, 0x0010", 0xC3FFF0, {
		{
			{ 0x007EB, 0x02C10, 0xFFF0, 0xFFF0 },
			{ 0x00FDC, 0x05850, 0xFFF0, 0xFFF0 },
			{ 0x01FBE, 0x0B0B0, 0xFFF0, 0xFFF0 },
		}, {
			{ 0x007EB, 0x02C10, 0xFFF0, 0xFFF0 },
			{ 0x00FDB, 0x05840, 0xFFF0, 0xFFF0 },
			{ 0x01FBD, 0x0B0A0, 0xFFF0, 0xFFF0 },
		}, {
			{ 0x007E9, 0x02C10, 0xFFF0, 0xFFF0 },
			{ 0x00FDA, 0x05840, 0xFFF0, 0xFFF0 },
			{ 0x01FBC, 0x0B0A0, 0xFFF0, 0xFFF0 },
		}
	} },
	{ "10b, 0x0011", 0xC3FFEF, {
		{
			{ 0x0086B, 0x02EE0, 0xFFEF, 0xFFEF },
			{ 0x010DC, 0x05DE0, 0xFFEF, 0xFFEF },
			{ 0x021BE, 0x0BBD0, 0xFFEF, 0xFFEF },
		}, {
			{ 0x0086B, 0x02EE0, 0xFFEF, 0xFFEF },
			{ 0x010DB, 0x05DD0, 0xFFEF, 0xFFEF },
			{ 0x021BD, 0x0BBD0, 0xFFEF, 0xFFEF },
		}, {
			{ 0x00869, 0x02ED0, 0xFFEF, 0xFFEF },
			{ 0x010DA, 0x05DD0, 0xFFEF, 0xFFEF },
			{ 0x021BC, 0x0BBC0, 0xFFEF, 0xFFEF },
		}
	} },
	{ "10b, 0x0012", 0xC3FFEE, {
		{
			{ 0x008EB, 0x031A0, 0xFFEE, 0xFFEE },
			{ 0x011DC, 0x06370, 0xFFEE, 0xFFEE },
			{ 0x023BE, 0x0C6F0, 0xFFEE, 0xFFEE },
		}, {
			{ 0x008EB, 0x031A0, 0xFFEE, 0xFFEE },
			{ 0x011DB, 0x06360, 0xFFEE, 0xFFEE },
			{ 0x023BD, 0x0C6F0, 0xFFEE, 0xFFEE },
		}, {
			{ 0x008E9, 0x031A0, 0xFFEE, 0xFFEE },
			{ 0x011DA, 0x06360, 0xFFEE, 0xFFEE },
			{ 0x023BC, 0x0C6E0, 0xFFEE, 0xFFEE },
		}
	} },
	{ "10b, 0x0013", 0xC3FFED, {
		{
			{ 0x0096B, 0x03470, 0xFFED, 0xFFED },
			{ 0x012DC, 0x06900, 0xFFED, 0xFFED },
			{ 0x025BE, 0x0D210, 0xFFED, 0xFFED },
		}, {
			{ 0x0096B, 0x03470, 0xFFED, 0xFFED },
			{ 0x012DB, 0x068F0, 0xFFED, 0xFFED },
			{ 0x025BD, 0x0D210, 0xFFED, 0xFFED },
		}, {
			{ 0x00969, 0x03460, 0xFFED, 0xFFED },
			{ 0x012DA, 0x068F0, 0xFFED, 0xFFED },
			{ 0x025BC, 0x0D200, 0xFFED, 0xFFED },
		}
	} },
};

static const u32 nTimerTests = sizeof(timerTests) / sizeof(*timerTests);

static unsigned passes;
static unsigned totalResults;
EWRAM_DATA static bool results[sizeof(timerTests) / sizeof(*timerTests)];

static u32 irqCounter;

IWRAM_CODE
static void testIrq(void) {
	--irqCounter;
	if (!irqCounter) {
		REG_TM0CNT_H = 0;
	}
}

IWRAM_CODE
__attribute__((noinline))
static void runTest(struct TimerTest* test) {
	REG_IME = 0;
	irqInit();
	irqSet(IRQ_TIMER0, testIrq);
	int ie = REG_IE;
	REG_IE = IRQ_TIMER0;
	REG_IME = 1;

	int d, i;
	for (d = 0; d < DELAYS; ++d) {
		for (i = 0; i < IRQS; ++i) {
			struct TimerTestResult* result = &test->results[d][i];
			int dd = 1 << d;
			int ii = 1 << i;
			__asm__ volatile("ldr r0, =0f \n"
				"bx r0 \n"
				".arm; 0:\n"
				"ldr r4, =0x4000100 \n"
				"ldr r5, =0x0C3FFFE \n"
				"ldr r6, =irqCounter \n"
				"ldr r3, =activeTestInfo+4 \n"

				"mov r0, #0 \n"
				"strh r0, [r3] \n"
				"str r5, [r4] \n"
				"mov r0, #1 \n"
				"str r0, [r6] \n"
				"mov r1, #8 \n"
				"swi 0x40000 \n"
				"str %[irqs], [r6] \n"
				"mov r0, #1 \n"
				"1: \n"
				"cmp r0, %[delay] \n"
				"addne r0, #1 \n"
				"bne 1b \n"
				"mov r0, #0 \n"
				"str %[timer], [r4] \n"
				"1: \n"
				"add r0, #1 \n"
				"ldr r2, [r4] \n"
				"tst r2, #0x800000 \n"
				"bne 1b \n"
				"str r0, [%[result], #0] \n"
				"strh r2, [%[result], #8] \n"

				"mov r0, #1 \n"
				"strh r0, [r3] \n"
				"str r5, [r4] \n"
				"mov r0, #1 \n"
				"str r0, [r6] \n"
				"mov r1, #8 \n"
				"swi 0x40000 \n"
				"str %[irqs], [r6] \n"
				"mov r0, #1 \n"
				"1: \n"
				"cmp r0, %[delay] \n"
				"addne r0, #1 \n"
				"bne 1b \n"
				"mov r0, #0 \n"
				"str %[timer], [r4] \n"
				"1: \n"
				"add r0, #1 \n"
				"add r0, #1 \n"
				"add r0, #1 \n"
				"add r0, #1 \n"
				"add r0, #1 \n"
				"add r0, #1 \n"
				"add r0, #1 \n"
				"add r0, #1 \n"
				"add r0, #1 \n"
				"add r0, #1 \n"
				"add r0, #1 \n"
				"add r0, #1 \n"
				"add r0, #1 \n"
				"add r0, #1 \n"
				"add r0, #1 \n"
				"add r0, #1 \n"
				"ldr r2, [r4] \n"
				"tst r2, #0x800000 \n"
				"bne 1b \n"
				"str r0, [%[result], #4] \n"
				"strh r2, [%[result], #10] \n"
				"ldr r0, =0xFFFF \n"
				"strh r0, [r3] \n"
				"ldr r0, =2f \n"
				"add r0, #1 \n"
				"bx r0 \n"
				".ltorg \n"
				".thumb; 2:"
				:
				: [result]"r"(result), [timer]"r"(test->timer), [delay]"r"(dd), [irqs]"r"(ii)
				: "r0", "r1", "r2", "r3", "r4", "r5", "r6", "memory");
		}
	}

	REG_IME = 0;
	REG_IE = ie;
	REG_IME = 1;
}

static void printResultLine(int offset, int line, const char* preface, const char* p2, u32 value, u32 expected) {
	static const int base = 3;
	if (offset > line || base + line - offset > 18) {
		return;
	}

	line += base - offset;
	snprintf(TEXT_LOC(line, 0), 31, "%-4s %-4s: %06lX", p2, preface, value);
	if (value == expected) {
		markLinePass(line);
		strncpy(TEXT_LOC(line, 19), "PASS", 10);
	} else {
		markLineFail(line);
		snprintf(TEXT_LOC(line, 19), 16, "!= %06lX", expected);
	}
}

static void printResult(int offset, int line, const char* preface, const struct TimerTestResult* value, const struct TimerTestResult* expected) {
	printResultLine(offset, line, preface, "1xs", value->tightLoopSum, expected->tightLoopSum);
	printResultLine(offset, line + 1, preface, "16xs", value->looseLoopSum, expected->looseLoopSum);
	printResultLine(offset, line + 2, preface, "1xv", value->tightLoopValue, expected->tightLoopValue);
	printResultLine(offset, line + 3, preface, "16xv", value->looseLoopValue, expected->looseLoopValue);
}

static void doResult(const char* preface, const char* testName, const struct TimerTestResult* value, const struct TimerTestResult* expected) {
	if (value->tightLoopSum != expected->tightLoopSum) {
		debugprintf("FAIL: %s 1xs %s", testName, preface);
		savprintf("1xs %s: Got %08lX vs %08lX: FAIL", preface, value->tightLoopSum, expected->tightLoopSum);
	} else {
		debugprintf("PASS: %s 1xs %s", testName, preface);
		++passes;
	}
	if (value->looseLoopSum != expected->looseLoopSum) {
		debugprintf("FAIL: %s 16xs %s", testName, preface);
		savprintf("16xs %s: Got %08lX vs %08lX: FAIL", preface, value->looseLoopSum, expected->looseLoopSum);
	} else {
		debugprintf("PASS: %s 16xs %s", testName, preface);
		++passes;
	}
	if (value->tightLoopValue != expected->tightLoopValue) {
		debugprintf("FAIL: %s 1xv %s", testName, preface);
		savprintf("1xv %s: Got %04X vs %04X: FAIL", preface, value->tightLoopValue, expected->tightLoopValue);
	} else {
		debugprintf("PASS: %s 1xv %s", testName, preface);
		++passes;
	}
	if (value->looseLoopValue != expected->looseLoopValue) {
		debugprintf("FAIL: %s 16xv %s", testName, preface);
		savprintf("16xv %s: Got %04X vs %04X: FAIL", preface, value->looseLoopValue, expected->looseLoopValue);
	} else {
		debugprintf("PASS: %s 16xv %s", testName, preface);
		++passes;
	}

	totalResults += 4;
}

static void printResults(const char* preface, const struct TimerTest* values, const struct TimerTest* expected, int base) {
	snprintf(&textGrid[32], 31, "Timer count-up test: %s", preface);
	printResult(base, 0,  "1d 1i", &values->results[0][0], &expected->results[0][0]);
	printResult(base, 4,  "2d 1i", &values->results[1][0], &expected->results[1][0]);
	printResult(base, 8,  "4d 1i", &values->results[2][0], &expected->results[2][0]);
	printResult(base, 12, "1d 2i", &values->results[0][1], &expected->results[0][1]);
	printResult(base, 16, "2d 2i", &values->results[1][1], &expected->results[1][1]);
	printResult(base, 20, "4d 2i", &values->results[2][1], &expected->results[2][1]);
	printResult(base, 24, "1d 4i", &values->results[0][2], &expected->results[0][2]);
	printResult(base, 28, "2d 4i", &values->results[1][2], &expected->results[1][2]);
	printResult(base, 32, "4d 4i", &values->results[2][2], &expected->results[2][2]);
}

static void runTimersSuite(void) {
	passes = 0;
	totalResults = 0;
	const struct TimerTest* activeTest = 0;
	int i;
	for (i = 0; i < nTimerTests; ++i) {
		struct TimerTest currentTest = {0};
		activeTest = &timerTests[i];
		memcpy(&currentTest, activeTest, sizeof(currentTest));
		activeTestInfo.testId = i;
		runTest(&currentTest);

		unsigned failed = totalResults - passes;
		savprintf("Timer count-up test: %s", activeTest->testName);
		doResult("1d 1i", activeTest->testName, &currentTest.results[0][0], &activeTest->results[0][0]);
		doResult("2d 1i", activeTest->testName, &currentTest.results[1][0], &activeTest->results[1][0]);
		doResult("4d 1i", activeTest->testName, &currentTest.results[2][0], &activeTest->results[2][0]);
		doResult("1d 2i", activeTest->testName, &currentTest.results[0][1], &activeTest->results[0][1]);
		doResult("2d 2i", activeTest->testName, &currentTest.results[1][1], &activeTest->results[1][1]);
		doResult("4d 2i", activeTest->testName, &currentTest.results[2][1], &activeTest->results[2][1]);
		doResult("1d 4i", activeTest->testName, &currentTest.results[0][2], &activeTest->results[0][2]);
		doResult("2d 4i", activeTest->testName, &currentTest.results[1][2], &activeTest->results[1][2]);
		doResult("4d 4i", activeTest->testName, &currentTest.results[2][2], &activeTest->results[2][2]);
		results[i] = failed == totalResults - passes;
	}
	activeTestInfo.testId = -1;
}

static size_t listTimersSuite(const char** names, bool* passed, size_t size, size_t offset) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (i + offset >= nTimerTests) {
			break;
		}
		names[i] = timerTests[i + offset].testName;
		passed[i] = results[i + offset];
	}
	return i;
}

static void showTimersSuite(size_t index) {
	const struct TimerTest* activeTest = &timerTests[index];
	struct TimerTest currentTest = {0};
	size_t resultIndex = 0;
	memset(&textGrid[GRID_STRIDE], 0, sizeof(textGrid) - GRID_STRIDE);
	memcpy(&currentTest, activeTest, sizeof(currentTest));
	activeTestInfo.testId = index;
	runTest(&currentTest);
	updateTextGrid();

	while (1) {
		scanKeys();
		u16 keys = keysDownRepeat();

		if (keys & KEY_UP) {
			if (resultIndex > 0) {
				--resultIndex;
			}
		}
		if (keys & KEY_DOWN) {
			if (resultIndex < 20) {
				++resultIndex;
			}
		}

		if (keys & KEY_B) {
			return;
		}

		printResults(activeTest->testName, &currentTest, activeTest, resultIndex);
		updateTextGrid();
	}
	activeTestInfo.testId = 0;
}

const struct TestSuite timersTestSuite = {
	.name = "Timer count-up tests",
	.run = runTimersSuite,
	.list = listTimersSuite,
	.show = showTimersSuite,
	.nTests = sizeof(timerTests) / sizeof(*timerTests),
	.passes = &passes,
	.totalResults = &totalResults
};
