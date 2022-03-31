#include <mbed.h>
#include "i2c.h"
#include "matrix.h"
#include "main.h"

char data_read[4];
char scratchpad[4];
char data_write[192];
I2CSlave slave(D14, D15);
Thread i2c_thread;

/*DigitalOut led(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);*/

EventFlags event_flags;
//MemoryPool<rgb_color, 2> mpool;
Queue<rgb_color, 2> matrix_full;
Queue<rgb_color, 2> matrix_empty;


static int wait_for_request(void)
{   
    int ret;
    int i = slave.receive();
    while (i == I2CSlave::NoData)
    {
        i = slave.receive();
    }
    
    switch (i) {
        case I2CSlave::ReadAddressed:
            //slave.write(msg, strlen(msg) + 1); // Includes null char
            ret = 0;
            break;
        case I2CSlave::WriteAddressed:
            //slave.read(buf, 10);
            ret = 1;
            break;
        }
    return ret;
}

static int answer_read_request(const char *data, size_t len) 
{   
    int ret = wait_for_request();
    if (ret == 0)
    {
        slave.write(data, len + 1);
        return 0;
    }else{
        return 1;//
    }
}

static int get_write_request_parameters(char *data, size_t len)
{
    int ret = wait_for_request();
    if (ret == 1)
    {
        slave.read(data, len);
        return 0;
    }else{
        return 1;//
    }
}

static void blink_matrix()
{   
    rgb_color *current_buffer;
    bool evt;
    evt = matrix_empty.try_get_for(Kernel::wait_for_u32_forever,&current_buffer);
    get_write_request_parameters((char *)current_buffer, 192);
    matrix_full.try_put(current_buffer);
}
/*void led_blink()
{
    if (data_read[0] & 0x01)
    {
        led = !led;
    }
    else if (data_read[0] & 0x02)
    {
        led2 = !led2;
    }
    else if (data_read[0] & 0x04)
    {
        led3 = !led3;
    } 
}*/
static void handle_command()
{
    int ret = get_write_request_parameters(data_read, 1);
    if (ret == 0){
        switch (data_read[0])
        {
        case 0x83:
            data_write[0] =0x2A;
            ret = answer_read_request(data_write, 1);
            printf("address = %x\n", data_write[0] );
            break;
        case 0x82:
            ret = answer_read_request(scratchpad, 4);
            break;
        case 0x80:
            data_write[0]=button_counter;
            ret = answer_read_request(data_write, 1);
            button_counter = 0;
            //printf("button_counter = %d\n", data_write[0] );
            break;
        ////write to buffer
        case 0x10:
            blink_matrix();
            break;
        case 0x06:
            ret = get_write_request_parameters(data_read, 1);
            //led_blink();
            break;
        case 0x02:
            ret = get_write_request_parameters(scratchpad, 4);
            break;
        }
    }else{
        //error
    }
}
static void loop()
{
    //int ret ;
    while (1)
    {
        handle_command();
    }
    
}

void init_i2c(){
  slave.address(0x2A<<1);
  i2c_thread.start(mbed::callback(loop));
}