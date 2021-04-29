#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "driver/mcpwm.h"

#define PWMpin 23
#define PWMFrequency 1

#define setPWM(DUTY) MCPWM0.channel[0].cmpr_value[0].val = ((4000/(100*PWMFrequency))*DUTY); /* Set the counter compare for DUTY% duty-cycle*/
#define PWMsetup \
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, PWMpin);      /* Initialise channel MCPWM0A on GPIO pin 27 */\
  MCPWM0.clk_cfg.prescale = 199;                 /* Set the 160MHz clock prescaler to 199 (160MHz/(199+1)=800kHz), 8 bit value*/\
  MCPWM0.timer[0].period.prescale = 199;         /* Set timer 0 prescaler to 199 (800kHz/(199+1))=4kHz), 8 bit value*/\
  MCPWM0.timer[0].period.period = (4000/PWMFrequency)-1;    /* Set the PWM period to 0.1Hz (4kHz/(39999+1)=0.1Hz), 16 bit value*/\
  MCPWM0.channel[0].cmpr_value[0].val = 0;       /* Set the counter compare for 0% duty-cycle*/\
  MCPWM0.channel[0].generator[0].utea = 2;       /* Set the PWM0A ouput to go low at the start of the timer period*/\
  MCPWM0.channel[0].generator[0].utez = 1;       /* High on compare match*/\
  MCPWM0.timer[0].mode.mode = 1;                 /* Set timer 0 to increment*/\
  MCPWM0.timer[0].mode.start = 2;                /* Set timer 0 to free-run*/
