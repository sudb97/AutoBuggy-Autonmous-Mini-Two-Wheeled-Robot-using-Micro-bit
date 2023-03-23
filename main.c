#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "bsp/board.h"
#include "bsp/lib.h"

#include "rtx/cmsis_os2.h"
#include "bsp/pwm.h"

#include "bsp/lsm303agr.h"
#include "bsp/i2c.h"

void delayMs(int n) //S/W one second delay
{

    int i, j;

    for(i = 0 ; i < n; i++)

       for(j = 0; j < 3180; j++);
}

void go(char dir,int speed){ //Motor controller for movement

    int duty;
    duty=(int) (speed*0.5 + 50); //conversion from speed to duty
    
    switch (dir)
    { 
    case 'f' : //forward
    pwm_out(338, duty, 1000);
    pwm1_out(338, 100-duty, 1000);
    break;
    

    case 'b' : //backward
    pwm_out(338, 100-duty, 1000);
    pwm1_out(338, duty, 1000);
    break;

    case 'l' :  //left
    pwm_out(338, 70, 1000);
    pwm1_out(338, 70, 1000);
    break;

    case 'r' :  //right
    pwm_out(338,40, 1000);
    pwm1_out(338,40, 1000);
    break;

    case 's' :   //stop
    pwm_out(338,52, 1000);
    pwm1_out(338,52, 1000);
    break;

    default:
    pwm_out(338,52, 1000);
    pwm1_out(338,52, 1000);
    break;
}
}

void trace_square() //tracing a square
{
    for (int i=0;i<=3;i++)
    {
    go('f',40);
    delay_ms(1000);
    go('s',40);
    delay_ms(1000);
    go('r',40);
    delay_ms(290);
    }
    pwmservo_stop();
}

int main()
{
//Speed can be in the range of 0-100%


    /* Initialisation */
    board_init();
    pwm_init(3);
    pwm1_init(4);
    //char dir[5]={'f','b','l','r','s'};
    

    uint8_t x_u;

    while(1)
    {
    x_u=COMPASSACCELERO_IO_Read(0X6a,0x12);
    printf("%c",x_u);
    printf("%d",1);
    }
    //moving in a square
    //trace_square();
    

    //Demonstration of all movements
    // while(1){
    // for (int i=0;i<=3;i++){

    //     go(dir[i]);
    //     delay_ms(1000);
    //     go('s');
    //     delay_ms(1000);
    // }

    //Implementing speed using duty cycle
    // go('f',55);
    // delay_ms(5000);
    // go('s',60);
    // delay_ms(1000);
    // go('f',95);
    // delay_ms(5000);
    // pwmservo_stop();
  }
