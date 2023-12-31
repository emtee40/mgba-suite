#include "macros.s"

TEST_ALL(testLdrh, _(ldrh r2, [sp]), ,)
TEST_ALL(testLdrhNop, _(ldrh r2, [sp] ; nop), ,)
TEST_ALL(testNopLdrh, _(nop ; ldrh r2, [sp]), ,)
TEST_ALL(testNopLdrhNop, _(nop ; ldrh r2, [sp] ; nop), ,)
TEST_ALL(testLdrhRom, _(ldrh r2, [r3]), _(ldr r3, =#0x08000000),)
TEST_ALL(testLdrhRomNop, _(ldrh r2, [r3] ; nop), _(ldr r3, =#0x08000000),)
TEST_ALL(testNopLdrhRom, _(nop ; ldrh r2, [r3]), _(ldr r3, =#0x08000000),)
TEST_ALL(testLdr, _(ldr r2, [sp]), ,)
TEST_ALL(testLdrNop, _(ldr r2, [sp] ; nop), ,)
TEST_ALL(testNopLdr, _(nop ; ldr r2, [sp]), ,)
TEST_ALL(testLdrRom, _(ldr r2, [r3]), _(ldr r3, =#0x08000000),)
TEST_ALL(testLdrRomNop, _(ldr r2, [r3] ; nop), _(ldr r3, =#0x08000000),)
TEST_ALL(testNopLdrRom, _(nop ; ldr r2, [r3]), _(ldr r3, =#0x08000000),)
TEST_ALL(testStrh, _(strh r3, [sp]), _(ldrh r3, [sp]),)
TEST_ALL(testStrhNop, _(strh r3, [sp] ; nop), _(ldrh r3, [sp]),)
TEST_ALL(testNopStrh, _(nop ; strh r3, [sp]), _(ldrh r3, [sp]),)
TEST_ALL(testNopStrhNop, _(nop ; strh r3, [sp] ; nop), _(ldrh r3, [sp]),)
TEST_ALL(testLdrStr, _(ldr r2, [sp] ; str r2, [sp]), ,)
TEST_ALL(testLdrLdr, _(ldr r2, [sp] ; ldr r2, [sp]), ,)
TEST_ALL(testLdrRomLdr, _(ldr r2, [r3] ; ldr r2, [sp]), _(ldr r3, =#0x08000000),)
TEST_ALL(testLdrLdrRom, _(ldr r2, [sp] ; ldr r2, [r3]), _(ldr r3, =#0x08000000),)
TEST_ALL(testLdrRomLdrRom, _(ldr r2, [r3] ; ldr r2, [r3]), _(ldr r3, =#0x08000000),)
TEST_ALL(testStrLdr, _(str r3, [sp] ; ldr r3, [sp]), _(ldr r3, [sp]),)
TEST_ALL(testStrStr, _(str r3, [sp] ; str r3, [sp]), _(ldr r3, [sp]),)
