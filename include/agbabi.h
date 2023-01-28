/*
===============================================================================

 C header file for agbabi

 Copyright (C) 2021-2023 agbabi contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef AGBABI_H
#define AGBABI_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * Empty IRQ handler that acknowledges raised IRQs
 */
void __agbabi_irq_empty() __attribute__((error("IRQ handler should not be directly called")));

/**
 * Nested IRQ handler that calls __agbabi_irq_user_fn with the raised IRQ flags
 */
void __agbabi_irq_user() __attribute__((error("IRQ handler should not be directly called")));

/**
 * Handler called by __agbabi_irq_user
 * @param irqFlags 16-bit mask of the raised flags
 */
extern void(*__agbabi_irq_user_fn)(int irqFlags);

/**
 * Copies n bytes from src to dest (forward)
 * Assumes dest and src are 2-byte aligned
 * @param dest Destination address
 * @param src Source address
 * @param n Number of bytes to copy
 */
void __agbabi_memcpy2(void *__restrict__ dest, const void *__restrict__ src, size_t n) __attribute__((nonnull(1, 2)));

/**
 * Copies n bytes from src to dest (forward)
 * This is a slow, unaligned, byte-by-byte copy: ideal for SRAM
 * @param dest Destination address
 * @param src Source address
 * @param n Number of bytes to copy
 */
void __agbabi_memcpy1(void *__restrict__ dest, const void *__restrict__ src, size_t n) __attribute__((nonnull(1, 2)));

/**
 * Fills dest with n bytes of c
 * Assumes dest is 4-byte aligned
 * Trailing copy uses the low byte of c
 * @param dest Destination address
 * @param n Number of bytes to set
 * @param c Value to set
 */
void __agbabi_wordset4(void *dest, size_t n, int c) __attribute__((nonnull(1)));

/**
 * Fills dest with n bytes of c
 * Assumes dest is 4-byte aligned
 * Trailing copy uses the low word of c, and the low byte of c
 * @param dest Destination address
 * @param n Number of bytes to set
 * @param c Value to set
 */
void __agbabi_lwordset4(void *dest, size_t n, long long c) __attribute__((nonnull(1)));

/**
 * Copies n bytes from src to dest (backwards)
 * @param dest Destination address
 * @param src Source address
 * @param n Number of bytes to copy
 */
void __agbabi_rmemcpy(void *__restrict__ dest, const void *__restrict__ src, size_t n) __attribute__((nonnull(1, 2)));

/**
 * Copies n bytes from src to dest (backwards)
 * This is a slow, unaligned, byte-by-byte copy: ideal for SRAM
 * @param dest Destination address
 * @param src Source address
 * @param n Number of bytes to copy
 */
void __agbabi_rmemcpy1(void *__restrict__ dest, const void *__restrict__ src, size_t n) __attribute__((nonnull(1, 2)));

/**
 * Unsigned 64-bit / 32-bit -> 64-bit division
 * @param numerator
 * @param denominator
 * @return quotient
 */
unsigned long long __agbabi_uluidiv(unsigned long long numerator, unsigned int denominator) __attribute__((const));

/**
 * Fixed-point sine approximation
 * @param x 16-bit binary angle measurement
 * @return 32-bit signed fixed point (Q29) between -1 and +1
 */
int __agbabi_sin(int x) __attribute__((const));

/**
 * Calculates the integer square root of x
 * @param x
 * @return Square root of x
 */
int __agbabi_sqrt(unsigned int x) __attribute__((const));

/**
 * Calculates the arc tangent of x, y
 * @param x 1.14 signed fixed-point coord around circle
 * @param y 1.14 signed fixed-point coord around circle
 * @return 15-bit binary angle measurement
 */
unsigned int __agbabi_atan2(int x, int y) __attribute__((const));

/**
 * Copies n bytes from src to dest (forward) using FIQ mode
 * Assumes dest and src are 4-byte aligned
 * @param dest Destination address
 * @param src Source address
 * @param n Number of bytes to copy
 */
void __agbabi_fiq_memcpy4(void *__restrict__ dest, const void *__restrict__ src, size_t n) __attribute__((nonnull(1, 2)));

/**
 * Copies n bytes in multiples of 16 bytes from src to dest (forward) using FIQ mode
 * Assumes dest and src are 4-byte aligned
 * @param dest Destination address
 * @param src Source address
 * @param n Number of bytes to copy, must be a multiple of 16
 */
void __agbabi_fiq_memcpy4x4(void *__restrict__ dest, const void *__restrict__ src, size_t n) __attribute__((nonnull(1, 2)));

typedef struct __agbabi_coro_t {
    unsigned int arm_sp : 31;
    unsigned int joined : 1;
} __agbabi_coro_t;

/**
 * Initializes a coro struct to call a given coroutine
 * @param coro pointer to coro struct to initialize
 * @param sp_top the TOP of the stack for this coroutine (stack grows down!)
 * @param coproc procedure to call as a coroutine
 */
void __agbabi_coro_make(__agbabi_coro_t* __restrict__ coro, void* __restrict__ sp_top, int(*coproc)(__agbabi_coro_t*)) __attribute__((nonnull(1, 2, 3)));

/**
 * Starts/resumes a given coroutine
 * @param coro coroutine to start/resume
 * @return integer value from coroutine
 */
int __agbabi_coro_resume(__agbabi_coro_t* coro) __attribute__((nonnull(1)));

/**
 * Yields a given value of a coroutine back to caller
 * @param coro coroutine that is yielding
 * @param value returned to caller
 */
void __agbabi_coro_yield(__agbabi_coro_t* coro, int value) __attribute__((nonnull(1)));

/**
 * Initialize GPIO pins for RTC
 * @return __agbabi_rtc_err_t error code (0 for success)
 */
int __agbabi_rtc_init();

/**
 * Get the current, raw time from the RTC
 * If RTC is in TEST mode: agbagbi_rtc_TEST will be set
 * @return Raw time in BCD
 */
unsigned int __agbabi_rtc_time();

/**
 * Set the Hour, Minute, Second
 * @param time raw BCD (second, minute, hour)
 */
void __agbabi_rtc_settime(unsigned int time);

/**
 * [raw time in BCD, raw date in BCD]
 */
typedef unsigned int __attribute__((vector_size(sizeof(unsigned int) * 2))) __agbabi_datetime_t;

/**
 * Get the current, raw date & time from the RTC
 * @return [raw time in BCD, raw date in BCD]
 */
__agbabi_datetime_t __agbabi_rtc_datetime();

/**
 * Set the time and date
 * @param datetime [raw BCD time, raw BCD date]
 */
void __agbabi_rtc_setdatetime(__agbabi_datetime_t datetime);

/**
 * Unsigned 32-bit division and modulo
 * Check for divide by zero is not performed
 * @param numerator
 * @param denominator
 * @return [quotient, remainder]
 */
unsigned int __attribute__((vector_size(sizeof(unsigned int) * 2))) __agbabi_unsafe_uidivmod(unsigned int numerator, unsigned int denominator) __attribute__((const));

/**
 * Unsigned 64-bit division and modulo
 * Check for divide by zero is not performed
 * @param numerator
 * @param denominator
 * @return [quotient, remainder]
 */
unsigned long long __attribute__((vector_size(sizeof(unsigned long long) * 2))) __agbabi_unsafe_uldivmod(unsigned long long numerator, unsigned long long denominator) __attribute__((const));

/**
 * Unsigned 64-bit / 32-bit -> 64-bit division and modulo
 * @param numerator
 * @param denominator
 * @return [quotient, remainder]
 */
unsigned long long __attribute__((vector_size(sizeof(unsigned long long) * 2))) __agbabi_uluidivmod(unsigned long long numerator, unsigned int denominator) __attribute__((const));

/**
 * Unsigned 64-bit / 32-bit -> 64-bit division and modulo
 * Check for divide by zero is not performed
 * @param numerator
 * @param denominator
 * @return [quotient, remainder]
 */
unsigned long long __attribute__((vector_size(sizeof(unsigned long long) * 2))) __agbabi_unsafe_uluidivmod(unsigned long long numerator, unsigned int denominator) __attribute__((const));

typedef struct {
    const unsigned short *header __attribute__((bounded(sizeof(unsigned short) * 96)));
    const void* begin;
    const void* end;
    int palette;
    int(*clients_connected)(int mask);
    int(*header_progress)(int prog);
    int(*palette_progress)(int mask);
    int(*accept)();
} __agbabi_multiboot_t;

int __agbabi_multiboot(const __agbabi_multiboot_t* param) __attribute__((nonnull(1)));

#ifdef __cplusplus
}
#endif
#endif /* define AGBABI_H */
