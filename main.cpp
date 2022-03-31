#include <mbed.h>
#include "matrix.h"
#include "main.h"
#include "i2c.h"



rgb_color buf_A[64] ;
rgb_color buf_B[64] ;


uint8_t button_counter = 0;


InterruptIn BP0Interrupt(PC_13); //user button

void isrBP0 ()
{
    event_flags.set(0x1);
    button_counter++;
}



void button_led()
{
    uint32_t flags_read = 0;
    while (true) {
        flags_read = event_flags.wait_any( 1, osWaitForever, true);
        if (flags_read)
        {
            ThisThread::sleep_for(100ms);
            //event_flags.clear();
           // led2 = !led2;
        }
    }
}
void display_thread()
{
     rgb_color *current_buffer;
     bool evt = matrix_full.try_get_for(Kernel::wait_for_u32_forever,&current_buffer);
     //printf("evt = %s \n",evt ? "true" : "false");
    while (true) {
        evt = matrix_full.empty();
        //printf("evt = %s \n",evt ? "true" : "false");
        if (evt == false)
        {   
            matrix_full.try_get_for((Kernel::Clock::duration_u32)0,&current_buffer);
            matrix_empty.try_put(current_buffer);
            display_image(current_buffer);
        }
        else{
            display_image(current_buffer);
        }
       // printf("thread \n");
        //wait_us(1*1000);
    }
}
/*void blink_matrix()
{   
    rgb_color *current_buffer;
    bool evt;
    while (true) {
        evt = matrix_empty.try_get_for(Kernel::wait_for_u32_forever,&current_buffer);
        for (int i = 0; i < 64; i++)
        {
            current_buffer[i].b = 0;
            current_buffer[i].r = 255;
            current_buffer[i].g = 0;
        }
        matrix_full.try_put(current_buffer);
        ThisThread::sleep_for(500ms);
        evt = matrix_empty.try_get_for(Kernel::wait_for_u32_forever,&current_buffer);
        for (int i = 0; i < 64; i++)
        {
            current_buffer[i].b = 0;
            current_buffer[i].r = 0;
            current_buffer[i].g = 255;
        }
        matrix_full.try_put(current_buffer);
        ThisThread::sleep_for(500ms);
    }
}*/
int main() {
    matrix_init();
    init_i2c();
    rgb_color *message = buf_A;
    rgb_color *message1 = buf_B;
    matrix_full.try_put(message);
    bool r_a = matrix_empty.try_put(message);
    bool r_b =matrix_empty.try_put(message1);
    printf("r_a = %s \n",r_a ? "true" : "false");
    printf("r_b = %s \n",r_b ? "true" : "false");
    BP0Interrupt.mode(PullUp);
    BP0Interrupt.fall(&isrBP0);
    Thread worker_thread,display_thread1;
    ////start thread
    worker_thread.start(mbed::callback(button_led));
    display_thread1.start(mbed::callback(display_thread));
    //blink_matrix1.start(mbed::callback(blink_matrix));

    

  for (;;) {
    //led = !led;
    ThisThread::sleep_for(1s);
  }
}