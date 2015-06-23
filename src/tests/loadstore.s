#include "macros.s"

TEST_ALL(testLdrh, _(ldrh r2, [sp]), ,)
TEST_ALL(testLdrhNop, _(ldrh r2, [sp] ; nop), ,)
TEST_ALL(testNopLdrh, _(nop ; ldrh r2, [sp]), ,)
TEST_ALL(testStrh, _(strh r3, [sp]), _(ldrh r3, [sp]),)
TEST_ALL(testStrhNop, _(strh r3, [sp] ; nop), _(ldrh r3, [sp]),)
TEST_ALL(testNopStrh, _(nop ; strh r3, [sp]), _(ldrh r3, [sp]),)