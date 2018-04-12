// EM Includes
#include <types.h>
#include <macros.h>
#include <em_qpn.h>
#include <bsp.h>
#include <gpio.h>
#include <platform.h>
#include <pml.h>

// Standard Includes
#include <string.h>

// Stack Includes
#include <BleBase.h>
#include <BleGap.h>
#include <BleEngine.h>
#include <AciCoreCallback.h>  // Enable vendor specific ACI commands

typedef struct
{
    // Root structure for all state machines.
    QActive super;

    // Handler for the Bluetooth Low Energy stack.
    BleHandler handler;
} Example_t;

typedef enum
{
    BEACON_BLE_INIT_SIG = EM_USER_SIG,
    BEACON_SIG_COUNT
} BeaconSignals_t;

// This variable represents the task.
static Example_t gBeaconTask;

// This variable stores all signals being sent to the task.
static SECTION_NONPERSISTENT QEvt gBeaconEventQueue[5];

QState Example_Init(Example_t *me)
{
    QState status;

    // Unused pins must be properly set to avoid significant current draw due
    // to floating pins.
    //
    // For all unsed GPIO, disable the input, output, and pullup function, and
    // enable the pulldown.  If an external pull is applied to the pin, then the
    // internal pullup and pulldown should both be disabled.  In this example,
    // GPIO5, 6, and 7 are not used.
    GPIO_DisableInput(5);
    GPIO_DisableOutput(5);
    GPIO_SetOutputPinFunction(5, GPIO_PIN_FUNC_OUT_GPIO);
    GPIO_DisablePullUp(5);
    GPIO_EnablePullDown(5);

    GPIO_DisableInput(6);
    GPIO_DisableOutput(6);
    GPIO_SetOutputPinFunction(6, GPIO_PIN_FUNC_OUT_GPIO);
    GPIO_DisablePullUp(6);
    GPIO_EnablePullDown(6);

    GPIO_DisableInput(7);
    GPIO_DisableOutput(7);
    GPIO_SetOutputPinFunction(7, GPIO_PIN_FUNC_OUT_GPIO);
    GPIO_DisablePullUp(7);
    GPIO_EnablePullDown(7);

    // Set the Bluetooth stack core callback.
    me->handler.callback = Example_CoreCallback;

    // Initialize the Bluetooth stack and register the callback.
    BleStatus bleStatus = BLEMGMT_InitWithHandler(&me->handler);

    if (BLESTATUS_SUCCESS == bleStatus || BLESTATUS_PENDING == bleStatus )
    {
        // Now wait for the stack to finish initialization.
        status = Q_TRAN(Example_WaitForStack);
    }
    else
    {
        // There was an error.
        status = Q_TRAN(Example_Error);
    }

    return status;
}

void Example_Constructor(Example_t *me)
{
    // Tell QP-nano what the initial state is.
    QActive_ctor(&me->super, Q_STATE_CAST(Example_Init));
}

void Example_EntryFunction(void)
{
    // Add the task and the task's event queue to QP-nano. This must always
    // occur during boot and resume from sleep because the event queue is not
    // in retention memory.
    (void)QF_addTask(&gBeaconTask.super, &gBeaconEventQueue[0],
        Q_DIM(gBeaconEventQueue));

    // Override configuration settings in lieu of adding a configuration
    // patch to the OTP.  The configuration settings must be updated
    // during the boot and resume from sleep process.  Configuration
    // settings are stored in nonpersistent memory to minimize
    // persistent memory use.  The configuration settings are re-applied
    // during the resume from sleep process.
    Platform_Configuration_t *pPlatform_Config =
        (Platform_Configuration_t*)MODIFY_CONST(&gPlatform_Config);

    pPlatform_Config->bleIsStackRequested = 1;

    // Setup the task after power on reset.
    if(!PML_DidBootFromSleep())
    {
        // Construct our new QP-nano task.  The task constructor is only called
        // during boot since the retention data is retained during sleep mode.
        Example_Constructor(&gBeaconTask);
    }
}

ENTRY_FUNCTION(Example_EntryFunction);
