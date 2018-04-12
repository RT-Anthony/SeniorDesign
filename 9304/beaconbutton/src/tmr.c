/**
 ** ############################################################################
 **
 ** @file    tmr.c
 ** @brief   beacon timer routines
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
#include <pml.h>
#include <timer_hal.h>
#include <sleep_timer.h>
#include <driver.h>
#include <gpio.h>
// stack
#include <BleBase.h>
#include <BleEngine.h>
// this app
#include "BleCommonTypes.h"
#include "projectConfig.h"
#include "tmr.h"

/** 
 * @brief Retrieve 32-bit rolling timer
 *
 * @param  none
 * @return time in 32K ticks
 */
U32 TMR_Get32(void)
{
   return SleepTimer_GetCurrentValue();
}

/** 
 * @brief  Determine if timer A is greater than or equal to timer B
 *
 * @param  timerA, timerB
 * @return TRUE if timerA >= timerB.  FALSE otherwise
 */
U8 TMR_IsA32GreaterOrEqualToB32(U32 timerA, U32 timerB) {
   timerA -= timerB;
   return (timerA < 0x80000000);
}

/** 
 * @brief  Determine if timer A has passed TMR
 *
 * @param  timerA
 * @return TRUE if timerA >= TMR.  FALSE otherwise
 */
U8 TMR_IsA32GreaterOrEqualToTMR(U32 timerA) {
   timerA -= TMR_Get32();
   return (timerA < 0x80000000);
}

/**
 * @brief scale up 1ms ticks into 32.768KHz ticks 
 *  
 * @param  time in ms. Max 131,072,000 (~36 mins)
 * @return time in 32K ticks 
 */
U32 MSTo32K(U32 tickMS)
{
   if (tickMS > MAX_SCALABLE_MS)
   {
      tickMS = MAX_SCALABLE_MS;
   }
   U32 tick32K = tickMS << 5;    // 2.34% error
   tickMS >>=1;
   tick32K += tickMS;            // .818% error
   tickMS >>=1;
   tick32K += tickMS;            // .055% error
   tickMS >>=4;
   tick32K += tickMS;            // .007% error
   return tick32K;
}

