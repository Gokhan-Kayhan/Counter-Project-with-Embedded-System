#include <msp430g2553.h> 
#include <lcdLib.h>

volatile double timer_value = 0; 
volatile double time_1 = 0; 
volatile double time_2 = 0;
int counter_in = 0;
int counter_out = 0;

#pragma vector=TIMER1_A0_VECTOR __interrupt void TimerA0(void) {
timer_value++; 
}

void main(void) {
WDTCTL = WDTPW | WDTHOLD;
BCSCTL1 = CALBC1_1MHZ; DCOCTL = CALDCO_1MHZ;
P1OUT = 0;
P1DIR |= BIT6+BIT0; // Leds
P1DIR |= BIT2;  // PWM Led
P1DIR &= ~BIT3; // Sensor 
P1DIR &= ~BIT5; // Sensor
P1DIR &= ~(BIT4+BIT7); //Buttons
P1SEL |= BIT2;


// stop watchdog timer
P1REN |= BIT3; 
P1OUT |= BIT3; 
P1REN |= BIT5; 
P1OUT |= BIT5; 
P1OUT |=(BIT4+BIT7); 
P1REN |=(BIT4+BIT7);

P1IES |= BIT3;
P1IES |= BIT5;
P1IES |= (BIT4+BIT7);

P1IFG &= ~BIT3;
P1IFG &= ~BIT5;
P1IFG &= ~(BIT4+BIT7);

P1IE |= BIT3;
P1IE |= BIT5;
P1IE |= (BIT4+BIT7);

TA1CCTL0 = CCIE;
TA1CTL = TASSEL_2 + MC_1 + ID_0; 
TA1CCR0 = 1000;

TA0CCR0 = 1000;
TA0CCTL1 = OUTMOD_7; 
TA0CCR1 = 0;
TA0CTL = TASSEL_2 + MC_1;

lcdInit();

while(1) {
    _BIS_SR(GIE);

    if(time_1 != 0 & time_2 != 0) {
        if (time_1 > time_2) {
            counter_out++; timer_value = 0;
            time_1 = 0;
            time_2 = 0;
            P1OUT &= ~(BIT0+BIT6);
        }

        if( time_2 > time_1) {
            counter_in++; timer_value = 0;
            time_1 = 0;
            time_2 = 0;
            P1OUT &= ~(BIT0+BIT6);
        } 
    }

    lcdSetText( "IN" , 0 , 0 ); 
    lcdSetText( "OUT" , 8 , 0 ); 
    lcdSetInt(counter_in, 0,1); 
    lcdSetInt(counter_out, 8,1);

    if( (counter_in - counter_out) > 5 ) {
    TA0CCR1 = 1000;
    }

    else if ( (counter_in - counter_out) < 0) {
    TA0CCR1 = 0; 
    }

    else {
    TA0CCR1 = ( (counter_in - counter_out)*200 ); 
    }

} 
}
#pragma vector = PORT1_VECTOR __interrupt void button(void) {

    if (P1IFG & BIT3) {
        time_1 = timer_value;
        P1OUT |= BIT0; 
        __delay_cycles(500); 
        P1IFG &= ~BIT3;
        }

    if (P1IFG & BIT5) {
        time_2 = timer_value; 
        P1OUT |= BIT6;
        __delay_cycles(500);
        P1IFG &= ~BIT5; 
        }

    if (P1IFG & BIT7) {
        lcdClear();
        if ( (counter_in - counter_out) < 0) {
            lcdSetText( "Total Person" , 0 , 0 ); lcdSetInt(0 , 0 , 1);
            lcdSetText( "!" , 2 , 1 ); __delay_cycles(1000000); lcdClear();
        }

        else {
        lcdSetText( "Total Person" , 0 , 0 ); 
        lcdSetInt(counter_in - counter_out, 0,1); 
        __delay_cycles(1000000);
        lcdClear();
        }

        P1IFG &= ~BIT7;
    }

    if (P1IFG & BIT4) {
    lcdClear();

    timer_value = 0;
    time_1 = 0; 
    time_2 = 0; 
    counter_in = 0; 
    counter_out = 0; 
    P1IFG &= ~BIT4;
    } 
}