
#include "FreeRTOS.h"
#include "task.h"

#if 0
    void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName ) {
	/* The stack space has been exceeded for a task, considering allocating more. */
	printf("\nOut of stack space!\n");
	printf(pcTaskGetName(NULL));
	printf("\n");
	log_printf("\nOut of stack space! Task: %p %s\n", xTask, pcTaskName);
	__disable_irq(); /* Disable global interrupts. */
	vTaskSuspendAll();
//	while(1) { __asm("bkpt #0"); }; // Stop in GUI as if at a breakpoint (if debugging, otherwise loop forever)    
	while(1);
}
#endif

// #ifndef NDEBUG
//  Note: All pvPortMallocs should be checked individually,
//  but we don't expect any to fail,
//  so this can help flag problems in Debug builds.
void vApplicationMallocFailedHook(void) {
    printf("\nMalloc failed!\n");
    printf("\nMalloc failed! Task: %s\n", pcTaskGetName(NULL));
    __disable_irq(); /* Disable global interrupts. */
    vTaskSuspendAll();
    __BKPT(5);
}
// #endif

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide
an implementation of vApplicationGetIdleTaskMemory() to provide the memory that
is used by the Idle task. */
#if (configNUMBER_OF_CORES == 1)
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static – otherwise they will be
    allocated on the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE]
        __attribute__((aligned));

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task’s
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task’s stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
#else
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                            StackType_t ** ppxIdleTaskStackBuffer,
                                            uint32_t * pulIdleTaskStackSize,
                                            BaseType_t xCoreID ) {
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static – otherwise they will be
    allocated on the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB[configNUMBER_OF_CORES];
    static StackType_t uxIdleTaskStack[configNUMBER_OF_CORES][configMINIMAL_STACK_SIZE]
        __attribute__((aligned));

    configASSERT(xCoreID < configNUMBER_OF_CORES);

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task’s
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB[xCoreID];

    /* Pass out the array that will be used as the Idle task’s stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack[xCoreID];

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
#endif 

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so
the application must provide an implementation of
vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize) {
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static – otherwise they will be
    allocated on the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH]
        __attribute__((aligned));

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task’s state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task’s stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
