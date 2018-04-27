/**
 ** ############################################################################
 **
 ** @file    beaconbutton.c
 ** @brief   Main entry point for the button/led/timer demo program
 **
 ** This SOC project demonstrates
 ** 1. Buttons via GPIO pins.  It includes normal presses, long presses, and a
 **    function that returns the running time that a button has been held.
 ** 2. Use of the 32.768 KHz clock
 ** 3. One-shot delay timer
 ** 4. A discoverable/connectable beacon
 ** 5. LEDs via GPIO
 **
 ** Copyright (c) 2018 EM Microelectronic-US Inc. All rights reserved.
 ** Developed by Glacier River Design, LLC.
 **
 ** ############################################################################
 ** EM Microelectronic-US Inc. License Agreement
 **
 ** Please read this License Agreement ("Agreement") carefully before
 ** accessing, copying, using, incorporating, modifying or in any way providing
 ** ("Using" or "Use") this source code.  By Using this source code, you: (i)
 ** warrant and represent that you have obtained all authorizations and other
 ** applicable consents required empowering you to enter into and (ii) agree to
 ** be bound by the terms of this Agreement.  If you do not agree to this
 ** Agreement, then you are not permitted to Use this source code, in whole or
 ** in part.
 **
 ** Pursuant to the terms in the accompanying software license agreement and
 ** Terms of Use located at: www.emdeveloper.com/emassets/emus_termsofuse.html
 ** (the terms of each are incorporated herein by this reference) and subject to
 ** the disclaimer and limitation of liability set forth below, EM
 ** Microelectronic US Inc. ("EM"), grants strictly to you, without the right to
 ** sublicense, a non-exclusive, non-transferable, revocable, worldwide license
 ** to use the source code to modify the software program for the sole purpose
 ** of developing object and executable versions that execute solely and
 ** exclusively on devices manufactured by or for EM or your products that use
 ** or incorporate devices manufactured by or for EM; provided that, you clearly
 ** notify third parties regarding the source of such modifications or Use.
 **
 ** Without limiting any of the foregoing, the name "EM Microelectronic-US
 ** Inc." or that of any of the EM Parties (as such term is defined below) must
 ** not be Used to endorse or promote products derived from the source code
 ** without prior written permission from an authorized representative of EM
 ** Microelectronic US Inc.
 **
 ** THIS SOURCE CODE IS PROVIDED "AS IS" AND "WITH ALL FAULTS", WITHOUT ANY
 ** SUPPORT OR ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 ** TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 ** PURPOSE ARE DISCLAIMED.  ALSO, THERE IS NO WARRANTY OF NON-INFRINGEMENT,
 ** TITLE OR QUIET ENJOYMENT.
 **
 ** IN NO EVENT SHALL EM MICROELECTRONIC US INC., ITS AFFILIATES, PARENT AND
 ** ITS/THEIR RESPECTIVE LICENSORS, THIRD PARTY PROVIDERS, REPRESENTATIVES,
 ** AGENTS AND ASSIGNS ("COLLECTIVLEY, "EM PARTIES") BE LIABLE FOR ANY DIRECT,
 ** INDIRECT, INCIDENTAL, SPECIAL, PUNITIVE, EXEMPLARY, OR CONSEQUENTIAL
 ** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, EQUIPMENT, SYSTEMS, SOFTWARE, TECHNOLOGY,
 ** SERVICES, GOODS, CONTENT, MATERIALS OR PROFITS; BUSINESS INTERRUPTION OR
 ** OTHER ECONOMIC LOSS OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT
 ** LIMITED TO ANY DEFENSE THEREOF) HOWEVER CAUSED AND ON ANY THEORY OF
 ** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 ** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOURCE
 ** CODE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  NOTWITHSTANDING
 ** ANYTHING ELSE TO THE CONTRARY, IN NO EVENT WILL THE EM PARTIES' AGGREGATE
 ** LIABILITY UNDER THIS AGREEMENT OR ARISING OUT OF YOUR USE OF THE SOURCE
 ** CODE EXCEED ONE HUNDRED U.S. DOLLARS (U.S. ).
 **
 ** Please refer to the accompanying software license agreement and Terms of
 ** Use located at: www.emdeveloper.com/emassets/emus_termsofuse.html to better
 ** understand all of your rights and obligations hereunder.
 ** ############################################################################
 */
// 9304 soc platform
#include <arc.h>
#include <types.h>
#include <macros.h>
#include <em_qpn.h>
#include <bsp.h>
#include <platform.h>
#include <pml.h>
#include <timer_hal.h>
#include <spi_slave.h>
#include <spi_master.h>
#include <spi_master_hal.h>
#include <gpio.h>
#include <driver.h>
#include <jli.h>
// stack
#include <BleBase.h>
#include <BleGap.h>
#include <BleSystem.h>
#include <BleEngine.h>
#include <BleHostStackTask.h>
#include <AciCoreCallback.h>
// beaconbutton
#include "BleCommonTypes.h"
#include "projectConfig.h"
#include "beaconbutton_jli.h"
#include "buttons.h"
#include "leds.h"
#include "tmr.h"

// PML configuration data structure
extern PML_Configuration_t gPML_Config;

/****************************************************************************\
 * Local data
\****************************************************************************/

//debug
U32 nextBlink = 0;
U8 toggle;

/****************************************************************************\
 * QPNano declarations
\****************************************************************************/
// This variable represents the task.
BcnBtnTask_t gBcnBtnTask;

// This variable stores all signals being sent to the task.
SECTION_NONPERSISTENT QEvt gBcnBtnEventQueue[20];

/****************************************************************************\
 * The entry function serves the place of main() in a traditional C/C++
 * environment.
\****************************************************************************/
ENTRY_FUNCTION(BcnBtn_Entry);

/****************************************************************************\
 * BcnBtn task functions
\****************************************************************************/

/**
 * @brief BLE stack mutex to ensure the BLE stack functions are not re-entered.
 *        Get the mutex.
 */
QMutex BcnBtn_StackMutexLock(void)
{
    return QK_mutexLock(0xFF /*AO_BleHostStackTask.super.prio*/);
}

/**
 * @brief BLE stack mutex to ensure the BLE stack functions are not re-entered.
 *        Release the mutex.
 */
void BcnBtn_StackMutexUnlock(QMutex mutex)
{
    QK_mutexUnlock(mutex);
}

/**
 * @brief get the task struct
 *
 * @param  none
 * @return the BcnBtnTask_t struct containing task variables
 */
BcnBtnTask_t* BcnBtnTask_Get(void)
{
    return &gBcnBtnTask;
}
JLI_SET(JLI_INDEX_BcnBtnTask_Get, BcnBtnTask_Get);

/**
 *  @brief BcnBtnTask_Error
 *
 *  A fatal error has occured. Sit here and do nothing.
 *
 **/
QState BcnBtnTask_Error(BcnBtnTask_t *me)
{
    QState status = Q_HANDLED();

    switch( Q_SIG(me) )
    {
        default:
            /// Let the super class handle this.
            status = Q_SUPER(&QHsm_top);
            break;
    }
    return status;
}

/**
 * @brief GPIO ISR
 */
void IRQUserHandler_GPIO(uint8_t gpio)
{
    // If the interrupt is for the button
    if(BUTTON_GPIO == gpio)
    {
        // avoid spurious interrupts.  post a wakeup only if there's no timer wakeup set
        BcnBtnTask_t* me = BcnBtnTask_Get();
        if(!me->wakeup)
        {
            QACTIVE_POST(&me->super, BCNBTN_BTNWAKE_SIG, (QParam)0);
        }
    }
}
JLI_OVERRIDE(IRQUserHandler_GPIO);

/**
 * @brief This callback receives the wakeup signal following a timed sleep
 *
 * @param status
 * @param pUserData
 *
 */
void BcnBtn_Wakeup(Driver_Status_t status, void *pUserData)
{
    BcnBtnTask_t* me = BcnBtnTask_Get();
    QACTIVE_POST(&me->super, BCNBTN_WAKEUP_SIG, (QParam)status);
}

/**
 * @brief callback for core stack events
 *
 * @param event
 * @param status
 * @param pParam
 */
void BcnBtn_CoreCallback(BleEvent event, BleStatus status, void *pParam)
{
    switch(event)
    {
        case BLEEVENT_INITIALIZATION_RSP:
        {
            // Notify the task.
            BcnBtnTask_t* me = BcnBtnTask_Get();
            QACTIVE_POST(&me->super, (int)BCNBTN_BLE_INIT_SIG, (QParam)status);
            break;
        }
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

/**
 * @brief callback for stack GAP events
 *
 * @param event
 * @param status
 * @param parms
 */
void BcnBtn_GapCallback(BleGapEvent event, BleStatus status, void *parms)
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


/**
 * @brief manage the LED.  Set it according to app and button states
 *
 * Normally, pressing the button toggles the LED on/off.  Holding the button for
 * more than 3 secs makes it begin to toggle at a 1Hz rate.
 *
 * This function demonstrates the use of GetButtonPressTime().
 *
 */
void BcnBtn_ManageLED(BcnBtnTask_t *me)
{
    if( PushButtonDown() ){
      (void)BLEGAP_SetLocalBluetoothDeviceName((U8*)"TT_BURST", 8u);
    }
    // Get the time in ms that the button has been held (0 if not pressed)
    U32 bt = GetButtonPressTime();
    // Has the button been held for >3 secs?
    if(bt > 3000)
    {
        while(bt > 1000)
        {
            bt -=1000;
        }
        if(bt > 500)
        {
            LED_SetState(LED_GRN, LED_ON);
        }
        else
        {
            LED_SetState(LED_GRN, LED_OFF);
        }
    }
    // No.  Is the button pressed?
    else if(bt >0)
    {
        if(me->bcnBtnState)
        {
            LED_SetState(LED_GRN, LED_OFF);
        }
        else
        {
            LED_SetState(LED_GRN, LED_ON);
        }
    }
    // No, not pressed.  Just set the LED according to app state.
    else if(me->bcnBtnState)
    {
        LED_SetState(LED_GRN, LED_ON);
    }
    else
    {
        LED_SetState(LED_GRN, LED_OFF);
    }
}

/**
 * @brief Wakeup and run the next interval
 *
 * Arrive here when in normal run state and the sleep timer
 * has expired.
 *
 */
QState BcnBtnRunNext(BcnBtnTask_t *me)
{
    // service the button
    ButtonTask();

    // set the LED
    BcnBtn_ManageLED(me);

    // set a wakeup timer if button is pressed, otherwise sleep until button INT
    me->wakeup = AreButtonsActive();
    if (me->wakeup)
    {
        #define BTN_DEBOUNCE_TIME   20     // ~20ms
        float delayTime = BTN_DEBOUNCE_TIME;
        Timer_OneshotDelay(delayTime, &BcnBtn_Wakeup, (void*)NULL);
    }

    // We handled this event
    return Q_HANDLED();
}

/**
 * @brief process stack events
 *
 * Arrive here when in normal run state and stack events occur
 *
 */
QState BcnBtnRunStackEvents(BcnBtnTask_t *me) {
    QState qpStatus;
    QMutex stackMutex;

    switch( Q_SIG(me) )
    {
        case BCNBTN_BLE_INIT_SIG:
        {
            if(BLESTATUS_SUCCESS == Q_PAR(me))
            {
                // Register a GAP device.
                stackMutex = BcnBtn_StackMutexLock();
                (void)BLEGAP_RegisterDevice(BLEGAPROLE_PERIPHERAL,
                    BcnBtn_GapCallback);
                BcnBtn_StackMutexUnlock(stackMutex);

                // Set the name of the peripheral.
                (void)BLEGAP_SetLocalBluetoothDeviceName((U8*)"TT_GOOD", 7u);

                // Start advertising.
                (void)BLEGAP_SetMode(BLEMODE_DISCOVERABLE | BLEMODE_CONNECTABLE);

                // LED off after startup
                LED_SetState(LED_GRN, LED_OFF);

                // trigger the first wakeup.  it runs forever after that
                QACTIVE_POST(&me->super, BCNBTN_WAKEUP_SIG, (QParam)0);

                qpStatus = Q_HANDLED();
            }
            else
            {
                qpStatus = Q_TRAN(&BcnBtnTask_Error);
            }

            break;
        }
        default:
            // Let the super class handle this.
            qpStatus = Q_SUPER(&QHsm_top);
            break;
    }

    return qpStatus;
}

/**
 * @brief normal run state
 *
 */
QState BcnBtnTask_Run(BcnBtnTask_t *me)
{
    QState qpStatus;

    if(Q_SIG(me) == BCNBTN_WAKEUP_SIG)
    {
        qpStatus = BcnBtnRunNext(me);
    }
    else if(Q_SIG(me) == BCNBTN_BTNWAKE_SIG)
    {
        qpStatus = BcnBtnRunNext(me);
    }
    else if((Q_SIG(me) >= BCNBTN_FIRST_STACK_EVENT) &&
            (Q_SIG(me) <= BCNBTN_LAST_STACK_EVENT))
    {
        qpStatus = BcnBtnRunStackEvents(me);
    }
    else
    {
        // Let the super class handle this.
        qpStatus = Q_SUPER(&QHsm_top);
    }
    return qpStatus;
}

/**
 * @brief Initialize the BcnBtn task
 *
 */
QState BcnBtnTask_Init(BcnBtnTask_t *me) {

   QState qpStatus;
   QMutex stackMutex;

   // LED on during startup
   LED_Init();
   LED_SetState(LED_GRN, LED_ON);

   // Initialize button HW and FW
   Buttons_Init();

   // Start this app in the off state
   me->bcnBtnState = 0;

   // Initialize the Bluetooth stack and register the callback.
   me->handler.callback = BcnBtn_CoreCallback;
   stackMutex = BcnBtn_StackMutexLock();
   BleStatus bleStatus = BLEMGMT_InitWithHandler(&me->handler);
   BcnBtn_StackMutexUnlock(stackMutex);

   if (BLESTATUS_SUCCESS == bleStatus || BLESTATUS_PENDING == bleStatus )
   {
       // Now wait for the stack to finish initialization.
       qpStatus = Q_TRAN(&BcnBtnTask_Run);
   }
   else
   {
       // The BLEMGMT_Init() call failed.
       qpStatus = Q_TRAN(&BcnBtnTask_Error);
   }

   return qpStatus;
}

/**
 * @brief Constructor - called once at reset
 *
 */
static void BcnBtnTask_ctor(BcnBtnTask_t *me)
{
    QActive_ctor(&(me->super), Q_STATE_CAST(&BcnBtnTask_Init));
}

/**
 * @brief For lowest power, disable all GPIO. Each will be overwritten as needed.
 */
static void BcnBtn_gpioinit(void)
{
   GPIO_DisableInput(0);
   GPIO_DisableOutput(0);
   GPIO_SetOutputPinFunction(0, GPIO_PIN_FUNC_OUT_GPIO);
   GPIO_EnablePullUp(0);
   GPIO_DisablePullDown(0);

   GPIO_DisableInput(1);
   GPIO_DisableOutput(1);
   GPIO_SetOutputPinFunction(1, GPIO_PIN_FUNC_OUT_GPIO);
   GPIO_EnablePullUp(1);
   GPIO_DisablePullDown(1);

   GPIO_DisableInput(2);
   GPIO_DisableOutput(2);
   GPIO_SetOutputPinFunction(2, GPIO_PIN_FUNC_OUT_GPIO);
   GPIO_EnablePullUp(2);
   GPIO_DisablePullDown(2);

   GPIO_DisableInput(3);
   GPIO_DisableOutput(3);
   GPIO_SetOutputPinFunction(3, GPIO_PIN_FUNC_OUT_GPIO);
   GPIO_EnablePullUp(3);
   GPIO_DisablePullDown(3);

   GPIO_DisableInput(4);
   GPIO_DisableOutput(4);
   GPIO_SetOutputPinFunction(4, GPIO_PIN_FUNC_OUT_GPIO);
   GPIO_EnablePullUp(4);
   GPIO_DisablePullDown(4);

   GPIO_DisableInput(5);
   GPIO_DisableOutput(5);
   GPIO_SetOutputPinFunction(5, GPIO_PIN_FUNC_OUT_GPIO);
   GPIO_EnablePullUp(5);
   GPIO_DisablePullDown(5);

   GPIO_DisableInput(6);
   GPIO_DisableOutput(6);
   GPIO_SetOutputPinFunction(6, GPIO_PIN_FUNC_OUT_GPIO);
   GPIO_EnablePullUp(6);
   GPIO_DisablePullDown(6);

   GPIO_DisableInput(7);
   GPIO_DisableOutput(7);
   GPIO_SetOutputPinFunction(7, GPIO_PIN_FUNC_OUT_GPIO);
   GPIO_EnablePullUp(7);
   GPIO_DisablePullDown(7);

   // initialize GPIO 8..11 only if not running debugger
   #if 0==ALLOW_JTAG_DEBUG
   GPIO_DisableInput(8);
   GPIO_DisableOutput(8);
   GPIO_SetOutputPinFunction(8, GPIO_PIN_FUNC_OUT_GPIO);
   GPIO_DisablePullUp(8);
   GPIO_EnablePullDown(8);

   GPIO_DisableInput(9);
   GPIO_DisableOutput(9);
   GPIO_SetOutputPinFunction(9, GPIO_PIN_FUNC_OUT_GPIO);
   GPIO_DisablePullUp(9);
   GPIO_EnablePullDown(9);

   GPIO_DisableInput(10);
   GPIO_DisableOutput(10);
   GPIO_SetOutputPinFunction(10, GPIO_PIN_FUNC_OUT_GPIO);
   GPIO_EnablePullUp(10);
   GPIO_DisablePullDown(10);

   GPIO_DisableInput(11);
   GPIO_DisableOutput(11);
   GPIO_SetOutputPinFunction(11, GPIO_PIN_FUNC_OUT_GPIO);
   GPIO_EnablePullUp(11);
   GPIO_DisablePullDown(11);
   #endif

}

/**
 * @brief The BcnBtn application starts here.
 *
 */
void BcnBtn_EntryPatchable(void)
{
    // Initialize all gpio to default states
    BcnBtn_gpioinit();

    // Add the task to QP-nano
    BcnBtnTask_t* me = BcnBtnTask_Get();
    QF_addTask(&me->super, &gBcnBtnEventQueue[0],
        Q_DIM(gBcnBtnEventQueue));

    // Enable the universal timer.
    ((Timer_Configuration_t *)MODIFY_CONST(&gTimer_Config))->enabled = true;

    // override some configuration settings so a configuration patch is not required
    Platform_Configuration_t *pPlatform_Config = (Platform_Configuration_t*) MODIFY_CONST (&gPlatform_Config);
    pPlatform_Config->blePacketPayloadLengthMax = 251;
    pPlatform_Config->bleIsStackRequested = 1;

    // true=prevent the processor from sleeping, false=allow it to sleep
    #if (1==ALLOW_JTAG_DEBUG)
    gPML_Config.sleepModeForbiden = true;
    #else
    gPML_Config.sleepModeForbiden = false;
    #endif

    // Setup the task during a reset.
    if(!PML_DidBootFromSleep())
    {
        // Construct our new QP-nano task.
        BcnBtnTask_ctor(me);
    }
}
JLI_SET(JLI_INDEX_BcnBtn_EntryPatchable, BcnBtn_EntryPatchable);

/**
 * @brief Entry function, called once on startup
 *
 */
void BcnBtn_Entry(void)
{
    ( void ) BcnBtn_EntryPatchable();
}
