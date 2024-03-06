//#include "hal_data.h"
//void hal_entry(void)
//{
//    // Reset pin 0 to 0 to turn green led ON (110 = 6),
//    //pins 0,1,2 are output (111 = 7),
//    R_IOPORT6->PCNTR1 = 0x00070007;
//    __asm("     ldr r4, =72000000");
//    __asm("loop0:                ");
//    __asm("     subs r4,r4,#1   ");
//    __asm("     bne loop0        ");
//
//    R_IOPORT6->PCNTR1 = 0x00060007;
//
//    __asm("     ldr r4, =72000000");
//    __asm("loop:                ");
//    __asm("     subs r4,r4,#1   ");
//    __asm("     bne loop        ");
//
//    // pins 0,1,2 are output, set pin 0 to 1 to turn green led OFF
//    R_IOPORT6->PCNTR1 = 0x00070007;
//}

//#include "hal_data.h"
//void hal_entry(void)
//{
//    while (1)
//    {
//        R_IOPORT6->PCNTR1 = 0x00010007;
//
//        // delay 0.9s
//        __asm(" LDR r4, =72000000");
//        __asm(" ON_LOOP: ");
//        __asm(" SUBS r4, r4, #1"); // r4-=1
//        __asm(" BNE ON_LOOP"); // if r4==0: break
//
//        // turn LED OFF
//        R_IOPORT6->PCNTR1 = 0x00050007;
//        __asm(" LDR r4, =72000000");
//        __asm(" OFF_LOOP: ");
//        __asm(" SUBS r4, r4, #1");
//        __asm(" BNE OFF_LOOP");
//    }
//}

/*Fiavi Yang*/
/* HAL-only entry function */
#include "hal_data.h"

// Define the number of counts per millisecond and microsecond (1 count per clock tick, clock rate is 120MHz)
// So, there are 120E6 ticks per second.
// Divide by 1000 to get ticks / millisecond
#define COUNTS_PER_MILLISECOND (120E6 / 1000)
#define COUNTS_PER_MICROSECOND (120E6 / 1000000)
bsp_leds_t leds;


// Frequency array, freq[0] is for testing
uint32_t freq[9] = {2, 440, 494, 523, 587, 659, 698, 783, 830};
uint32_t f_index = 1;
const uint32_t noteDuration = 500;

void hal_entry(void)
{
     ioport_level_t led_level = IOPORT_LEVEL_HIGH; //off
     ioport_level_t buzz_level = IOPORT_LEVEL_HIGH; //off
     R_BSP_LedsGet(&leds);
     g_ioport.p_api->pinWrite(leds.p_leds[0],led_level);
     g_ioport.p_api->pinWrite(leds.p_leds[1],led_level);
     g_ioport.p_api->pinWrite(leds.p_leds[2],led_level);
     g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_04,buzz_level);


    // Variable to hold counts
     timer_size_t counts = 0;
     uint32_t duration_counts = 0;

    // Open the timer using the configured options from the configurator
     g_timer0.p_api->open(g_timer0.p_ctrl, g_timer0.p_cfg);
    //Reset the counter to initial value
     g_timer0.p_api->reset(g_timer0.p_ctrl);



    // Main Loop
    while(1)
    {
     g_timer0.p_api->counterGet(g_timer0.p_ctrl, &counts);
     uint32_t timer_period = (1000000  / (freq[f_index]*2));
    // Check if 500ms has elapsed => This should be a helper function at some point
    // Need to look if the PBCLK settings are stored in a define somewhere...
            if (counts > (timer_period*COUNTS_PER_MICROSECOND))
            {

            // determine the next state for the led and buzzer PWM
                led_level = !led_level;
                buzz_level = !buzz_level;

                // Reset the timer to 0
                g_timer0.p_api->reset(g_timer0.p_ctrl);
            }

      //write the state
      g_ioport.p_api->pinWrite(leds.p_leds[0],led_level);
      g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_04,buzz_level);
      duration_counts +=1;

      if (duration_counts >300000){
          f_index +=1;
          duration_counts = 0;
       }
      if (f_index>9){
          f_index = 1;
       }

    }

}

