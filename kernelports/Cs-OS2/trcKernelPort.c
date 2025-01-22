/*
 * Trace Recorder for Tracealyzer v4.10.2
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Tracealyzer support for Cs/OS2
 */

#include <os.h>
#include <trcRecorder.h>

#if (defined(TRC_USE_TRACEALYZER_RECORDER) && TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) || (defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0))
uint32_t uiTraceTickCount;

/* TzCtrl task Stack and TCB */
static  OS_STK       TzCtrlStk[TRC_CFG_CTRL_TASK_STACK_SIZE];
static  CPU_BOOLEAN  TzTaskCreated = DEF_FALSE;

/* The TzCtrl task - receives commands from Tracealyzer (start/stop) */
static  void  TzCtrl (void  *p_arg);

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) || (defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)) */

#define TRC_MAX_STRING_APPEND_SIZE 64

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t* const pxBuffer)
{
    return TRC_SUCCESS;
}

traceResult xTraceKernelPortEnable(void)
{
    INT8U  err;


	if (TzTaskCreated == DEF_FALSE)
	{
        /* Creates the TzCtrl task - receives trace commands (start, stop, ...) */
        OSTaskCreateExt( TzCtrl,                                /* Create the control task                              */
                         0,
                        &TzCtrlStk[APP_CFG_STARTUP_TASK_STK_SIZE - 1u],
                         TRC_CFG_CTRL_TASK_PRIORITY,
                         TRC_CFG_CTRL_TASK_PRIORITY,
                        &TzCtrlStk[0u],
                         TRC_CFG_CTRL_TASK_STACK_SIZE,
                         0u,
                        (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
#if OS_TASK_NAME_EN > 0u
    OSTaskNameSet(         TRC_CFG_CTRL_TASK_PRIORITY,
                  (INT8U *)"Tracealyzer Control Task",
                           &err);
#endif

		TzTaskCreated = DEF_TRUE;
	}


	return TRC_SUCCESS;
}

/*******************************************************************************
 * TzCtrl
 *
 * Task used for reporting the stack usage for each task.
 ******************************************************************************/
static void TzCtrl (void  *p_arg)
{
	(void)p_arg;

	while (1)
	{
		xTraceTzCtrl();

		OSTimeDly(TRC_CFG_CTRL_TASK_DELAY);
	}
}

#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/
