#include "timers.h"

#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_timers.h>
#include <gba_systemcalls.h>

#include <stdio.h>
#include <string.h>

#include "suite.h"

#define VERSIONS 10
struct TimerIRQTest {
	const char* testName;
	u16 timer;
	u16 results[VERSIONS];
};

static const struct TimerIRQTest timerIRQTests[] = {
	{ "FFFF", 0xFFFF, { 0,   1,  2,  3, 0x51, 0x51, 0x51, 0x51, 0x51, 0x51 } },
	{ "FFFE", 0xFFFE, { 0,   1,  2,  3,    4, 0x52, 0x52, 0x52, 0x52, 0x52 } },
	{ "FFFD", 0xFFFD, { -1,  0,  1,  2,    3,    4, 0x52, 0x52, 0x52, 0x52 } },
	{ "FFFC", 0xFFFC, { -2, -1,  0,  1,    2,    3,    4, 0x52, 0x52, 0x52 } },
	{ "FFFB", 0xFFFB, { -3, -2, -1,  0,    1,    2,    3,    4, 0x52, 0x52 } },
	{ "FFFA", 0xFFFA, { -4, -3, -2, -1,    0,    1,    2,    3,    4, 0x52 } },
	{ "FFF9", 0xFFF9, { -5, -4, -3, -2,   -1,    0,    1,    2,    3,    4 } },
	{ "FFF8", 0xFFF8, { -6, -5, -4, -3,   -2,   -1,    0,    1,    2,    3 } },
	{ "FFF7", 0xFFF7, { -7, -6, -5, -4,   -3,   -2,   -1,    0,    1,    2 } },
};

static const u32 nTimerIRQTests = sizeof(timerIRQTests) / sizeof(*timerIRQTests);

static unsigned passes;
static unsigned totalResults;
EWRAM_DATA static bool results[sizeof(timerIRQTests) / sizeof(*timerIRQTests)];

IWRAM_CODE
static void testIrq(void) {
	REG_TM0CNT_H = 0x0;
}

IWRAM_CODE
__attribute__((noinline))
static void runTest(struct TimerIRQTest* test) {
	REG_IME = 0;
	irqInit();
	irqSet(IRQ_TIMER0, testIrq);
	int ie = REG_IE;
	REG_IE = IRQ_TIMER0;
	REG_IME = 1;

	__asm__ volatile("ldr r0, =0f \n"
		"bx r0 \n"
		".arm; 0:\n"
		"ldr r1, =0x4000100 \n"
		"mov r2, #0xC00000 \n"
		"orr r3, r2, %[timer] \n"
		"ldr r4, =activeTestInfo+4 \n"
	
		"mov r5, #0 \n"
		"strh r5, [r4] \n"
		"str r3, [r1] \n"
		"strh r2, [r1] \n"
		"ldrh r0, [r1] \n"
		"strh r0, [%[result], #0] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"

		"add r5, #1 \n"
		"strh r5, [r4] \n"
		"str r3, [r1] \n"
		"strh r2, [r1] \n"
		"nop \n"
		"ldrh r0, [r1] \n"
		"strh r0, [%[result], #2] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"

		"add r5, #1 \n"
		"strh r5, [r4] \n"
		"str r3, [r1] \n"
		"strh r2, [r1] \n"
		"nop \n"
		"nop \n"
		"ldrh r0, [r1] \n"
		"strh r0, [%[result], #4] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"

		"add r5, #1 \n"
		"strh r5, [r4] \n"
		"str r3, [r1] \n"
		"strh r2, [r1] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"ldrh r0, [r1] \n"
		"strh r0, [%[result], #6] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"

		"add r5, #1 \n"
		"strh r5, [r4] \n"
		"str r3, [r1] \n"
		"strh r2, [r1] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"ldrh r0, [r1] \n"
		"strh r0, [%[result], #8] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"

		"add r5, #1 \n"
		"strh r5, [r4] \n"
		"str r3, [r1] \n"
		"strh r2, [r1] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"ldrh r0, [r1] \n"
		"strh r0, [%[result], #10] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"

		"add r5, #1 \n"
		"strh r5, [r4] \n"
		"str r3, [r1] \n"
		"strh r2, [r1] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"ldrh r0, [r1] \n"
		"strh r0, [%[result], #12] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"

		"add r5, #1 \n"
		"strh r5, [r4] \n"
		"str r3, [r1] \n"
		"strh r2, [r1] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"ldrh r0, [r1] \n"
		"strh r0, [%[result], #14] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"

		"add r5, #1 \n"
		"strh r5, [r4] \n"
		"str r3, [r1] \n"
		"strh r2, [r1] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"ldrh r0, [r1] \n"
		"strh r0, [%[result], #16] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"

		"add r5, #1 \n"
		"strh r5, [r4] \n"
		"str r3, [r1] \n"
		"strh r2, [r1] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"ldrh r0, [r1] \n"
		"strh r0, [%[result], #18] \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"

		"ldr r5, =0xFFFF \n"
		"strh r5, [r4] \n"

		"ldr r0, =1f \n"
		"add r0, #1 \n"
		"bx r0 \n"
		".ltorg \n"
		".thumb; 1:"
		:
		: [result]"r"(test->results), [timer]"r"(test->timer)
		: "r0", "r1", "r2", "r3", "r4", "r5", "memory");
	REG_IME = 0;
	REG_IE = ie;
	REG_IME = 1;
}

static void printResult(int offset, int line, const char* preface, uint16_t value, uint16_t expected) {
	static const int base = 3;
	if (offset > line || base + line - offset > 18) {
		return;
	}

	line += base - offset;
	snprintf(TEXT_LOC(line, 0), 31, "%-4s: %04X", preface, value);
	if (value == expected) {
		markLinePass(line);
		strncpy(TEXT_LOC(line, 19), "PASS", 10);
	} else {
		markLineFail(line);
		snprintf(TEXT_LOC(line, 19), 16, "!= %04X", expected);
	}
}

static void doResult(const char* preface, const char* testName, uint16_t value, uint16_t expected) {
	if (value != expected) {
		debugprintf("FAIL: %s %s", testName, preface);
		savprintf("Got %04X != %04X: FAIL", value, expected);
	} else {
		debugprintf("PASS: %s %s", testName, preface);
		++passes;
	}

	totalResults++;
}

static void printResults(const char* preface, const struct TimerIRQTest* values, const struct TimerIRQTest* expected, int base) {
	snprintf(&textGrid[32], 31, "Timer IRQ test: %s", preface);
	printResult(base, 0, "0 nops", values->results[0], expected->results[0]);
	printResult(base, 1, "1 nop ", values->results[1], expected->results[1]);
	printResult(base, 2, "2 nops", values->results[2], expected->results[2]);
	printResult(base, 3, "3 nops", values->results[3], expected->results[3]);
	printResult(base, 4, "4 nops", values->results[4], expected->results[4]);
	printResult(base, 5, "5 nops", values->results[5], expected->results[5]);
	printResult(base, 6, "6 nops", values->results[6], expected->results[6]);
	printResult(base, 7, "7 nops", values->results[7], expected->results[7]);
	printResult(base, 8, "8 nops", values->results[8], expected->results[8]);
	printResult(base, 9, "9 nops", values->results[9], expected->results[9]);
}

static void runTimerIRQSuite(void) {
	passes = 0;
	totalResults = 0;
	const struct TimerIRQTest* activeTest = 0;
	int i;
	for (i = 0; i < nTimerIRQTests; ++i) {
		struct TimerIRQTest currentTest = {0};
		activeTestInfo.testId = i;
		activeTest = &timerIRQTests[i];
		memcpy(&currentTest, activeTest, sizeof(currentTest));
		runTest(&currentTest);

		unsigned failed = totalResults - passes;
		savprintf("Timer IRQ test: %s", activeTest->testName);
		doResult("0 nops", activeTest->testName, currentTest.results[0], activeTest->results[0]);
		doResult("1 nop ", activeTest->testName, currentTest.results[1], activeTest->results[1]);
		doResult("2 nops", activeTest->testName, currentTest.results[2], activeTest->results[2]);
		doResult("3 nops", activeTest->testName, currentTest.results[3], activeTest->results[3]);
		doResult("4 nops", activeTest->testName, currentTest.results[4], activeTest->results[4]);
		doResult("5 nops", activeTest->testName, currentTest.results[5], activeTest->results[5]);
		doResult("6 nops", activeTest->testName, currentTest.results[6], activeTest->results[6]);
		doResult("7 nops", activeTest->testName, currentTest.results[7], activeTest->results[7]);
		doResult("8 nops", activeTest->testName, currentTest.results[8], activeTest->results[8]);
		doResult("9 nops", activeTest->testName, currentTest.results[9], activeTest->results[9]);
		results[i] = failed == totalResults - passes;
	}
	activeTestInfo.testId = -1;
}

static size_t listTimerIRQSuite(const char** names, bool* passed, size_t size, size_t offset) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (i + offset >= nTimerIRQTests) {
			break;
		}
		names[i] = timerIRQTests[i + offset].testName;
		passed[i] = results[i + offset];
	}
	return i;
}

static void showTimerIRQSuite(size_t index) {
	const struct TimerIRQTest* activeTest = &timerIRQTests[index];
	struct TimerIRQTest currentTest = {0};
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
			if (resultIndex < 0) {
				++resultIndex;
			}
		}

		if (keys & KEY_B) {
			return;
		}

		printResults(activeTest->testName, &currentTest, activeTest, resultIndex);
		updateTextGrid();
	}
	activeTestInfo.testId = -1;
}

const struct TestSuite timerIRQTestSuite = {
	.name = "Timer IRQ tests",
	.run = runTimerIRQSuite,
	.list = listTimerIRQSuite,
	.show = showTimerIRQSuite,
	.nTests = sizeof(timerIRQTests) / sizeof(*timerIRQTests),
	.passes = &passes,
	.totalResults = &totalResults
};
