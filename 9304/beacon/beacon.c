////////////////////////////////////////////////////////////////////////////////
///
/// @file       patches/beacon/beacon.c
///
/// @project    EM9304 SOC SDK Example
///
/// @brief      Example Bluetooth Low Energy connectable-beacon application.
///             The beacon application advertises with the friendly name of
///             "Example Beacon".
///
///             To create a new application based on this example, follow these
///             steps:
///             1. Create a new application folder under the SDK "projects"
///                folder.
///             2. Copy the example application files to the new application
///                folder.  Only the source files (*.h and *.c) and the
///                CMakelists.txt file need be copied to the new application
///                folder.
///             3. Modify the application specific CMakelists.txt file as
///                instructed in the comment sections of the file.
///             4. Add the application folder to the projects CMakelists.txt
///                located in the [SOC SDK path]/sw/projects folder.
///             5. In the Metaware IDE, press F5 to refresh the files.
///             6. In the Metaware IDE, select Project->Build All.
///
////////////////////////////////////////////////////////////////////////////////
///
/// @copyright Copyright (C) 2015-present EM Microelectronic-US Inc.
/// @cond
///
/// All rights reserved.
///
/// IMPORTANT - PLEASE CAREFULLY READ THE FOLLOWING SOURCE CODE LICENSE
/// AGREEMENT, WHICH IS LEGALLY BINDING.  THE SOURCE CODE MAY BE USED ONLY IF
/// YOU ACCEPT THE TERMS AND CONDITIONS OF THIS SOURCE CODE LICENSE AGREEMENT
/// (hereafter, the Agreement).
///
/// By Using this source code, you: (i) warrant and represent that you have
/// obtained all authorizations and other applicable consents required
/// empowering you toenter into and (ii) agree to be bound by the terms of this
/// Agreement on your own behalf and/or on behalf of your company.  If you do
/// not agree to this Agreement, then you are not permitted to Use this source
/// code, in whole or in part.
///
/// License Grant.  Pursuant to the terms in the accompanying software license
/// agreement and Terms of Use located at:
/// https://forums.emdeveloper.com/emassets/emus_sourcelicense.html and
/// https://forums.emdeveloper.com/emassets/emus_termsofuse.html  (the terms of
/// each are incorporated herein by this reference) and subject to the
/// restrictions, disclaimer and limitation of liability set forth below,
/// EM Microelectronic US Inc. (EM), grants strictly to you, without the right
/// to sublicense, a non-exclusive, non-transferable, revocable, limited
/// worldwide license to use the source code to modify or create derivative
/// works of the software program for the sole purpose of developing object
/// and executable versions that execute solely and exclusively on devices
/// manufactured by or for EM or your products that use or incorporate devices
/// manufactured by or for EM.
///
/// Restriction.  Without limiting any of the foregoing, the name
/// "EM Microelectronic-US Inc." or that of any of the EM Parties (as such term
/// is defined below) must not be Used (as such term is defined below) to
/// endorse or promote products derived from the source code without prior
/// written permission from an authorized representative of EM Microelectronic
/// US Inc. Unless authorized officers of the parties to this Agreement have
/// duly executed a special written contract specifically governing such Use,
/// in no event shall the source code be Used in or with: (i) life-critical
/// medical equipment, products or services, (ii) military, armament, nuclear or
/// aerospace applications or environments; or (iii) automotive applications
/// unless specifically designated by EM as automotive-grade.  In addition, you
/// understand and agree that you remain solely responsible for using your
/// independent analysis, evaluation, and judgment in designing and developing
/// your applications, products and services.
///
/// DISCLAIMER.  THE SOURCE CODE IS PROVIDED "AS IS" WITH ALL FAULTS, WITHOUT
/// ANY SUPPORT. EM MAKES NO WARRANTIES OR REPRESENTATIONS, EXPRESS, IMPLIED OR
/// STATUTORY, INCLUDING ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
/// A PARTICULAR PURPOSE OR USE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS OF
/// RESPONSES, RESULTS, LACK OF NEGLIGENCE AND LACK OF SECURITY.  EM DISCLAIMS
/// ANY WARRANTY OF TITLE, QUIET ENJOYMENT, QUIET POSSESSION, AND NON-
/// INFRINGEMENT OF ANY THIRD PARTY INTELLECTUAL PROPERTY RIGHTS WITH REGARD TO
/// THE SOURCE CODE OR YOUR USE OF THE SOURCE CODE.  THE ENTIRE RISK AS TO THE
/// QUALITY AND PERFORMANCE OF THE SOURCE CODE IS WITH YOU. SHOULD THE SOURCE
/// CODE PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR
/// OR CORRECTION.  NOTHING CONTAINED IN THIS AGREEMENT WILL BE CONSTRUED AS A
/// WARRANTY OR REPRESENTATION BY ANY EM PARTIES TO MAINTAIN PRODUCTION OF ANY
/// EM DEVICE OR OTHER HARDWARE OR SOFTWARE WITH WHICH THE SOURCE CODE MAY BE
/// USED.  NO ORAL OR WRITTEN INFORMATION OR ADVICE GIVEN BY EM OR AN EM
/// AUTHORIZED REPRESENTATIVE WILL CREATE A WARRANTY
///
/// LIMITATION OF LIABILITY.  IN NO EVENT SHALL EM MICROELECTRONIC US INC., ITS
/// PARENT AND ITS AND THEIR RESPECTIVE AFFILIATES, SUBSIDIARIES, LICENSORS,
/// THIRD PARTY PROVIDERS, REPRESENTATIVES, AGENTS AND ASSIGNS (COLLECTIVLEY,
/// EM PARTIES) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
/// PUNITIVE, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
/// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
/// EQUIPMENT, SYSTEMS, SOFTWARE, TECHNOLOGY, SERVICES, GOODS, CONTENT,
/// MATERIALS OR PROFITS; BUSINESS INTERRUPTION OR OTHER ECONOMIC LOSS OR ANY
/// CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF)
/// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
/// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
/// OUT OF THE USE OF THIS SOURCE CODE, INCLUDING ANY PORTION(S) THEREOF, EVEN
/// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  NOTWITHSTANDING ANYTHING ELSE
/// TO THE CONTRARY, IN NO EVENT WILL THE EM PARTIES AGGREGATE LIABILITY UNDER
/// THIS AGREEMENT OR ARISING OUT OF YOUR USE OF THE SOURCE CODE EXCEED ONE
/// HUNDRED U.S. DOLLARS (U.S. $100).
///
/// Because some jurisdictions do not allow the exclusion or limitation of
/// incidental or consequential damages or limitation on how long an implied
/// warranty lasts, the above limitations or exclusions may not apply to you.
///
/// Please refer to the accompanying software license agreement and Terms of Use
/// located at, respectively:
/// https://forums.emdeveloper.com/emassets/emus_sourcelicense.html and
/// https://forums.emdeveloper.com/emassets/emus_termsofuse.html  to better
/// understand all of your rights and obligations hereunder.  Except as
/// otherwise provided in this Agreement, all capitalized terms defined in said
/// software license agreement and Terms of Use shall have the meanings
/// ascribed to such terms therein.
/// @endcond
////////////////////////////////////////////////////////////////////////////////

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

_Static_assert(255 >= (int)BEACON_SIG_COUNT, "Too many QP-nano signals!");

// This variable represents the task.
static Example_t gBeaconTask;

// This variable stores all signals being sent to the task.
static SECTION_NONPERSISTENT QEvt gBeaconEventQueue[5];

/**
 * @bried Called during a stack event.
 */
void Example_CoreCallback(BleEvent event, BleStatus status, void* pParam);

/**
 * @bried Called during a GAP event.
 */
void Example_GapCallback(BleGapEvent event, BleStatus status, void *parms);

/**
 * @brief Constructor for the task.
 *
 * This will initialize the state machine variable @ref gBeaconTask.
 *
 * @param me Pointer to the task to construct.
 */
void Example_Constructor(Example_t *me);

/**
 * @brief Initialization state.
 *
 * The first state of a QP-nano task is special. It must always make a
 * transition to another state. This implementation will initialize the
 * Bluetooth Low Energy stack.
 *
 * @param me Pointer to the task.
 * @returns Next action for the state machine to take (stay or transition).
 */
QState Example_Init(Example_t *me);

/**
 * @brief Waiting for Bluetooth Low Energy stack initialization.
 *
 * @param me Pointer to the task.
 * @returns Next action for the state machine to take (stay or transition).
 */
QState Example_WaitForStack(Example_t *me);

/**
 * @brief Main state of the beacon application.
 *
 * @param me Pointer to the task.
 * @returns Next action for the state machine to take (stay or transition).
 */
QState Example_Run(Example_t *me);

/**
 * @brief Where programs go to die.
 *
 * @param me Pointer to the task.
 * @returns Next action for the state machine to take (stay or transition).
 */
QState Example_Error(Example_t *me);

////////////////////////////////////////////////////////////////////////////////

void Example_CoreCallback(BleEvent event, BleStatus status, void *pParam)
{
    switch(event)
    {
        case BLEEVENT_INITIALIZATION_RSP:
            // Notify the task.
            QACTIVE_POST(&gBeaconTask.super, (int)BEACON_BLE_INIT_SIG,
                (QParam)status);
            break;

        // Handle the vendor specific ACI commands to allow the Configuration
        // Editor to communicate with the EM9304 DVK with the application
        // active.  This allows you to invalidate OTP patches with the
        // Configuration Editor.  When the BLE stack is active, ACI commands
        // are ignored unless the vendor specific events are handled here.
        // After development is complete, this functionality can be removed
        // if no longer needed.
        case BLEEVENT_VENDOR_EVENT:
        case BLEEVENT_VENDOR_COMMAND_STATUS:
        case BLEEVENT_VENDOR_COMMAND_COMPLETE:
            BleAci_CoreCallback(event, status, pParam);
            break;

        default:
            break;
    }
}


void Example_GapCallback(BleGapEvent event, BleStatus status, void *parms)
{
    switch(event)
    {
        case BLEGAP_EVENT_CONNECTED:
            // Handle connected event.
            break;

        case BLEGAP_EVENT_DISCONNECTED:
            (void)BLEGAP_SetMode(BLEMODE_DISCOVERABLE | BLEMODE_CONNECTABLE);
            break;

        default:
            break;
    }
}


void Example_Constructor(Example_t *me)
{
    // Tell QP-nano what the initial state is.
    QActive_ctor(&me->super, Q_STATE_CAST(Example_Init));
}


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


QState Example_WaitForStack(Example_t *me)
{
    QState status;

    switch( Q_SIG(me) )
    {
        case BEACON_BLE_INIT_SIG:
        {
            if(BLESTATUS_SUCCESS == Q_PAR(me))
            {
                // Register a GAP device.
                (void)BLEGAP_RegisterDevice(BLEGAPROLE_PERIPHERAL,
                    Example_GapCallback);

                // Set the name of the peripheral.
                (void)BLEGAP_SetLocalBluetoothDeviceName((U8*)"Example Beacon", 14u);

                // Start advertising.
                (void)BLEGAP_SetMode(BLEMODE_DISCOVERABLE | BLEMODE_CONNECTABLE);

                status = Q_TRAN(Example_Run);
            }
            else
            {
                status = Q_TRAN(Example_Error);
            }

            break;
        }
        default:
            // Let the super class handle this.
            status = Q_SUPER(QHsm_top);
            break;
    }

    return status;
}


QState Example_Run(Example_t *me)
{
    QState status;

    switch( Q_SIG(me) )
    {
        default:
            // Let the super class handle this.
            status = Q_SUPER(QHsm_top);
            break;
    }

    return status;
}


QState Example_Error(Example_t *me)
{
    QState status;

    switch( Q_SIG(me) )
    {
        default:
            // Let the super class handle this.
            status = Q_SUPER(QHsm_top);
            break;
    }

    return status;
}


/**
 * @brief Entry function for the application.
 *
 * The application entry function is called early in the boot process to
 * allow for early initialization, creation of the application's task, and
 * to set configuration options.  The entry function is also called when the
 * EM9304 resumes from sleep.  The entry function is called prior to the
 * initialization of the peripheral drivers, QP-nano, the link layer, and
 * the Bluetooth stack.  Therefore, care must be taken to not call initialized
 * functions in the entry function.
 */
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
