/**
 ** ############################################################################
 **
 ** @file    buttons.c
 ** @brief   button features for 9304 programs
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
#include <types.h>
#include <macros.h>
#include <em_qpn.h>
#include <bsp.h>
#include <platform.h>
#include <gpio.h>
#include <pml.h>
#include <boot.h>
// stack
#include <BleBase.h>
#include <BleEngine.h>
// this app
#include "BleCommonTypes.h"
#include "projectConfig.h"
#include "beaconbutton_jli.h"
#include "tmr.h"
#include "leds.h"
#include "buttons.h"

/****************************************************************************\
 * RAM data
\****************************************************************************/

/**
 * button state variables
 */
U8 buttonIsActive;
U8 buttonDownEvent;
U8 buttonLongEvent;
U8 buttonUpEvent;
U32 buttonDownTime;
U32 buttonDebounceTime;
U32 buttonLongTime;

/****************************************************************************\
 * Local functions Prototype
\****************************************************************************/

/****************************************************************************\
 * External functions Prototype
\****************************************************************************/

/****************************************************************************\
 * Local data
\****************************************************************************/

/****************************************************************************\
 * Local functions
\****************************************************************************/

/**
 ******************************************************************************
 * @brief utils to get/clear the primary button events
 * 
 * @param  
 * @return 
 ******************************************************************************/
#define GetButtonUpEvent()    buttonUpEvent
#define ClrButtonUpEvent()    {buttonUpEvent = 0;}
#define GetButtonDownEvent()  buttonDownEvent
#define ClrButtonDownEvent()  {buttonDownEvent = 0;}
#define GetButtonLongEvent()  buttonLongEvent
#define ClrButtonLongEvent() {buttonLongEvent = 0;}


/**
 * @brief returns true if a button is currently pushed
 * 
 * @param  none
 * @return true if a button is currently pushed
 */
U8 AreButtonsActive(void)
{
   return buttonIsActive;
}
JLI_SET(JLI_INDEX_AreButtonsActive, AreButtonsActive);

/**
 * @brief take action when the button is released
 * @param  none 
 * @return nothing 
 */
void RunButtonUpEvent() {
   BcnBtnTask_t* me = BcnBtnTask_Get();
   me->bcnBtnState = !me->bcnBtnState;

   // acknowledge button events
   ClrButtonDownEvent();
   ClrButtonLongEvent();
   ClrButtonUpEvent();
}

/**
 * @brief take action when the button is pressed
 * @param  none 
 * @return nothing 
 */
void RunButtonDownEvent(void){
}

/**
 *******************************************************************************
 * @brief return the raw button pin state
 * 
 * @param  none
 * @return TRUE iff button is pressed now, FALSE otherwise
 ******************************************************************************/
U8 PushButtonDown(void) {
   //pushbutton closes to ground.  It has a pull-up attached.
   //Thus when the button is pressed, the result is 0.
   if (((GPIO->RegGPIODataIn.r32 >> BUTTON_GPIO) & 1) == 1 || ((GPIO->RegGPIODataIn.r32 >> BUTTON_GPIO5) & 1) == 0 || ((GPIO->RegGPIODataIn.r32 >> BUTTON_GPIO6) & 1) == 0)
      return (FALSE );
   else
      return (TRUE );
}

/**
 ******************************************************************************
 * @brief called when the button is first pressed
 *  
 * @param  none 
 * @return nothing 
 *****************************************************************************/
#define PUSH_DEBOUNCE_TIME_32K       655      // ~20ms
#define PUSH_LONG_TIME_32K           163840   // ~5000ms
void ButtonStart(void) {
   buttonDownTime = TMR_Get32();
   buttonDebounceTime = buttonDownTime + PUSH_DEBOUNCE_TIME_32K;
   buttonLongTime = buttonDownTime + PUSH_LONG_TIME_32K;
   buttonDownEvent = 0;
   buttonLongEvent = 0;
   buttonUpEvent = 0;
}

/**
 ******************************************************************************
 * @brief get the time since button was first pressed
 * 
 * @param  none
 * @return approximate elapsed time in ms, 0 if the button is not pushed or
 *         not debounced
 *****************************************************************************/
U32 GetButtonPressTime(void)
{
    if (!buttonIsActive)
    {
       return 0;
    }
    U32 bdTime = TMR_Get32() - buttonDownTime;
    if (bdTime < PUSH_DEBOUNCE_TIME_32K)
    {
       return 0;
    }
    // div by 32 for approximate time in ms
    return(bdTime>>5);
}

/***
 ******************************************************************************
 * @brief periodically checks button status, registers up, down, 
 *        and long push events.
 *  
 * Call this regularly.  For constent feel, call it at a consistent
 * interval while the button is pressed. 
 * 
 * WARNING:  Do not getAndClear the down or long events until 
 *           the button up event occurs.  Otherwise, you may get
 *           multiple down/long events for the same push.
 * @param  none
 * @return nothing
 *****************************************************************************/
void CheckButtonTransition(void)
{
   if(PushButtonDown()){
      if(!buttonIsActive) {
         // new press
         ButtonStart();
         buttonIsActive = 1;
      }
      else {
         // ongoing press.  not new
         if (!TMR_IsA32GreaterOrEqualToTMR(buttonDebounceTime)) {
            buttonDownEvent = 1;
         }
         if (!TMR_IsA32GreaterOrEqualToTMR(buttonLongTime)) {
            buttonLongEvent = 1;
         }
      }
   }
   else
   {
      // button is not pushed at this time
      if (buttonDownEvent) {
         // register an up event only if it was down long enough to debounce
         buttonUpEvent = 1;
      }
      // but in any case, it's up now.
      buttonIsActive = 0;
   }
}

/**
 ******************************************************************************
 * @brief service the buttons
 *
 * @param  nothing
 * @return none
 *****************************************************************************/
void ButtonTask(void)
{
   // Update the current state and events
   CheckButtonTransition();

   if (buttonDownEvent) {
      RunButtonDownEvent();
   }
   if(buttonUpEvent){
      RunButtonUpEvent();
   }

   // Optional action for long press event
   // if (GetButtonLongEvent()) {
   //    RunButtonLongEvent();
   // }
}
JLI_SET(JLI_INDEX_ButtonTask, ButtonTask);

/**
 * @brief initialize button and reed switch FW and HW 
 *  
 * @param none
 *  
 * @return nothing
 */
void Buttons_Init(void) {
   // Setup the GPIOs for the pins
   // When button is pressed, it shorts to ground, so it needs a pullup.
   GPIO_EnableInput(BUTTON_GPIO);
   GPIO_EnablePullUp(BUTTON_GPIO);
   GPIO_DisableOutput(BUTTON_GPIO);
   
   GPIO_EnableInput(BUTTON_GPIO5);
   GPIO_EnablePullDown(BUTTON_GPIO5);
   GPIO_DisableOutput(BUTTON_GPIO5);
   
   GPIO_EnableInput(BUTTON_GPIO6);
   GPIO_EnablePullDown(BUTTON_GPIO6);
   GPIO_DisableOutput(BUTTON_GPIO6);
   // Setup INT based on SW GPIO pulled low
   GPIO_SetPolarityFalling(BUTTON_GPIO);

   IRQ_Enable(IRQ_GROUP_GPIO, GPIO_MASK(BUTTON_GPIO));
   IRQ_Unmask(IRQ_GROUP_GPIO, GPIO_MASK(BUTTON_GPIO));
   
   IRQ_Enable(IRQ_GROUP_GPIO, GPIO_MASK(BUTTON_GPIO5));
   IRQ_Unmask(IRQ_GROUP_GPIO, GPIO_MASK(BUTTON_GPIO5));
   IRQ_Enable(IRQ_GROUP_GPIO, GPIO_MASK(BUTTON_GPIO6));
   IRQ_Unmask(IRQ_GROUP_GPIO, GPIO_MASK(BUTTON_GPIO6));

   // // Register a wake-up action.
   PML_ConfigWakeUpByGpio(BUTTON_GPIO, false, true);
   PML_RegisterWakeUpAction(PML_WAKEUP_ACTION_RUN_HF_XTAL,
        SET_BOOT_ACTION_FLAGS_GPIO(GPIO_MASK(BUTTON_GPIO)), true);
   
   PML_ConfigWakeUpByGpio(BUTTON_GPIO5, false, true);
   PML_RegisterWakeUpAction(PML_WAKEUP_ACTION_RUN_HF_XTAL,
        SET_BOOT_ACTION_FLAGS_GPIO(GPIO_MASK(BUTTON_GPIO5)), true);
   
   PML_ConfigWakeUpByGpio(BUTTON_GPIO6, false, true);
   PML_RegisterWakeUpAction(PML_WAKEUP_ACTION_RUN_HF_XTAL,
        SET_BOOT_ACTION_FLAGS_GPIO(GPIO_MASK(BUTTON_GPIO6)), true);

   // initialize module vars
   buttonDownEvent = 0;
   buttonLongEvent = 0;
   buttonUpEvent = 0;
   buttonDebounceTime = 0;
   buttonLongTime = 0;
   buttonIsActive = 0;
}
JLI_SET(JLI_INDEX_Buttons_Init, Buttons_Init);

