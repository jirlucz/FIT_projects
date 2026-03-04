/*
 * Copyright 2016-2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    MKL27Z644_Project.c
 * @author Jiri Kotek, xkotek09, xkotek09@stud.fit.vutbr.cz
 */
#include <stdio.h>
#include "board.h"
#include "main.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

#define SEC_3_VALUE 0x0011
#define SEC_5_VALUE 0x0101
#define SEC_10_VALUE 0x1010
#define SEC_15_VALUE 0x1111
#define SEC_20_VALUE 0x10100

#define LED1 0x04
#define LED2 0x01
#define LED3 0x02
#define LED4 0x02
#define LED5 0x100000
#define LED6 0x01
#define LED8 0x20
#define LED9 0x80
#define LED10 0x40
#define LED11 0x200
#define LED12 0x100
#define LED13 0x40
#define LED14 0x10
#define LED15 0x20
#define LED17 0x2000000
#define LED18 0x1000

#define BTN1 0x200000
#define BTN2 0x10000
#define BTN3 0x1000000

#define RTC_SR_TCE_ENABLE 0x10 /* Time Counter Enable */

int speed_multiplier = 3; /* Multiplies time, that will be spend in each state */

enum Zebra last_zebra = CARS_MAIN;
enum Road last_road = MAIN;
enum State actual_state = STOP;
enum State next_state = STOP;
bool zebra_crossing_pressed = false; /* If buttons are pressed, value is true */


/* M1 semaphore*/
void set_M1_red(void){
	GPIOA->PSOR |= LED1; /*ON*/
	GPIOB->PCOR |= LED2;
	GPIOB->PCOR |= LED3;
}
void set_M1_orange(void){
	GPIOA->PCOR |= LED1;
	GPIOB->PSOR |= LED2; /*ON*/
	GPIOB->PCOR |= LED3;
}

void set_M1_green(void){
	GPIOA->PCOR |= LED1;
	GPIOB->PCOR |= LED2;
	GPIOB->PSOR |= LED3; /*ON*/
}

void set_M1_red_orange(void){
	GPIOA->PSOR |= LED1; /*ON*/
	GPIOB->PSOR |= LED2; /*ON*/
	GPIOB->PCOR |= LED3;
}

/* M2 semaphore*/
void set_M2_red(void){
	GPIOC->PSOR |= LED9; /*ON*/
	GPIOC->PCOR |= LED8;
	GPIOD->PCOR |= LED10;
}
void set_M2_orange(void){
	GPIOC->PCOR |= LED9;
	GPIOC->PSOR |= LED8; /*ON*/
	GPIOD->PCOR |= LED10;
}

void set_M2_green(void){
	GPIOC->PCOR |= LED9;
	GPIOC->PCOR |= LED8;
	GPIOD->PSOR |= LED10; /*ON*/
}

void set_M2_red_orange(void){
	GPIOC->PSOR |= LED9; /*ON*/
	GPIOC->PSOR |= LED8; /*ON*/
	GPIOD->PCOR |= LED10;
}

/* M3 semaphore*/
void set_M3_red(void){
	GPIOA->PSOR |= LED15; /*ON*/
	GPIOC->PCOR |= LED14;
	GPIOC->PCOR |= LED13;
}
void set_M3_orange(void){
	GPIOA->PCOR |= LED15;
	GPIOC->PSOR |= LED14; /*ON*/
	GPIOC->PCOR |= LED13;
}

void set_M3_green(void){
	GPIOA->PCOR |= LED15;
	GPIOC->PCOR |= LED14;
	GPIOC->PSOR |= LED13; /*ON*/
}

void set_M3_red_orange(void){
	GPIOA->PSOR |= LED15; /*ON*/
	GPIOC->PSOR |= LED14; /*ON*/
	GPIOC->PCOR |= LED13;
}

/* Z1 semaphore*/
void set_Z1_red(void){
	GPIOE->PSOR |= LED5; /*ON*/
	GPIOC->PCOR |= LED6;
}
void set_Z1_green(void){
	GPIOE->PCOR |= LED5;
	GPIOC->PSOR |= LED6; /*ON*/
}

/* Z2 semaphore*/
void set_Z2_red(void){
	GPIOC->PSOR |= LED11; /*ON*/
	GPIOC->PCOR |= LED12;
}
void set_Z2_green(void){
	GPIOC->PCOR |= LED11;
	GPIOC->PSOR |= LED12; /*ON*/
}

/* Z3 semaphore*/
void set_Z3_red(void){
	GPIOE->PSOR |= LED17; /*ON*/
	GPIOA->PCOR |= LED18;
}
void set_Z3_green(void){
	GPIOE->PCOR |= LED17;
	GPIOA->PSOR |= LED18; /*ON*/
}


void delay(uint64_t bound) {
	for (uint64_t i=0; i < bound; i++) { __NOP(); }
}

void flash(void) {
	GPIOA->PSOR |= LED4;
	delay(12000);
	GPIOA->PCOR |= LED4;
}

void flash2(void) {
	GPIOC->PSOR |= LED6;
	delay(12000);
	GPIOC->PCOR |= LED6;
}

void Port_init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;  /* Enable clock for port PORTA */
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;  /* Enable clock for port PORTB */
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;  /* Enable clock for port PORTC */
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;  /* Enable clock for port PORTD */
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;  /* Enable clock for port PORTE */

    /* Set pins to GPIO */
    PORTA->PCR[1] = PORT_PCR_MUX(1);
    PORTA->PCR[2] = PORT_PCR_MUX(1);
    PORTA->PCR[5] = PORT_PCR_MUX(1);
    PORTA->PCR[12] = PORT_PCR_MUX(1);

    PORTB->PCR[0] = PORT_PCR_MUX(1);
    PORTB->PCR[1] = PORT_PCR_MUX(1);

    PORTC->PCR[0] = PORT_PCR_MUX(1);
    PORTC->PCR[4] = PORT_PCR_MUX(1);
    PORTC->PCR[5] = PORT_PCR_MUX(1);
    PORTC->PCR[6] = PORT_PCR_MUX(1);
    PORTC->PCR[7] = PORT_PCR_MUX(1);
    PORTC->PCR[8] = PORT_PCR_MUX(1);
    PORTC->PCR[9] = PORT_PCR_MUX(1);

    PORTD->PCR[6] = PORT_PCR_MUX(1);
    PORTD->PCR[7] = PORT_PCR_MUX(1);

    PORTE->PCR[16] = PORT_PCR_MUX(1);
    PORTE->PCR[20] = PORT_PCR_MUX(1);
    PORTE->PCR[21] = PORT_PCR_MUX(1);
    PORTE->PCR[24] = PORT_PCR_MUX(1);
    PORTE->PCR[25] = PORT_PCR_MUX(1);
    PORTE->PCR[31] = PORT_PCR_MUX(1);

    /* Set output pins  */
    GPIOA->PDDR |= (1 << 1);             /*  */
    GPIOA->PDDR |= (1 << 2);             /*  */
    GPIOA->PDDR |= (1 << 5);             /*  */
    GPIOA->PDDR |= (1 << 12);            /*  */
    GPIOB->PDDR |= (1 << 0);             /*  */
    GPIOB->PDDR |= (1 << 1);             /*  */

    GPIOC->PDDR |= (1 << 0);             /*  */
    GPIOC->PDDR |= (1 << 4);             /*  */
    GPIOC->PDDR |= (1 << 5);             /*  */
    GPIOC->PDDR |= (1 << 6);             /*  */
    GPIOC->PDDR |= (1 << 7);             /*  */
    GPIOC->PDDR |= (1 << 8);             /*  */
    GPIOC->PDDR |= (1 << 9);             /*  */

    GPIOD->PDDR |= (1 << 6);             /*  */
    GPIOD->PDDR |= (1 << 7);             /*  */

    GPIOE->PDDR |= (1 << 20);            /*  */
    GPIOE->PDDR |= (1 << 25);            /*  */
    GPIOE->PDDR |= (1 << 31);            /*  */

    /* Set buttons as input */
    GPIOE->PDDR &= ~(1 << 16);  /* BTN1 */
    GPIOE->PDDR &= ~(1 << 21);  /* BTN2 */
    GPIOE->PDDR &= ~(1 << 24);  /* BTN3 */

}

/* Turns on all LEDs */
void LED_On(void) {
	GPIOA->PSOR |= LED4;			/* LED4 */
	GPIOA->PSOR |= LED1;			/* LED1 */
	GPIOA->PSOR |= LED15;			/* LED15 0x20 */
	GPIOA->PSOR |= LED18;			/* LED18 Z-Ch 0x1000*/

	GPIOB->PSOR |= LED2;            /* LED2 0x01 */
	GPIOB->PSOR |= LED3;            /* LED3 0x02 */

	GPIOC->PSOR |= LED6;            /* LED6 0x01 */
	GPIOC->PSOR |= LED14;           /* LED14 0x10*/
	GPIOC->PSOR |= LED8;            /* LED8 0x20 */
	GPIOC->PSOR |= LED13;           /* LED13 0x40 */
	GPIOC->PSOR |= LED9;            /* LED9 0x80 */
	GPIOC->PSOR |= LED12;           /* LED12 0x100 */
	GPIOC->PSOR |= LED11;           /* LED11 0x200 */

	GPIOD->PSOR |= LED10;           /* LED10 0x40 */
	GPIOD->PSOR |= (1 << 7);        /* Additional LED near LED9 0x80 */

	GPIOE->PSOR |= LED5;            /* LED5 0x100000 */
	GPIOE->PSOR |= LED17;           /* LED17 0x2000000 */
	GPIOE->PSOR |= (1 << 31);       /* Additional LED near LED15 0x80000000 */
}

/* Turns off all LEDs */
void LED_Off(void) {
	GPIOA->PCOR = 0xFFFFFFFF;
	GPIOB->PCOR = 0xFFFFFFFF;
	GPIOC->PCOR = 0xFFFFFFFF;
	GPIOD->PCOR = 0xFFFFFFFF;
	GPIOE->PCOR = 0xFFFFFFFF;
}

/* Function that handles finite automata */
void Crossroad_handle(void){
	if(actual_state == MAIN_ROAD_GREEN){
		set_M1_green();
		set_M2_green();
		set_M3_red();
		set_Z1_red();
		set_Z2_red();
		set_Z3_red();
		next_state = MAIN_ROAD_ORANGE;
	}
	else if(actual_state == MAIN_ROAD_ORANGE){
		set_M1_orange();
		set_M2_orange();
		set_M3_red();
		set_Z1_red();
		set_Z2_red();
		set_Z3_red();
		next_state = STOP;
	}
	else if(actual_state == MAIN_ROAD_PREPARE){
		set_M1_red_orange();
		set_M2_red_orange();
		set_M3_red();
		set_Z1_red();
		set_Z2_red();
		set_Z3_red();
		next_state = MAIN_ROAD_GREEN;
		last_road = MAIN;
		last_zebra = CARS_MAIN;
	}
	else if(actual_state == SIDE_ROAD_GREEN){
		set_M1_red();
		set_M2_red();
		set_M3_green();
		set_Z1_red();
		set_Z2_red();
		set_Z3_red();
		next_state = SIDE_ROAD_ORANGE;
	}
	else if(actual_state == SIDE_ROAD_ORANGE){
		set_M1_red();
		set_M2_red();
		set_M3_orange();
		set_Z1_red();
		set_Z2_red();
		set_Z3_red();
		next_state = STOP;
	}
	else if(actual_state == SIDE_ROAD_PREPARE){
		set_M1_red();
		set_M2_red();
		set_M3_red_orange();
		set_Z1_red();
		set_Z2_red();
		set_Z3_red();
		next_state = SIDE_ROAD_GREEN;
		last_road = SIDE;
		last_zebra = CARS_SIDE;
	}
	else if(actual_state == ZEBRA_CROSSING){
		set_M1_red();
		set_M2_red();
		set_M3_red();
		set_Z1_green();
		set_Z2_green();
		set_Z3_green();
		next_state = ZEBRA_CROSSING_STOP;
	}
	else if(actual_state == ZEBRA_CROSSING_STOP){
		set_M1_red();
		set_M2_red();
		set_M3_red();
		set_Z1_red();
		set_Z2_red();
		set_Z3_red();
		next_state = STOP;
		last_zebra = WALKERS;
		zebra_crossing_pressed = false;
		}
	else if(actual_state == STOP){
		set_M1_red();
		set_M2_red();
		set_M3_red();
		set_Z1_red();
		set_Z2_red();
		set_Z3_red();

		/* Switches between main and side road */
		if(last_road == SIDE){
			next_state = MAIN_ROAD_PREPARE;
		}
		else{
			next_state = SIDE_ROAD_PREPARE;
		}

		/* Switches to zebra crossing if any button was pressed */
		if(last_zebra == CARS_SIDE && zebra_crossing_pressed == true){
			next_state = ZEBRA_CROSSING;

		}
		else{
			last_zebra = CARS_MAIN;
		}
	}
	return;
}


/* Button interrupt handler */
void PORTB_PORTC_PORTD_PORTE_DriverIRQHandler(void) {
	zebra_crossing_pressed = true;
    /* Clear the interrupt flag */
    PORTE->ISFR = (1 << 16) | (1 << 21) | (1 << 24);
}

/* Enables button interrupts */
void Button_interrupt_init(void){

	PORTE->PCR[16] |= PORT_PCR_IRQC(0xA);  // Enable interrupt on falling edge for BTN1
	PORTE->PCR[21] |= PORT_PCR_IRQC(0xA);  // Enable interrupt on falling edge for BTN2
	PORTE->PCR[24] |= PORT_PCR_IRQC(0xA);  // Enable interrupt on falling edge for BTN3
	PORTE->ISFR = (1 << 16) | (1 << 21) | (1 << 24);

	/* Enable GPIO interrupt in the NVIC */
	NVIC_ClearPendingIRQ(31); /* Clear interrupt flag on PORT B */
	NVIC_EnableIRQ(31); /* Enable PORT B interrupt in NVIC */
}

/* Timer function for each state */
int state_timer(void){
	if(actual_state == MAIN_ROAD_GREEN){
		return 10/speed_multiplier;
	}
	else if(actual_state == MAIN_ROAD_ORANGE){
		return 2/speed_multiplier;
	}
	else if(actual_state == MAIN_ROAD_PREPARE){
		return 2/speed_multiplier;
	}
	else if(actual_state == SIDE_ROAD_GREEN){
		return 10/speed_multiplier;
	}
	else if(actual_state == SIDE_ROAD_ORANGE){
		return 2/speed_multiplier;
	}
	else if(actual_state == SIDE_ROAD_PREPARE){
		return 2/speed_multiplier;
	}
	else if(actual_state == ZEBRA_CROSSING){
		return 15/speed_multiplier;
	}
	else if(actual_state == ZEBRA_CROSSING_STOP){
		return 5/speed_multiplier;
	}
	else if(actual_state == STOP){
		return 3/speed_multiplier;
	}
	return 1/speed_multiplier;
}

/* Time interrupt handler */
void RTC_IRQHandler(void) {
	/*flash2(); */
	actual_state = next_state;
	RTC->SR &= ~RTC_SR_TAF_MASK;
	Crossroad_handle();
	RTC->TSR = 0;
	RTC->TAR  = RTC->TSR + state_timer();
}

/* Help function, handles seconds interrupts*/
void RTC_Seconds_IRQHandler(void) {
	/*flash2();*/
    RTC->SR &= ~RTC_SR_TIF_MASK;
}


/* Sets real time clock module */
void RTC_init(void){
	SIM->SCGC6 |= SIM_SCGC6_RTC_MASK;  		/* Enable clock to the RTC module */
	SIM->SOPT1 |= SIM_SOPT1_OSC32KSEL(0); 	/* Select 32kHz oscillator as the RTC source clock */

	RTC->CR = RTC_CR_OSCE_MASK; 			/* Enable RTC oscillator */

	RTC->TPR = (32768);  					/* Set RTC prescaler */

	RTC->IER |= RTC_IER_TAIE_MASK; 			/* Enable Time Alarm Interrupt */
	RTC->IER |= RTC_IER_TSIE_MASK;			/* Time Seconds Interrupt Enable */

	RTC->TSR = 0x00; 						/* Set counter to 0 */
	RTC->SR &= ~RTC_SR_TCE_MASK;			/* Disable Time Counter explicitly for configuration */
	RTC->TAR  = RTC->TSR + 1;				/* Set the alarm for the default time (+1s) */

	NVIC_ClearPendingIRQ(RTC_IRQn); 		/* 20 - Alarm Interrupt */
	NVIC_EnableIRQ(RTC_IRQn); 				/* Enable interrupt in NVIC */

	#if 0	/* Debug */
	NVIC_ClearPendingIRQ(RTC_Seconds_IRQn); /* 20 - Alarm Interrupt */
	NVIC_EnableIRQ(RTC_Seconds_IRQn); 		/* Enable interrupt in NVIC */
	#endif	/* End Debug */

	RTC->SR |= RTC_SR_TCE_MASK;				/* Enable Time Counter */
	RTC->SR |= RTC_SR_TOF_MASK;				/* Time Overflow Flag */
	RTC->SR |= RTC_SR_TAF_MASK;				/* Set Time Alarm Flag */

}

/* Set TAR register to timer */
void RTC_Timer_set(uint64_t timer){
	RTC->TAR  = timer;  /* Set the alarm for the correct time */
	RTC->SR |= RTC_SR_TAF_MASK;  /* Set the Time Alarm Flag */
}

/* MCUXpresso generated HW initialization */
void HW_init(void){
	/* Init board hardware. */
	    BOARD_InitBootPins();
	    BOARD_InitBootClocks();
	    BOARD_InitBootPeripherals();
	#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	    /* Init FSL debug console. */
	    BOARD_InitDebugConsole();
	#endif

}

/* Main function */
int main(void) {
	HW_init();
	Port_init();
	/*flash2();*/
    Button_interrupt_init();
    RTC_init();

    while(1) {
    	__WFI();  /* Wait for interrupt (low-power mode) */
    }
    return 0 ;
}
