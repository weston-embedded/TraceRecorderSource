/*
 * Trace Recorder for Tracealyzer v4.10.2
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Tracealyzer support for Cs/OS2
 */

#ifndef TRC_KERNEL_PORT_H
#define TRC_KERNEL_PORT_H

#include <trcDefines.h>
#include <cpu_core.h>
#include <os_cfg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_USE_TRACEALYZER_RECORDER (TRC_CFG_USE_TRACEALYZER_RECORDER) /* Allows for disabling the recorder */

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

extern uint32_t uiTraceTickCount;
extern volatile uint32_t uiTraceSystemState;

#undef TRC_CFG_ENABLE_STACK_MONITOR
#define TRC_CFG_ENABLE_STACK_MONITOR 0

/*** Don't change the below definitions, unless you know what you are doing! ***/

#define TRACE_KERNEL_VERSION 0x2AA2

#undef TRC_TICK_RATE_HZ
#undef TRC_HWTC_TYPE
#undef TRC_HWTC_COUNT
#undef TRC_HWTC_PERIOD
#undef TRC_HWTC_DIVISOR
#undef TRC_HWTC_FREQ_HZ

#define TRC_TICK_RATE_HZ    OS_TICKS_PER_SEC
#define TRC_HWTC_TYPE       TRC_FREE_RUNNING_32BIT_INCR
#define TRC_HWTC_COUNT      CPU_TS_TmrRd()
#define TRC_HWTC_PERIOD     1
#define TRC_HWTC_DIVISOR    1
#define TRC_HWTC_FREQ_HZ    CPU_TS_TmrFreq_Hz

/**
 * @def TRACE_CPU_CLOCK_HZ
 * @brief Trace CPU clock speed in Hz.
 */
#define TRACE_CPU_CLOCK_HZ TRC_CFG_CPU_CLOCK_HZ

#if (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC)
#include <stdlib.h> /* Include malloc() */

/**
 * @internal Kernel port specific heap initialization
 */
#define TRC_KERNEL_PORT_HEAP_INIT(size)

/**
 * @internal Kernel port specific heap malloc definition
 */
#define TRC_KERNEL_PORT_HEAP_MALLOC(size) malloc(size)
#endif

#define TRC_PLATFORM_CFG "Cs-OS2"
#define TRC_PLATFORM_CFG_MAJOR 2
#define TRC_PLATFORM_CFG_MINOR 93
#define TRC_PLATFORM_CFG_PATCH 7

#ifndef TRACE_ENTER_CRITICAL_SECTION
    #error "This hardware port has no definition for critical sections! See http://percepio.com/2014/10/27/how-to-define-critical-sections-for-the-recorder/"
#endif

#define TRC_KERNEL_PORT_ALLOC_CRITICAL_SECTION()    CPU_SR cpu_sr;
#define TRC_KERNEL_PORT_ENTER_CRITICAL_SECTION()    CPU_CRITICAL_ENTER()
#define TRC_KERNEL_PORT_EXIT_CRITICAL_SECTION()     CPU_CRITICAL_EXIT()

#define NOT_SET 1

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
#define TRC_KERNEL_PORT_BUFFER_SIZE (sizeof(TraceTaskHandle_t) * (TRC_CFG_CORE_COUNT))
#else
#define TRC_KERNEL_PORT_BUFFER_SIZE (sizeof(TraceUnsignedBaseType_t) * (TRC_CFG_CORE_COUNT))
#endif

/**
 * @internal The kernel port data buffer
 */
typedef struct TraceKernelPortDataBuffer    /* Aligned */
{
    uint8_t buffer[TRC_KERNEL_PORT_BUFFER_SIZE];
} TraceKernelPortDataBuffer_t;

/**
 * @internal Initializes the kernel port
 *
 * @param[in] pxBuffer Kernel port data buffer
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t* const pxBuffer);

/**
 * @internal Enables the kernel port
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortEnable(void);

/**
 * @internal Not used for BareMetal
 *
 * @param[in] uiTicks Tick count to delay
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceKernelPortDelay(uiTicks) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2((void)uiTicks, TRC_SUCCESS)

/**
 * @internal Query if scheduler is suspended. For Bare Metal systems this will always be false.
 *
 * @retval 1 Scheduler suspended
 * @retval 0 Scheduler not suspended
 */
#define xTraceKernelPortIsSchedulerSuspended() (0U)

/******************************************************************************/
/*** Definitions for Streaming mode *******************************************/
/******************************************************************************/
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

/*******************************************************************************
* vTraceStoreKernelObjectName
*
* Set the name for a kernel object (defined by its address).
******************************************************************************/
void vTraceStoreKernelObjectName(void* object, const char* name);

/*******************************************************************************
 * prvTraceOnBegin
 *
 * Called on trace begin.
 ******************************************************************************/
void prvTraceOnBegin(void);

/*******************************************************************************
 * prvTraceOnEnd
 *
 * Called on trace end.
 ******************************************************************************/
void prvTraceOnEnd(void);

/*************************************************************************/
/* KERNEL SPECIFIC OBJECT CONFIGURATION                                  */
/*************************************************************************/

/*******************************************************************************
 * The event codes - should match the offline config file.
 ******************************************************************************/

/*** Event codes for streaming - should match the Tracealyzer config file *****/
#define PSF_EVENT_NULL_EVENT                                0x000UL

#define PSF_EVENT_TRACE_START                               0x001UL
#define PSF_EVENT_TS_CONFIG                                 0x002UL /* Unused */
#define PSF_EVENT_OBJ_NAME                                  0x003UL
#define PSF_EVENT_TASK_PRIORITY                             0x004UL
#define PSF_EVENT_DEFINE_ISR                                0x005UL
#define PSF_EVENT_UNUSED_STACK                              0x006UL

#define PSF_EVENT_IFE_NEXT                                  0x008UL
#define PSF_EVENT_IFE_DIRECT                                0x009UL

#define PSF_EVENT_TASK_CREATE                               0x010UL
#define PSF_EVENT_TASK_DELETE                               0x011UL
#define PSF_EVENT_PROCESS_CREATE                            0x012UL /* Unused */
#define PSF_EVENT_PROCESS_DELETE                            0x013UL /* Unused */
#define PSF_EVENT_THREAD_CREATE                             0x014UL /* Unused */
#define PSF_EVENT_THREAD_DELETE                             0x015UL /* Unused */

#define PSF_EVENT_TASK_READY                                0x020UL
#define PSF_EVENT_ISR_BEGIN                                 0x021UL
#define PSF_EVENT_ISR_RESUME                                0x022UL
#define PSF_EVENT_TS_BEGIN                                  0x023UL /* Unused */
#define PSF_EVENT_TS_RESUME                                 0x024UL /* Unused */
#define PSF_EVENT_TASK_ACTIVATE                             0x025UL

#define PSF_EVENT_MALLOC                                    0x030UL
#define PSF_EVENT_FREE                                      0x031UL
#define PSF_EVENT_MALLOC_FAILED                             0x032UL
#define PSF_EVENT_FREE_FAILED                               0x033UL

#define PSF_EVENT_LOWPOWER_BEGIN                            0x038UL /* Unused */
#define PSF_EVENT_LOWPOWER_END                              0x039UL /* Unused */

#define PSF_EVENT_STATEMACHINE_STATE_CREATE                 0x040UL
#define PSF_EVENT_STATEMACHINE_CREATE                       0x041UL
#define PSF_EVENT_STATEMACHINE_STATECHANGE                  0x042UL

#define PSF_EVENT_INTERVAL_CHANNEL_CREATE                   0x043UL
#define PSF_EVENT_INTERVAL_START                            0x044UL
#define PSF_EVENT_INTERVAL_STOP                             0x045UL
#define PSF_EVENT_INTERVAL_CHANNEL_SET_CREATE               0x046UL

#define PSF_EVENT_EXTENSION_CREATE                          0x047UL

#define PSF_EVENT_HEAP_CREATE                               0x048UL

#define PSF_EVENT_COUNTER_CREATE                            0x049UL
#define PSF_EVENT_COUNTER_CHANGE                            0x04AUL
#define PSF_EVENT_COUNTER_LIMIT_EXCEEDED                    0x04BUL

#define PSF_EVENT_DEPENDENCY_REGISTER                       0x04CUL

#define PSF_EVENT_RUNNABLE_REGISTER                         0x04DUL
#define PSF_EVENT_RUNNABLE_START                            0x04EUL
#define PSF_EVENT_RUNNABLE_STOP                             0x04FUL

#define PSF_EVENT_USER_EVENT                                0x050UL

#define PSF_EVENT_USER_EVENT_FIXED                          0x058UL

#define PSF_EVENT_NEW_TIME                                  0x100UL
#define PSF_EVENT_NEW_TIME_SCHEDULER_SUSPENDED              0x101UL  /* Unused */

#define PSF_EVENT_FLAG_CREATE                               0x102UL
#define PSF_EVENT_FLAG_DELETE                               0x103UL
#define PSF_EVENT_FLAG_POST                                 0x104UL
#define PSF_EVENT_FLAG_POST_FAILED                          0x105UL
#define PSF_EVENT_FLAG_PEND                                 0x106UL
#define PSF_EVENT_FLAG_PEND_FAILED                          0x107UL
#define PSF_EVENT_FLAG_PEND_BLOCK                           0x108UL
#define PSF_EVENT_FLAG_DEL_ENTER                            0x109UL
#define PSF_EVENT_FLAG_POST_ENTER                           0x10AUL
#define PSF_EVENT_FLAG_PEND_ENTER                           0x10BUL
#define PSF_EVENT_FLAG_DEL_EXIT                             0x10CUL
#define PSF_EVENT_FLAG_POST_EXIT                            0x10DUL
#define PSF_EVENT_FLAG_PEND_EXIT                            0x10EUL

#define PSF_EVENT_MBOX_CREATE                               0x10FUL
#define PSF_EVENT_MBOX_DELETE                               0x110UL
#define PSF_EVENT_MBOX_POST                                 0x111UL
#define PSF_EVENT_MBOX_POST_FAILED                          0x112UL
#define PSF_EVENT_MBOX_PEND                                 0x113UL
#define PSF_EVENT_MBOX_PEND_FAILED                          0x114UL
#define PSF_EVENT_MBOX_PEND_BLOCK                           0x115UL
#define PSF_EVENT_MBOX_DEL_ENTER                            0x116UL
#define PSF_EVENT_MBOX_POST_ENTER                           0x117UL
#define PSF_EVENT_MBOX_POST_OPT_ENTER                       0x118UL
#define PSF_EVENT_MBOX_PEND_ENTER                           0x119UL
#define PSF_EVENT_MBOX_DEL_EXIT                             0x11AUL
#define PSF_EVENT_MBOX_POST_EXIT                            0x11BUL
#define PSF_EVENT_MBOX_POST_OPT_EXIT                        0x11CUL
#define PSF_EVENT_MBOX_PEND_EXIT                            0x11DUL

#define PSF_EVENT_MEM_CREATE                                0x11EUL
#define PSF_EVENT_MEM_PUT                                   0x11FUL
#define PSF_EVENT_MEM_PUT_FAILED                            0x120UL
#define PSF_EVENT_MEM_GET                                   0x121UL
#define PSF_EVENT_MEM_GET_FAILED                            0x122UL
#define PSF_EVENT_MEM_PUT_ENTER                             0x123UL
#define PSF_EVENT_MEM_GET_ENTER                             0x124UL
#define PSF_EVENT_MEM_PUT_EXIT                              0x125UL
#define PSF_EVENT_MEM_GET_EXIT                              0x126UL

#define PSF_EVENT_MUTEX_CREATE                              0x127UL
#define PSF_EVENT_MUTEX_DELETE                              0x128UL
#define PSF_EVENT_MUTEX_POST                                0x129UL
#define PSF_EVENT_MUTEX_POST_FAILED                         0x12AUL
#define PSF_EVENT_MUTEX_PEND                                0x12BUL
#define PSF_EVENT_MUTEX_PEND_FAILED                         0x12CUL
#define PSF_EVENT_MUTEX_PEND_BLOCK                          0x12DUL
#define PSF_EVENT_MUTEX_DEL_ENTER                           0x12EUL
#define PSF_EVENT_MUTEX_POST_ENTER                          0x12FUL
#define PSF_EVENT_MUTEX_PEND_ENTER                          0x130UL
#define PSF_EVENT_MUTEX_DEL_EXIT                            0x131UL
#define PSF_EVENT_MUTEX_POST_EXIT                           0x132UL
#define PSF_EVENT_MUTEX_PEND_EXIT                           0x133UL

#define PSF_EVENT_Q_CREATE                                  0x134UL
#define PSF_EVENT_Q_DELETE                                  0x135UL
#define PSF_EVENT_Q_POST                                    0x136UL
#define PSF_EVENT_Q_POST_FAILED                             0x137UL
#define PSF_EVENT_Q_POST_FRONT                              0x138UL
#define PSF_EVENT_Q_POST_FRONT_FAILED                       0x139UL
#define PSF_EVENT_Q_PEND                                    0x13AUL
#define PSF_EVENT_Q_PEND_FAILED                             0x13BUL
#define PSF_EVENT_Q_PEND_BLOCK                              0x13CUL
#define PSF_EVENT_Q_FLUSH                                   0x13DUL
#define PSF_EVENT_Q_DEL_ENTER                               0x13EUL
#define PSF_EVENT_Q_POST_ENTER                              0x13FUL
#define PSF_EVENT_Q_POST_FRONT_ENTER                        0x140UL
#define PSF_EVENT_Q_POST_OPT_ENTER                          0x141UL
#define PSF_EVENT_Q_PEND_ENTER                              0x142UL
#define PSF_EVENT_Q_DEL_EXIT                                0x143UL
#define PSF_EVENT_Q_POST_EXIT                               0x144UL
#define PSF_EVENT_Q_POST_FRONT_EXIT                         0x145UL
#define PSF_EVENT_Q_POST_OPT_EXIT                           0x146UL
#define PSF_EVENT_Q_PEND_EXIT                               0x147UL

#define PSF_EVENT_SEM_CREATE                                0x148UL
#define PSF_EVENT_SEM_DELETE                                0x149UL
#define PSF_EVENT_SEM_POST                                  0x14AUL
#define PSF_EVENT_SEM_POST_FAILED                           0x14BUL
#define PSF_EVENT_SEM_PEND                                  0x14CUL
#define PSF_EVENT_SEM_PEND_FAILED                           0x14DUL
#define PSF_EVENT_SEM_PEND_BLOCK                            0x14EUL
#define PSF_EVENT_SEM_ACCEPT                                0x14FUL
#define PSF_EVENT_SEM_SET                                   0x150UL
#define PSF_EVENT_SEM_SET_FAILED                            0x151UL
#define PSF_EVENT_SEM_DEL_ENTER                             0x152UL
#define PSF_EVENT_SEM_POST_ENTER                            0x153UL
#define PSF_EVENT_SEM_PEND_ENTER                            0x154UL
#define PSF_EVENT_SEM_DEL_EXIT                              0x155UL
#define PSF_EVENT_SEM_POST_EXIT                             0x156UL
#define PSF_EVENT_SEM_PEND_EXIT                             0x157UL

#define PSF_EVENT_TASK_PRIO_INHERIT                         0x158UL
#define PSF_EVENT_TASK_PRIO_DISINHERIT                      0x159UL
#define PSF_EVENT_TASK_CREATE_FAILED                        0x15AUL
#define PSF_EVENT_TASK_DELAY                                0x15BUL
#define PSF_EVENT_TASK_RESUME                               0x15CUL
#define PSF_EVENT_TASK_SUSPEND                              0x15DUL

#define PSF_EVENT_TMR_CREATE                                0x15EUL
#define PSF_EVENT_TMR_DELETE                                0x15FUL
#define PSF_EVENT_TMR_EXPIRED                               0x160UL
#define PSF_EVENT_TMR_DEL_ENTER                             0x161UL
#define PSF_EVENT_TMR_DEL_EXIT                              0x162UL

#define TRC_EVENT_LAST_ID                                  (PSF_EVENT_TMR_DEL_EXIT)

/*** The trace macros for streaming ******************************************/
                                                                /* Trace Recorder Control Macros                        */
#undef OS_TRACE_INIT
#define OS_TRACE_INIT()                             xTraceInitialize()

#undef OS_TRACE_START
#define OS_TRACE_START()                            xTraceEnable(TRC_START)

#undef OS_TRACE_STOP
#define OS_TRACE_STOP()                             xTraceDisable()

#undef OS_TRACE_CLEAR
#define OS_TRACE_CLEAR()

                                                                /* ISR Trace Macros                                     */
#undef OS_TRACE_ISR_REGISTER
#define OS_TRACE_ISR_REGISTER(isr_name, isr_prio) \
    vTraceSetISRProperties(isr_name, isr_prio);

#undef OS_TRACE_ISR_BEGIN
#define OS_TRACE_ISR_BEGIN(isr_name) \
    vTraceStoreISRBegin(isr_name);

#undef OS_TRACE_ISR_END
#define OS_TRACE_ISR_END(flag) \
    vTraceStoreISREnd(flag);

#undef OS_TRACE_ISR_ENTER
#define OS_TRACE_ISR_ENTER()

#undef OS_TRACE_ISR_EXIT
#define OS_TRACE_ISR_EXIT()

#undef OS_TRACE_ISR_EXIT_TO_SCHEDULER
#define OS_TRACE_ISR_EXIT_TO_SCHEDULER()

                                                                /* Core Trace Macros                                    */
/* Called for each task that becomes ready */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_READY
#if (TRC_CFG_INCLUDE_READY_EVENTS == 1)
#define OS_TRACE_TASK_READY(p_tcb) \
    xTraceTaskReady(p_tcb);
#else /* (TRC_CFG_INCLUDE_READY_EVENTS == 1) */
#define OS_TRACE_TASK_READY(p_tcb)
#endif /* (TRC_CFG_INCLUDE_READY_EVENTS == 1) */

#undef OS_TRACE_TASK_NOT_READY
#define OS_TRACE_TASK_NOT_READY(p_tcb)

/* Called on each task-switch */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SWITCHED_IN
#define OS_TRACE_TASK_SWITCHED_IN(p_tcb) \
    xTraceTaskSwitch(p_tcb, p_tcb->OSTCBPrio)

                                                                /* Event Flag Trace Macros                              */
/* Called from OSFlagCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_CREATE
#define OS_TRACE_FLAG_CREATE(p_grp, p_name) \
    xTraceObjectRegisterWithoutHandle(PSF_EVENT_FLAG_CREATE, p_grp, (CPU_CHAR *)p_name, 0);

/* Called on OSFlagDel */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_DEL
#define OS_TRACE_FLAG_DEL(p_grp) \
    xTraceObjectUnregisterWithoutHandle(PSF_EVENT_FLAG_DELETE, (void *)p_grp, 0);

/* Called when a signal is sent to a flag group */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_POST
#define OS_TRACE_FLAG_POST(p_grp) \
    xTraceEventCreate1(PSF_EVENT_FLAG_POST, (uint32_t)p_grp);

/* Called when a message failed to be sent to a flag group (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_POST_FAILED
#define OS_TRACE_FLAG_POST_FAILED(p_grp) \
    xTraceEventCreate1(PSF_EVENT_FLAG_POST_FAILED, (uint32_t)p_grp);

/* Called when a signal is received from a flag group */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_PEND
#define OS_TRACE_FLAG_PEND(p_grp) \
    xTraceEventCreate1(PSF_EVENT_FLAG_PEND, (uint32_t)p_grp);

/* Called when a signal is received from a flag group */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_PEND_FAILED
#define OS_TRACE_FLAG_PEND_FAILED(p_grp) \
    xTraceEventCreate1(PSF_EVENT_FLAG_PEND_FAILED, (uint32_t)p_grp);

/* Called when a signal is received from a flag group */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_PEND_BLOCK
#define OS_TRACE_FLAG_PEND_BLOCK(p_grp) \
    xTraceEventCreate1(PSF_EVENT_FLAG_PEND_BLOCK, (uint32_t)p_grp);

#if (OS_TRACE_API_ENTER_EN == 1u)
#undef OS_TRACE_FLAG_DEL_ENTER
#define  OS_TRACE_FLAG_DEL_ENTER(p_grp, opt) \
    xTraceEventCreate2(PSF_EVENT_FLAG_DEL_ENTER, (uint32_t)p_grp, (uint32_t)opt);

#undef OS_TRACE_FLAG_POST_ENTER
#define  OS_TRACE_FLAG_POST_ENTER(p_grp, flags, opt) \
    xTraceEventCreate3(PSF_EVENT_FLAG_POST_ENTER, (uint32_t)p_grp, (uint32_t)flags, (uint32_t)opt);

#undef OS_TRACE_FLAG_PEND_ENTER
#define  OS_TRACE_FLAG_PEND_ENTER(p_grp, flags, timeout, opt) \
    xTraceEventCreate4(PSF_EVENT_FLAG_PEND_ENTER, (uint32_t)p_grp, (uint32_t)flags, (uint32_t)timeout, (uint32_t)opt);
#endif

#if (OS_TRACE_API_EXIT_EN == 1u)
#undef OS_TRACE_FLAG_DEL_EXIT
#define  OS_TRACE_FLAG_DEL_EXIT(p_grp, err) \
    if (err != OS_ERR_NONE) { \
        xTraceEventCreate2(PSF_EVENT_FLAG_DEL_EXIT, (uint32_t)p_grp, (uint32_t)err); \
    }

#undef OS_TRACE_FLAG_POST_EXIT
#define  OS_TRACE_FLAG_POST_EXIT(p_grp, err) \
    xTraceEventCreate2(PSF_EVENT_FLAG_POST_EXIT, (uint32_t)p_grp, (uint32_t)err);

#undef OS_TRACE_FLAG_PEND_EXIT
#define  OS_TRACE_FLAG_PEND_EXIT(p_grp, err) \
    xTraceEventCreate2(PSF_EVENT_FLAG_PEND_EXIT, (uint32_t)p_grp, (uint32_t)err);
#endif

                                                                /* Mailbox Trace Macros                                 */
#undef OS_TRACE_MBOX_CREATE
#define OS_TRACE_MBOX_CREATE(p_event, p_name) \
    xTraceObjectRegisterWithoutHandle(PSF_EVENT_MBOX_CREATE, p_event, (CPU_CHAR *)p_name, 0);

#undef OS_TRACE_MBOX_DEL
#define OS_TRACE_MBOX_DEL(p_event) \
    xTraceObjectUnregisterWithoutHandle(PSF_EVENT_MBOX_DELETE, (void *)p_event, 0);

#undef OS_TRACE_MBOX_POST
#define OS_TRACE_MBOX_POST(p_event) \
    xTraceEventCreate1(PSF_EVENT_MBOX_POST, (uint32_t)p_event);

#undef OS_TRACE_MBOX_POST_FAILED
#define OS_TRACE_MBOX_POST_FAILED(p_event) \
    xTraceEventCreate1(PSF_EVENT_MBOX_POST_FAILED, (uint32_t)p_event);

#undef OS_TRACE_MBOX_PEND
#define OS_TRACE_MBOX_PEND(p_event) \
    xTraceEventCreate1(PSF_EVENT_MBOX_PEND, (uint32_t)p_event);

#undef OS_TRACE_MBOX_PEND_FAILED
#define OS_TRACE_MBOX_PEND_FAILED(p_event) \
    xTraceEventCreate1(PSF_EVENT_MBOX_PEND_FAILED, (uint32_t)p_event);

#undef OS_TRACE_MBOX_PEND_BLOCK
#define OS_TRACE_MBOX_PEND_BLOCK(p_event) \
    xTraceEventCreate1(PSF_EVENT_MBOX_PEND_BLOCK, (uint32_t)p_event);

#if (OS_TRACE_API_ENTER_EN == 1u)
#undef OS_TRACE_MBOX_DEL_ENTER
#define OS_TRACE_MBOX_DEL_ENTER(p_event, opt) \
    xTraceEventCreate2(PSF_EVENT_MBOX_DEL_ENTER, (uint32_t)p_event, (uint32_t)opt);

#undef OS_TRACE_MBOX_POST_ENTER
#define OS_TRACE_MBOX_POST_ENTER(p_event) \
    xTraceEventCreate1(PSF_EVENT_MBOX_POST_ENTER, (uint32_t)p_event);

#undef OS_TRACE_MBOX_POST_OPT_ENTER
#define OS_TRACE_MBOX_POST_OPT_ENTER(p_event, opt) \
    xTraceEventCreate2(PSF_EVENT_MBOX_POST_OPT_ENTER, (uint32_t)p_event, (uint32_t)opt);

#undef OS_TRACE_MBOX_PEND_ENTER
#define OS_TRACE_MBOX_PEND_ENTER(p_event, timeout) \
    xTraceEventCreate2(PSF_EVENT_MBOX_PEND_ENTER, (uint32_t)p_event, (uint32_t)timeout);
#endif

#if (OS_TRACE_API_EXIT_EN == 1u)
#undef OS_TRACE_MBOX_DEL_EXIT
#define OS_TRACE_MBOX_DEL_EXIT(p_event, err) \
    if (err != OS_ERR_NONE) { \
        xTraceEventCreate2(PSF_EVENT_MBOX_DEL_EXIT, (uint32_t)p_event, (uint32_t)err); \
    }

#undef OS_TRACE_MBOX_POST_EXIT
#define OS_TRACE_MBOX_POST_EXIT(p_event, err) \
    xTraceEventCreate2(PSF_EVENT_MBOX_POST_EXIT, (uint32_t)p_event, (uint32_t)err);

#undef OS_TRACE_MBOX_POST_OPT_EXIT
#define OS_TRACE_MBOX_POST_OPT_EXIT(p_event, err) \
    xTraceEventCreate2(PSF_EVENT_MBOX_POST_OPT_EXIT, (uint32_t)p_event, (uint32_t)err);

#undef OS_TRACE_MBOX_PEND_EXIT
#define OS_TRACE_MBOX_PEND_EXIT(p_event, err) \
    xTraceEventCreate2(PSF_EVENT_MBOX_PEND_EXIT, (uint32_t)p_event, (uint32_t)err);
#endif

                                                                /* Memory Partition Trace Macros                        */
/* Called from OSMemCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_CREATE
#define OS_TRACE_MEM_CREATE(p_mem) \
    xTraceObjectRegisterWithoutHandle(PSF_EVENT_MEM_CREATE, p_mem, 0, (uint8_t)(p_mem->OSMemNBlks));

/* Called when a signal is sent to a memory partition */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_PUT
#define OS_TRACE_MEM_PUT(p_mem) \
    xTraceEventCreate1(PSF_EVENT_MEM_PUT, (uint32_t)p_mem);

/* Called when a message failed to be sent to a memory partition (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_PUT_FAILED
#define OS_TRACE_MEM_PUT_FAILED(p_mem) \
    xTraceEventCreate1(PSF_EVENT_MEM_PUT_FAILED, (uint32_t)p_mem);

/* Called when a signal is received from a memory partition */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_GET
#define OS_TRACE_MEM_GET(p_mem) \
    xTraceEventCreate1(PSF_EVENT_MEM_GET, (uint32_t)p_mem);

/* Called when a signal is received from a memory partition */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_GET_FAILED
#define OS_TRACE_MEM_GET_FAILED(p_mem) \
    xTraceEventCreate1(PSF_EVENT_MEM_GET_FAILED, (uint32_t)p_mem);

#if (OS_TRACE_API_ENTER_EN == 1u)
#undef OS_TRACE_MEM_PUT_ENTER
#define OS_TRACE_MEM_PUT_ENTER(p_mem, p_blk) \
    xTraceEventCreate2(PSF_EVENT_MEM_PUT_ENTER, (uint32_t)p_mem, (uint32_t)p_blk);

#undef OS_TRACE_MEM_GET_ENTER
#define OS_TRACE_MEM_GET_ENTER(p_mem) \
    xTraceEventCreate1(PSF_EVENT_MEM_GET_ENTER, (uint32_t)p_mem);
#endif

#if (OS_TRACE_API_EXIT_EN == 1u)
#undef OS_TRACE_MEM_PUT_EXIT
#define OS_TRACE_MEM_PUT_EXIT(p_mem, err) \
    xTraceEventCreate2(PSF_EVENT_MEM_PUT_EXIT, (uint32_t)p_mem, (uint32_t)err);

#undef OS_TRACE_MEM_GET_EXIT
#define OS_TRACE_MEM_GET_EXIT(p_mem, err) \
    xTraceEventCreate2(PSF_EVENT_MEM_GET_EXIT, (uint32_t)p_mem, (uint32_t)err);
#endif

                                                                /* Mutex Trace Macros                                   */
/* Called from OSMutexCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_CREATE
#define OS_TRACE_MUTEX_CREATE(p_event, p_name) \
    xTraceObjectRegisterWithoutHandle(PSF_EVENT_MUTEX_CREATE, p_event, (CPU_CHAR *)p_name, 0);

/* Called on OSMutexDel */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_DEL
#define OS_TRACE_MUTEX_DEL(p_event) \
    xTraceObjectUnregisterWithoutHandle(PSF_EVENT_MUTEX_DELETE, (void *)p_event, 0);

/* Called when a signal is sent to a mutex */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_POST
#define OS_TRACE_MUTEX_POST(p_event) \
    xTraceEventCreate1(PSF_EVENT_MUTEX_POST, (uint32_t)p_event);

/* Called when a message failed to be sent to a mutex (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_POST_FAILED
#define OS_TRACE_MUTEX_POST_FAILED(p_event) \
    xTraceEventCreate1(PSF_EVENT_MUTEX_POST_FAILED, (uint32_t)p_event);

/* Called when a signal is received from a mutex */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_PEND
#define OS_TRACE_MUTEX_PEND(p_event) \
    xTraceEventCreate1(PSF_EVENT_MUTEX_PEND, (uint32_t)p_event);

/* Called when a signal is received from a mutex */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_PEND_FAILED
#define OS_TRACE_MUTEX_PEND_FAILED(p_event) \
    xTraceEventCreate1(PSF_EVENT_MUTEX_PEND_FAILED, (uint32_t)p_event);

/* Called when a signal is received from a mutex */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_PEND_BLOCK
#define OS_TRACE_MUTEX_PEND_BLOCK(p_event) \
    xTraceEventCreate1(PSF_EVENT_MUTEX_PEND_BLOCK, (uint32_t)p_event);

/* Called in by mutex operations, when the task's priority is changed to the one from the mutex's owner */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_TASK_PRIO_INHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_INHERIT(p_tcb, prio) \
	xTraceEventCreate2(PSF_EVENT_TASK_PRIO_INHERIT, (uint32_t)p_tcb, prio);

/* Called in by mutex operations, when the task's priority is changed back to its original */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb, prio) \
	xTraceEventCreate2(PSF_EVENT_TASK_PRIO_DISINHERIT, (uint32_t)p_tcb, prio);

#if (OS_TRACE_API_ENTER_EN == 1u)
#undef OS_TRACE_MUTEX_DEL_ENTER
#define OS_TRACE_MUTEX_DEL_ENTER(p_event, opt) \
	xTraceEventCreate2(PSF_EVENT_MUTEX_DEL_ENTER, (uint32_t)p_event, (uint32_t)opt);

#undef OS_TRACE_MUTEX_POST_ENTER
#define OS_TRACE_MUTEX_POST_ENTER(p_event) \
	xTraceEventCreate1(PSF_EVENT_MUTEX_POST_ENTER, (uint32_t)p_event);

#undef OS_TRACE_MUTEX_PEND_ENTER
#define OS_TRACE_MUTEX_PEND_ENTER(p_event, timeout) \
	xTraceEventCreate2(PSF_EVENT_MUTEX_PEND_ENTER, (uint32_t)p_event, (uint32_t)timeout);
#endif

#if (OS_TRACE_API_EXIT_EN == 1u)
#undef OS_TRACE_MUTEX_DEL_EXIT
#define OS_TRACE_MUTEX_DEL_EXIT(p_event, err) \
    if (err != OS_ERR_NONE) { \
        xTraceEventCreate2(PSF_EVENT_MUTEX_DEL_EXIT, (uint32_t)p_event, (uint32_t)err); \
    }

#undef OS_TRACE_MUTEX_POST_EXIT
#define OS_TRACE_MUTEX_POST_EXIT(p_event, err) \
	xTraceEventCreate2(PSF_EVENT_MUTEX_POST_EXIT, (uint32_t)p_event, (uint32_t)err);

#undef OS_TRACE_MUTEX_PEND_EXIT
#define OS_TRACE_MUTEX_PEND_EXIT(p_event, err) \
	xTraceEventCreate2(PSF_EVENT_MUTEX_PEND_EXIT, (uint32_t)p_event, (uint32_t)err);
#endif

                                                                /* Message Queue Trace Macros                           */
/* Called from os_q.c. */
/* No need to protect critical section. */
#undef OS_TRACE_Q_CREATE
#define OS_TRACE_Q_CREATE(p_event, p_name) \
    xTraceObjectRegisterWithoutHandle(PSF_EVENT_Q_CREATE, p_event, (CPU_CHAR *)p_name, ((OS_Q *)(pevent->OSEventPtr))->OSQSize);

/* Called on OSQDel */
/* No need to protect critical section. */
#undef OS_TRACE_Q_DEL
#define OS_TRACE_Q_DEL(p_event) \
    xTraceObjectUnregisterWithoutHandle(PSF_EVENT_Q_DELETE, (void *)p_event, 0);

/* Called when a signal is sent to a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_POST
#define OS_TRACE_Q_POST(p_event)  \
    xTraceEventCreate2(PSF_EVENT_Q_POST, (uint32_t)p_event, (uint8_t)(((OS_Q *)p_event->OSEventPtr)->OSQEntries));

/* Called when a message failed to be sent to a queue (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_Q_POST_FAILED
#define OS_TRACE_Q_POST_FAILED(p_event) \
    xTraceEventCreate2(PSF_EVENT_Q_POST_FAILED, (uint32_t)p_event, (uint8_t)(((OS_Q *)p_event->OSEventPtr)->OSQEntries));

/* Called when a signal is sent to a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_POST_FRONT
#define OS_TRACE_Q_POST_FRONT(p_event)  \
    xTraceEventCreate2(PSF_EVENT_Q_POST_FRONT, (uint32_t)p_event, (uint8_t)(((OS_Q *)p_event->OSEventPtr)->OSQEntries));

/* Called when a signal is sent to a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_POST_FRONT_FAILED
#define OS_TRACE_Q_POST_FRONT_FAILED(p_event)  \
    xTraceEventCreate2(PSF_EVENT_Q_POST_FRONT_FAILED, (uint32_t)p_event, (uint8_t)(((OS_Q *)p_event->OSEventPtr)->OSQEntries));

/* Called when a signal is sent to a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_POST_OPT
#define OS_TRACE_Q_POST_OPT(p_event, opt)  \
    do { \
        if ((opt & OS_POST_OPT_FRONT) != 0x00u) { \
            xTraceEventCreate2(PSF_EVENT_Q_POST_FRONT, (uint32_t)p_event, (uint8_t)(((OS_Q *)p_event->OSEventPtr)->OSQEntries)); \
        } else { \
            xTraceEventCreate2(PSF_EVENT_Q_POST, (uint32_t)p_event, (uint8_t)(((OS_Q *)p_event->OSEventPtr)->OSQEntries)); \
        } \
    } while (0);

#undef OS_TRACE_Q_POST_OPT_FAILED
#define OS_TRACE_Q_POST_OPT_FAILED(p_event, opt)  \
    do { \
        if ((opt & OS_POST_OPT_FRONT) != 0x00u) { \
            xTraceEventCreate2(PSF_EVENT_Q_POST_FRONT_FAILED, (uint32_t)p_event, (uint8_t)(((OS_Q *)p_event->OSEventPtr)->OSQEntries)); \
        } else { \
            xTraceEventCreate2(PSF_EVENT_Q_POST_FAILED, (uint32_t)p_event, (uint8_t)(((OS_Q *)p_event->OSEventPtr)->OSQEntries)); \
        } \
    } while (0);

/* Called when a message is received from a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_PEND
#define OS_TRACE_Q_PEND(p_event) \
    xTraceEventCreate2(PSF_EVENT_Q_PEND, (uint32_t)p_event, (uint8_t)(((OS_Q *)p_event->OSEventPtr)->OSQEntries));

/* Called when a message is received from a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_PEND_FAILED
#define OS_TRACE_Q_PEND_FAILED(p_event) \
    xTraceEventCreate2(PSF_EVENT_Q_PEND_FAILED, (uint32_t)p_event, (uint8_t)(((OS_Q *)p_event->OSEventPtr)->OSQEntries));

/* Called when a message is received from a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_PEND_BLOCK
#define OS_TRACE_Q_PEND_BLOCK(p_event) \
    xTraceEventCreate2(PSF_EVENT_Q_PEND_BLOCK, (uint32_t)p_event, (uint8_t)(((OS_Q *)p_event->OSEventPtr)->OSQEntries));

/* Called when the queue is flushed */
/* No need to protect critical section. */
#undef OS_TRACE_Q_FLUSH
#define OS_TRACE_Q_FLUSH(p_event) \
    xTraceEventCreate2(PSF_EVENT_Q_FLUSH, (uint32_t)p_event, (uint8_t)(((OS_Q *)p_event->OSEventPtr)->OSQEntries));

#if (OS_TRACE_API_ENTER_EN == 1u)
#undef OS_TRACE_Q_DEL_ENTER
#define OS_TRACE_Q_DEL_ENTER(p_event, opt) \
	xTraceEventCreate2(PSF_EVENT_Q_DEL_ENTER, (uint32_t)p_event, (uint32_t)opt);

#undef OS_TRACE_Q_POST_ENTER
#define OS_TRACE_Q_POST_ENTER(p_event) \
	xTraceEventCreate1(PSF_EVENT_Q_POST_ENTER, (uint32_t)p_event);

#undef OS_TRACE_Q_POST_FRONT_ENTER
#define OS_TRACE_Q_POST_FRONT_ENTER(p_event) \
	xTraceEventCreate1(PSF_EVENT_Q_POST_FRONT_ENTER, (uint32_t)p_event);

#undef OS_TRACE_Q_POST_OPT_ENTER
#define OS_TRACE_Q_POST_OPT_ENTER(p_event, opt) \
	xTraceEventCreate2(PSF_EVENT_Q_POST_OPT_ENTER, (uint32_t)p_event, (uint32_t)opt);

#undef OS_TRACE_Q_PEND_ENTER
#define OS_TRACE_Q_PEND_ENTER(p_event, timeout) \
	xTraceEventCreate2(PSF_EVENT_Q_PEND_ENTER, (uint32_t)p_event, (uint32_t)timeout);
#endif

#if (OS_TRACE_API_EXIT_EN == 1u)
#undef OS_TRACE_Q_DEL_EXIT
#define OS_TRACE_Q_DEL_EXIT(p_event, err) \
    if (err != OS_ERR_NONE) { \
    	xTraceEventCreate2(PSF_EVENT_Q_DEL_EXIT, (uint32_t)p_event, (uint32_t)err); \
    }

#undef OS_TRACE_Q_POST_EXIT
#define OS_TRACE_Q_POST_EXIT(p_event, err) \
	xTraceEventCreate2(PSF_EVENT_Q_POST_EXIT, (uint32_t)p_event, (uint32_t)err);

#undef OS_TRACE_Q_POST_FRONT_EXIT
#define OS_TRACE_Q_POST_FRONT_EXIT(p_event, err) \
	xTraceEventCreate2(PSF_EVENT_Q_POST_FRONT_EXIT, (uint32_t)p_event, (uint32_t)err);

#undef OS_TRACE_Q_POST_OPT_EXIT
#define OS_TRACE_Q_POST_OPT_EXIT(p_event, err) \
	xTraceEventCreate2(PSF_EVENT_Q_POST_OPT_EXIT, (uint32_t)p_event, (uint32_t)err);

#undef OS_TRACE_Q_PEND_EXIT
#define OS_TRACE_Q_PEND_EXIT(p_event, err) \
	xTraceEventCreate2(PSF_EVENT_Q_PEND_EXIT, (uint32_t)p_event, (uint32_t)err);
#endif

                                                                /* Semaphore Trace Macros                               */
/* Called from OSSemCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_CREATE
#define OS_TRACE_SEM_CREATE(p_event, p_name) \
    xTraceObjectRegisterWithoutHandle(PSF_EVENT_SEM_CREATE, p_event, (CPU_CHAR *)p_name, p_event->OSEventCnt);

/* Called on OSSemDel */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_DEL
#define OS_TRACE_SEM_DEL(p_event) \
    xTraceObjectUnregisterWithoutHandle(PSF_EVENT_SEM_DELETE, (void *)p_event, 0);

/* Called when a signal is sent to a semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_POST
#define OS_TRACE_SEM_POST(p_event) \
    xTraceEventCreate2(PSF_EVENT_SEM_POST, (uint32_t)p_event, (uint8_t)(p_event->OSEventCnt));

/* Called when a signal failed to be sent to a semaphore (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_POST_FAILED
#define OS_TRACE_SEM_POST_FAILED(p_event) \
    xTraceEventCreate2(PSF_EVENT_SEM_POST_FAILED, (uint32_t)p_event, (uint8_t)(p_event->OSEventCnt));

/* Called when a message is received from a semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_PEND
#define OS_TRACE_SEM_PEND(p_event) \
    xTraceEventCreate2(PSF_EVENT_SEM_PEND, (uint32_t)p_event, (uint8_t)(p_event->OSEventCnt));

/* Called when a message is received from a semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_PEND_FAILED
#define OS_TRACE_SEM_PEND_FAILED(p_event) \
    xTraceEventCreate2(PSF_EVENT_SEM_PEND_FAILED, (uint32_t)p_event, (uint8_t)(p_event->OSEventCnt));

/* Called when a message is received from a semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_PEND_BLOCK
#define OS_TRACE_SEM_PEND_BLOCK(p_event) \
    xTraceEventCreate2(PSF_EVENT_SEM_PEND_BLOCK, (uint32_t)p_event, (uint8_t)(p_event->OSEventCnt));

/* Called from OSSemAccept. */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_ACCEPT
#define OS_TRACE_SEM_ACCEPT(p_event) \
    xTraceEventCreate2(PSF_EVENT_SEM_ACCEPT, (uint32_t)p_event, (uint8_t)(p_event->OSEventCnt));

/* Called when setting the semaphore to a specified value */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_SET
#define OS_TRACE_SEM_SET(p_event) \
    xTraceEventCreate2(PSF_EVENT_SEM_SET, (uint32_t)p_event, (uint8_t)(p_event->OSEventCnt));

/* Called when setting the semaphore to a specified value */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_SET_FAILED
#define OS_TRACE_SEM_SET_FAILED(p_event) \
    xTraceEventCreate2(PSF_EVENT_SEM_SET_FAILED, (uint32_t)p_event, (uint8_t)(p_event->OSEventCnt));

#if (OS_TRACE_API_ENTER_EN == 1u)
#undef OS_TRACE_SEM_DEL_ENTER
#define OS_TRACE_SEM_DEL_ENTER(p_event, opt) \
	xTraceEventCreate2(PSF_EVENT_SEM_DEL_ENTER, (uint32_t)p_event, (uint32_t)opt);

#undef OS_TRACE_SEM_POST_ENTER
#define OS_TRACE_SEM_POST_ENTER(p_event) \
	xTraceEventCreate1(PSF_EVENT_SEM_POST_ENTER, (uint32_t)p_event);

#undef OS_TRACE_SEM_PEND_ENTER
#define OS_TRACE_SEM_PEND_ENTER(p_event, timeout) \
	xTraceEventCreate2(PSF_EVENT_SEM_PEND_ENTER, (uint32_t)p_event, (uint32_t)timeout);
#endif

#if (OS_TRACE_API_EXIT_EN == 1u)
#undef OS_TRACE_SEM_DEL_EXIT
#define OS_TRACE_SEM_DEL_EXIT(p_event, err) \
    if (err != OS_ERR_NONE) { \
    	xTraceEventCreate2(PSF_EVENT_SEM_DEL_EXIT, (uint32_t)p_event, (uint32_t)err); \
    }

#undef OS_TRACE_SEM_POST_EXIT
#define OS_TRACE_SEM_POST_EXIT(p_event, err) \
	xTraceEventCreate2(PSF_EVENT_SEM_POST_EXIT, (uint32_t)p_event, (uint32_t)err);

#undef OS_TRACE_SEM_PEND_EXIT
#define OS_TRACE_SEM_PEND_EXIT(p_event, err) \
	xTraceEventCreate2(PSF_EVENT_SEM_PEND_EXIT, (uint32_t)p_event, (uint32_t)err);
#endif

                                                                /* Task Management Trace Macros                         */
/* Called in OSTaskCreate */
/* Need to protect critical section without reallocating the SR.                     */
#undef OS_TRACE_TASK_CREATE
#define OS_TRACE_TASK_CREATE(p_tcb) \
	TRACE_ENTER_CRITICAL_SECTION(); \
    xTraceTaskRegisterWithoutHandle((void *)(p_tcb), (CPU_CHAR *)(p_tcb)->OSTCBTaskName, (p_tcb)->OSTCBPrio); \
	TRACE_EXIT_CRITICAL_SECTION();

/* Called in OSTaskCreate, if it fails (typically if the stack can not be allocated) */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_CREATE_FAILED
#define OS_TRACE_TASK_CREATE_FAILED(p_tcb) \
    xTraceEventCreate0(PSF_EVENT_TASK_CREATE_FAILED);

/* Called on OSTaskDel */
/* No need to protect critical section. */
/* If this macro give you problem try increase optimization. */
#undef OS_TRACE_TASK_DEL
#define OS_TRACE_TASK_DEL(p_tcb) \
    (void)xTraceTaskUnregisterWithoutHandle(p_tcb, (p_tcb)->OSTCBPrio)

/* Called on OSTimeDly */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_DLY
#define OS_TRACE_TASK_DLY(dly_ticks) \
    xTraceEventCreate1(PSF_EVENT_TASK_DELAY, dly_ticks);

#undef OS_TRACE_TASK_RESUME
#define OS_TRACE_TASK_RESUME(p_tcb) \
    xTraceEventCreate1(PSF_EVENT_TASK_RESUME, (uint32_t)p_tcb);

#undef OS_TRACE_TASK_SUSPEND
#define OS_TRACE_TASK_SUSPEND(p_tcb) \
    xTraceEventCreate1(PSF_EVENT_TASK_SUSPEND, (uint32_t)p_tcb);

/* Called in OSTaskChangePrio */
#undef OS_TRACE_TASK_PRIO_CHANGE
#define OS_TRACE_TASK_PRIO_CHANGE(p_tcb, prio) \
    xTraceTaskSetPriorityWithoutHandle(p_tcb, prio);

#undef OS_TRACE_TASK_NAME_SET
#define OS_TRACE_TASK_NAME_SET(p_tcb) \
    xTraceTaskSetNameWithoutHandle(p_tcb, (CPU_CHAR *)p_tcb->OSTCBTaskName);

#undef OS_TRACE_EVENT_NAME_SET
#define OS_TRACE_EVENT_NAME_SET(p_event, p_name) \
    xTraceObjectSetNameWithoutHandle(p_event, (CPU_CHAR *)p_name);

                                                                /* Tick Trace Macros                                     */
/* No need to protect critical section. */
#undef OS_TRACE_TICK_INCREMENT
#if (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1)
#define OS_TRACE_TICK_INCREMENT(ctr) \
    xTraceEventCreate1(PSF_EVENT_NEW_TIME, ctr);
#else /* (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1) */
#define OS_TRACE_TICK_INCREMENT(ctr)
#endif /* (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1) */

                                                                /* Tmr Trace Macros                                     */
#undef OS_TRACE_TMR_CREATE
#define OS_TRACE_TMR_CREATE(p_tmr, p_name, dly, period) \
    xTraceObjectRegisterWithoutHandle2(PSF_EVENT_TMR_CREATE, p_tmr, (CPU_CHAR *)p_name, dly, period);

#undef  OS_TRACE_TMR_CREATE_FAILED
#define  OS_TRACE_TMR_CREATE_FAILED(p_tmr) \
    xTraceEventCreate0(PSF_EVENT_TMR_CREATE_FAILED);

#undef OS_TRACE_TMR_DEL
#define OS_TRACE_TMR_DEL(p_tmr) \
    xTraceObjectUnregisterWithoutHandle(PSF_EVENT_TMR_DELETE, (void *)p_tmr, 0);

#undef OS_TRACE_TMR_EXPIRED
#define  OS_TRACE_TMR_EXPIRED(p_tmr) \
    xTraceEventCreate1(PSF_EVENT_TMR_EXPIRED, (uint32_t)p_tmr);

#undef OS_TRACE_TMR_DEL_ENTER
#define OS_TRACE_TMR_DEL_ENTER(p_tmr) \
    xTraceEventCreate1(PSF_EVENT_TMR_DEL_ENTER, (uint32_t)p_tmr);

#undef OS_TRACE_TMR_DEL_EXIT
#define OS_TRACE_TMR_DEL_EXIT(p_tmr, err) \
    if (err != OS_ERR_NONE) { \
        xTraceEventCreate2(PSF_EVENT_TMR_DEL_EXIT, (uint32_t)p_tmr, (uint32_t)err); \
    }


#if (TRC_CFG_SCHEDULING_ONLY == 1)
/* SCHEDULING ONLY, DEFINE ALL AS EMPTY */

#if 0
#undef OS_TRACE_TASK_READY
#define OS_TRACE_TASK_READY(p_tcb)

#undef OS_TRACE_TASK_NOT_READY
#define OS_TRACE_TASK_NOT_READY(p_tcb)

#undef OS_TRACE_TASK_SWITCHED_IN
#define OS_TRACE_TASK_SWITCHED_IN(p_tcb)
#endif

#undef OS_TRACE_FLAG_CREATE
#define OS_TRACE_FLAG_CREATE(p_grp, p_name)

#undef OS_TRACE_FLAG_DEL
#define OS_TRACE_FLAG_DEL(p_grp)

#undef OS_TRACE_FLAG_POST
#define OS_TRACE_FLAG_POST(p_grp)

#undef OS_TRACE_FLAG_POST_FAILED
#define OS_TRACE_FLAG_POST_FAILED(p_grp)

#undef OS_TRACE_FLAG_PEND
#define OS_TRACE_FLAG_PEND(p_grp)

#undef OS_TRACE_FLAG_PEND_FAILED
#define OS_TRACE_FLAG_PEND_FAILED(p_grp)

#undef OS_TRACE_FLAG_PEND_BLOCK
#define OS_TRACE_FLAG_PEND_BLOCK(p_grp)

#undef OS_TRACE_FLAG_DEL_ENTER
#define OS_TRACE_FLAG_DEL_ENTER(p_grp, opt)

#undef OS_TRACE_FLAG_POST_ENTER
#define OS_TRACE_FLAG_POST_ENTER(p_grp, flags, opt)

#undef OS_TRACE_FLAG_PEND_ENTER
#define OS_TRACE_FLAG_PEND_ENTER(p_grp, flags, timeout, opt)

#undef OS_TRACE_FLAG_DEL_EXIT
#define OS_TRACE_FLAG_DEL_EXIT(p_grp, err)

#undef OS_TRACE_FLAG_POST_EXIT
#define OS_TRACE_FLAG_POST_EXIT(p_grp, err)

#undef OS_TRACE_FLAG_PEND_EXIT
#define OS_TRACE_FLAG_PEND_EXIT(p_grp, err)

#undef OS_TRACE_MBOX_CREATE
#define OS_TRACE_MBOX_CREATE(p_event, p_name)

#undef OS_TRACE_MBOX_DEL
#define OS_TRACE_MBOX_DEL(p_event)

#undef OS_TRACE_MBOX_POST
#define OS_TRACE_MBOX_POST(p_event)

#undef OS_TRACE_MBOX_POST_FAILED
#define OS_TRACE_MBOX_POST_FAILED(p_event)

#undef OS_TRACE_MBOX_PEND
#define OS_TRACE_MBOX_PEND(p_event)

#undef OS_TRACE_MBOX_PEND_FAILED
#define OS_TRACE_MBOX_PEND_FAILED(p_event)

#undef OS_TRACE_MBOX_PEND_BLOCK
#define OS_TRACE_MBOX_PEND_BLOCK(p_event)

#undef OS_TRACE_MBOX_DEL_ENTER
#define OS_TRACE_MBOX_DEL_ENTER(p_event, opt)

#undef OS_TRACE_MBOX_POST_ENTER
#define OS_TRACE_MBOX_POST_ENTER(p_event)

#undef OS_TRACE_MBOX_POST_OPT_ENTER
#define OS_TRACE_MBOX_POST_OPT_ENTER(p_event, opt)

#undef OS_TRACE_MBOX_PEND_ENTER
#define OS_TRACE_MBOX_PEND_ENTER(p_event, timeout)

#undef OS_TRACE_MBOX_DEL_EXIT
#define OS_TRACE_MBOX_DEL_EXIT(p_event, err)

#undef OS_TRACE_MBOX_POST_EXIT
#define OS_TRACE_MBOX_POST_EXIT(p_event, err)

#undef OS_TRACE_MBOX_POST_OPT_EXIT
#define OS_TRACE_MBOX_POST_OPT_EXIT(p_event, err)

#undef OS_TRACE_MBOX_PEND_EXIT
#define OS_TRACE_MBOX_PEND_EXIT(p_event, err)

#undef OS_TRACE_MEM_CREATE
#define OS_TRACE_MEM_CREATE(p_mem)

#undef OS_TRACE_MEM_PUT
#define OS_TRACE_MEM_PUT(p_mem)

#undef OS_TRACE_MEM_PUT_FAILED
#define OS_TRACE_MEM_PUT_FAILED(p_mem)

#undef OS_TRACE_MEM_GET
#define OS_TRACE_MEM_GET(p_mem)

#undef OS_TRACE_MEM_GET_FAILED
#define OS_TRACE_MEM_GET_FAILED(p_mem)

#undef OS_TRACE_MEM_PUT_ENTER
#define OS_TRACE_MEM_PUT_ENTER(p_mem, p_blk)

#undef OS_TRACE_MEM_GET_ENTER
#define OS_TRACE_MEM_GET_ENTER(p_mem)

#undef OS_TRACE_MEM_PUT_EXIT
#define OS_TRACE_MEM_PUT_EXIT(p_mem, err)

#undef OS_TRACE_MEM_GET_EXIT
#define OS_TRACE_MEM_GET_EXIT(p_mem, err)

#undef OS_TRACE_MUTEX_CREATE
#define OS_TRACE_MUTEX_CREATE(p_event, p_name)

#undef OS_TRACE_MUTEX_DEL
#define OS_TRACE_MUTEX_DEL(p_event)

#undef OS_TRACE_MUTEX_POST
#define OS_TRACE_MUTEX_POST(p_event)

#undef OS_TRACE_MUTEX_POST_FAILED
#define OS_TRACE_MUTEX_POST_FAILED(p_event)

#undef OS_TRACE_MUTEX_PEND
#define OS_TRACE_MUTEX_PEND(p_event)

#undef OS_TRACE_MUTEX_PEND_FAILED
#define OS_TRACE_MUTEX_PEND_FAILED(p_event)

#undef OS_TRACE_MUTEX_PEND_BLOCK
#define OS_TRACE_MUTEX_PEND_BLOCK(p_event)

#undef OS_TRACE_MUTEX_TASK_PRIO_INHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_INHERIT(p_tcb, prio)

#undef OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb, prio)

#undef OS_TRACE_MUTEX_DEL_ENTER
#define OS_TRACE_MUTEX_DEL_ENTER(p_event, opt)

#undef OS_TRACE_MUTEX_POST_ENTER
#define OS_TRACE_MUTEX_POST_ENTER(p_event)

#undef OS_TRACE_MUTEX_PEND_ENTER
#define OS_TRACE_MUTEX_PEND_ENTER(p_event, timeout)

#undef OS_TRACE_MUTEX_DEL_EXIT
#define OS_TRACE_MUTEX_DEL_EXIT(p_event, err)

#undef OS_TRACE_MUTEX_POST_EXIT
#define OS_TRACE_MUTEX_POST_EXIT(p_event, err)

#undef OS_TRACE_MUTEX_PEND_EXIT
#define OS_TRACE_MUTEX_PEND_EXIT(p_event, err)

#undef OS_TRACE_Q_CREATE
#define OS_TRACE_Q_CREATE(p_event, p_name)

#undef OS_TRACE_Q_DEL
#define OS_TRACE_Q_DEL(p_event)

#undef OS_TRACE_Q_POST
#define OS_TRACE_Q_POST(p_event)

#undef OS_TRACE_Q_POST_FAILED
#define OS_TRACE_Q_POST_FAILED(p_event)

#undef OS_TRACE_Q_POST_FRONT
#define OS_TRACE_Q_POST_FRONT(p_event)

#undef OS_TRACE_Q_POST_FRONT_FAILED
#define OS_TRACE_Q_POST_FRONT_FAILED(p_event)

#undef OS_TRACE_Q_POST_OPT
#define OS_TRACE_Q_POST_OPT(p_event, opt)

#undef OS_TRACE_Q_POST_OPT_FAILED
#define OS_TRACE_Q_POST_OPT_FAILED(p_event, opt)

#undef OS_TRACE_Q_PEND
#define OS_TRACE_Q_PEND(p_event)

#undef OS_TRACE_Q_PEND_FAILED
#define OS_TRACE_Q_PEND_FAILED(p_event)

#undef OS_TRACE_Q_PEND_BLOCK
#define OS_TRACE_Q_PEND_BLOCK(p_event)

#undef OS_TRACE_Q_FLUSH
#define OS_TRACE_Q_FLUSH(p_event)

#undef OS_TRACE_Q_DEL_ENTER
#define OS_TRACE_Q_DEL_ENTER(p_event, opt)

#undef OS_TRACE_Q_POST_ENTER
#define OS_TRACE_Q_POST_ENTER(p_event)

#undef OS_TRACE_Q_POST_FRONT_ENTER
#define OS_TRACE_Q_POST_FRONT_ENTER(p_event)

#undef OS_TRACE_Q_POST_OPT_ENTER
#define OS_TRACE_Q_POST_OPT_ENTER(p_event, opt)

#undef OS_TRACE_Q_PEND_ENTER
#define OS_TRACE_Q_PEND_ENTER(p_event, timeout)

#undef OS_TRACE_Q_DEL_EXIT
#define OS_TRACE_Q_DEL_EXIT(p_event, err)

#undef OS_TRACE_Q_POST_EXIT
#define OS_TRACE_Q_POST_EXIT(p_event, err)

#undef OS_TRACE_Q_POST_FRONT_EXIT
#define OS_TRACE_Q_POST_FRONT_EXIT(p_event, err)

#undef OS_TRACE_Q_POST_OPT_EXIT
#define OS_TRACE_Q_POST_OPT_EXIT(p_event, err)

#undef OS_TRACE_Q_PEND_EXIT
#define OS_TRACE_Q_PEND_EXIT(p_event, err)

#undef OS_TRACE_SEM_CREATE
#define OS_TRACE_SEM_CREATE(p_event, p_name)

#undef OS_TRACE_SEM_DEL
#define OS_TRACE_SEM_DEL(p_event)

#undef OS_TRACE_SEM_POST
#define OS_TRACE_SEM_POST(p_event)

#undef OS_TRACE_SEM_POST_FAILED
#define OS_TRACE_SEM_POST_FAILED(p_event)

#undef OS_TRACE_SEM_PEND
#define OS_TRACE_SEM_PEND(p_event)

#undef OS_TRACE_SEM_PEND_FAILED
#define OS_TRACE_SEM_PEND_FAILED(p_event)

#undef OS_TRACE_SEM_PEND_BLOCK
#define OS_TRACE_SEM_PEND_BLOCK(p_event)

#undef OS_TRACE_SEM_SET
#define OS_TRACE_SEM_SET(p_event)

#undef OS_TRACE_SEM_SET_FAILED
#define OS_TRACE_SEM_SET_FAILED(p_event)

#undef OS_TRACE_SEM_DEL_ENTER
#define OS_TRACE_SEM_DEL_ENTER(p_event, opt)

#undef OS_TRACE_SEM_POST_ENTER
#define OS_TRACE_SEM_POST_ENTER(p_event)

#undef OS_TRACE_SEM_PEND_ENTER
#define OS_TRACE_SEM_PEND_ENTER(p_event, timeout)

#undef OS_TRACE_SEM_DEL_EXIT
#define OS_TRACE_SEM_DEL_EXIT(p_event, err)

#undef OS_TRACE_SEM_POST_EXIT
#define OS_TRACE_SEM_POST_EXIT(p_event, err)

#undef OS_TRACE_SEM_PEND_EXIT
#define OS_TRACE_SEM_PEND_EXIT(p_event, err)

#if 0
#undef OS_TRACE_TASK_CREATE
#define OS_TRACE_TASK_CREATE(p_tcb)

#undef OS_TRACE_TASK_CREATE_FAILED
#define OS_TRACE_TASK_CREATE_FAILED(p_tcb)

#undef OS_TRACE_TASK_DEL
#define OS_TRACE_TASK_DEL(p_tcb)
#endif

#undef OS_TRACE_TASK_DLY
#define OS_TRACE_TASK_DLY(dly_ticks)

#undef OS_TRACE_TASK_SUSPEND
#define OS_TRACE_TASK_SUSPEND(p_tcb)

#undef OS_TRACE_TASK_RESUME
#define OS_TRACE_TASK_RESUME(p_tcb)

#undef OS_TRACE_TASK_PRIO_CHANGE
#define OS_TRACE_TASK_PRIO_CHANGE(p_tcb, prio)

#undef OS_TRACE_TASK_NAME_SET
#define OS_TRACE_TASK_NAME_SET(p_tcb)

#undef OS_TRACE_EVENT_NAME_SET
#define OS_TRACE_EVENT_NAME_SET(p_event, p_name)

#if 0
#undef OS_TRACE_TICK_INCREMENT
#define OS_TRACE_TICK_INCREMENT(ctr)
#endif

#undef OS_TRACE_TMR_CREATE
#define OS_TRACE_TMR_CREATE(p_tmr, p_name, dly, period)

#undef OS_TRACE_TMR_EXPIRED
#define OS_TRACE_TMR_EXPIRED(p_tmr)

#undef OS_TRACE_TMR_DEL_ENTER
#define OS_TRACE_TMR_DEL_ENTER(p_tmr)

#undef OS_TRACE_TMR_START_ENTER
#define OS_TRACE_TMR_START_ENTER(p_tmr)

#undef OS_TRACE_TMR_STOP_ENTER
#define OS_TRACE_TMR_STOP_ENTER(p_tmr)

#undef OS_TRACE_TMR_DEL_EXIT
#define OS_TRACE_TMR_DEL_EXIT(p_tmr, err)

#undef OS_TRACE_TMR_START_EXIT
#define OS_TRACE_TMR_START_EXIT(p_tmr, err)

#undef OS_TRACE_TMR_STOP_EXIT
#define OS_TRACE_TMR_STOP_EXIT(p_tmr, err)

#endif /* (TRC_CFG_SCHEDULING_ONLY == 0) */

#else
#error "Only TRC_RECORDER_MODE_STREAMING is supported."
#endif /* TRC_RECORDER_MODE_STREAMING */

#endif /* TRC_USE_TRACEALYZER_RECORDER */

#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_H */
