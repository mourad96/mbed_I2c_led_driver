#include <stdint.h>
#include "stm32l475xx.h"
#include "matrix.h"

#define RST(x)  do{GPIOC->BSRR = x ? GPIO_BSRR_BS3 : GPIO_BSRR_BR3; }while(0)
#define SB(x)   do{GPIOC->BSRR = x ? GPIO_BSRR_BS5 : GPIO_BSRR_BR5; }while(0)
#define LAT(x)  do{GPIOC->BSRR = x ? GPIO_BSRR_BS4 : GPIO_BSRR_BR4; }while(0)
#define SCK(x)  do{GPIOB->BSRR = x ? GPIO_BSRR_BS1 : GPIO_BSRR_BR1; }while(0)
#define SDA(x)  do{GPIOA->BSRR = x ? GPIO_BSRR_BS4 : GPIO_BSRR_BR4; }while(0)
#define ROW0(x) do{GPIOB->BSRR = x ? GPIO_BSRR_BS2 : GPIO_BSRR_BR2; }while(0)
#define ROW1(x) do{GPIOA->BSRR = x ? GPIO_BSRR_BS15 :GPIO_BSRR_BR15;}while(0)
#define ROW2(x) do{GPIOA->BSRR = x ? GPIO_BSRR_BS2 : GPIO_BSRR_BR2; }while(0)
#define ROW3(x) do{GPIOA->BSRR = x ? GPIO_BSRR_BS7 : GPIO_BSRR_BR7; }while(0)
#define ROW4(x) do{GPIOA->BSRR = x ? GPIO_BSRR_BS6 : GPIO_BSRR_BR6; }while(0)
#define ROW5(x) do{GPIOA->BSRR = x ? GPIO_BSRR_BS5 : GPIO_BSRR_BR5; }while(0)
#define ROW6(x) do{GPIOB->BSRR = x ? GPIO_BSRR_BS0 : GPIO_BSRR_BR0; }while(0)
#define ROW7(x) do{GPIOA->BSRR = x ? GPIO_BSRR_BS3 : GPIO_BSRR_BR3; }while(0)





void static inline deactivate_rows()
{
    ROW0(0);
    ROW1(0);
    ROW2(0);
    ROW3(0);
    ROW4(0);
    ROW5(0);
    ROW6(0);
    ROW7(0);
}
void static inline activate_row(int row)
{
    switch (row)
    {
    case 0:
        ROW0(1);
        break;
    case 1:
        ROW1(1);
        break;
    case 2:
        ROW2(1);
        break;
    case 3:
        ROW3(1);
        break;
    case 4:
        ROW4(1);
        break;
    case 5:
        ROW5(1);
        break;
    case 6:
        ROW6(1);
        break;
    case 7:
        ROW7(1);
        break;
    
    }
}

void static inline pulse_SCK()
{
   SCK(1);
   SCK(0);
}


void static inline pulse_LAT()
{
   LAT(0);
   LAT(1);  
}

void send_byte(uint8_t val, int bank)

{
    SB(bank);
 
    int i =7;
    while (i>=0)
    {
        SDA((val & ( 1 << i )) >> i);
        pulse_SCK();
        i--;
    }
}
 


void init_bank0()
{
    for (int i = 0; i < 18 ; i++)
    {
        send_byte(0xff,0);
    }
    pulse_LAT();
    
}

void mat_set_row(int row, const rgb_color *val)
{
    
     for (int i = 7; i >= 0; i--)
     {
         send_byte(val[i].b,1);
         send_byte(val[i].g,1);
         send_byte(val[i].r,1);
     } 
     deactivate_rows();
     for (int i=0; i<100; i++)//delay
           asm volatile("nop");
     pulse_LAT();
     activate_row(row);
}   

void matrix_init()
{
    //activation du clock du GPIOB ,GPIOA et GPIOC

    RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIOAEN);//IO port B clock enabled et IO port C clock enabled

    //GPIOB ,GPIOA et GPIOC en mode sortie
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE4_Msk) | GPIO_MODER_MODE4_0;
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE15_Msk) | GPIO_MODER_MODE15_0;
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE2_Msk) | GPIO_MODER_MODE2_0;
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE7_Msk) | GPIO_MODER_MODE7_0;
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE6_Msk) | GPIO_MODER_MODE6_0;
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE5_Msk) | GPIO_MODER_MODE5_0;
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE3_Msk) | GPIO_MODER_MODE3_0;


    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE1_Msk) | GPIO_MODER_MODE1_0;
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE2_Msk) | GPIO_MODER_MODE2_0;
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE0_Msk) | GPIO_MODER_MODE0_0;

    GPIOC->MODER = (GPIOC->MODER & ~GPIO_MODER_MODE5_Msk) | GPIO_MODER_MODE5_0;
    GPIOC->MODER = (GPIOC->MODER & ~GPIO_MODER_MODE4_Msk) | GPIO_MODER_MODE4_0;
    GPIOC->MODER = (GPIOC->MODER & ~GPIO_MODER_MODE3_Msk) | GPIO_MODER_MODE3_0;

    //GPIO Output avec la vitesse maximale (très haute vitesse)
    GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~ GPIO_OSPEEDR_OSPEED4_Msk) | GPIO_OSPEEDR_OSPEED4;
    GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~ GPIO_OSPEEDR_OSPEED15_Msk) | GPIO_OSPEEDR_OSPEED15;
    GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~ GPIO_OSPEEDR_OSPEED2_Msk) | GPIO_OSPEEDR_OSPEED2;
    GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~ GPIO_OSPEEDR_OSPEED7_Msk) | GPIO_OSPEEDR_OSPEED7;
    GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~ GPIO_OSPEEDR_OSPEED6_Msk) | GPIO_OSPEEDR_OSPEED6;
    GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~ GPIO_OSPEEDR_OSPEED5_Msk) | GPIO_OSPEEDR_OSPEED5;
    GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~ GPIO_OSPEEDR_OSPEED3_Msk) | GPIO_OSPEEDR_OSPEED3;

    GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~ GPIO_OSPEEDR_OSPEED1_Msk) | GPIO_OSPEEDR_OSPEED1;
    GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~ GPIO_OSPEEDR_OSPEED2_Msk) | GPIO_OSPEEDR_OSPEED2;
    GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~ GPIO_OSPEEDR_OSPEED0_Msk) | GPIO_OSPEEDR_OSPEED0;

    GPIOC->OSPEEDR = (GPIOC->OSPEEDR & ~ GPIO_OSPEEDR_OSPEED5_Msk) | GPIO_OSPEEDR_OSPEED5;
    GPIOC->OSPEEDR = (GPIOC->OSPEEDR & ~ GPIO_OSPEEDR_OSPEED4_Msk) | GPIO_OSPEEDR_OSPEED4;
    GPIOC->OSPEEDR = (GPIOC->OSPEEDR & ~ GPIO_OSPEEDR_OSPEED3_Msk) | GPIO_OSPEEDR_OSPEED3;
    //état initiale
    RST(0);//RST : 0  = PC3
    LAT(1);//LAT : 1 = PC4
    SB(1);//SB : 1 = PC5
    SCK(0);//SCK : 0 = PB1 
    SDA(0);//SDA :0 = PA4
    //C0 à C7 : 0
    deactivate_rows();

    // attendre au moins 100ms que le DM163 soit initialisé
    for (int i=0; i<5; i++)//delay
    {
        asm volatile("nop");
    }
    //RST passe à l'état haut.
    RST(1);
    init_bank0();

}

void test_pixels(rgb_color *val)
{
    for (int i = 0; i < 8; i++) {
        for (int c = 0; c < 8; c++) {
            if (i == 0 || i == 3 || i == 6 ) {
                val[c].b = 0xffU>>i;
                val[c].g = 0;
                val[c].r = 0;
            } else if (i == 1 || i == 4 || i == 7) {
                val[c].b = 0;
                val[c].g = 0xffU>>i;
                val[c].r = 0;
            } else {
                val[c].b = 0;
                val[c].g = 0;
                val[c].r = 0xffU>>i;
            }
        }
        mat_set_row(i,val);
    }
}


void set_test_image()
{
    /*char *start = &_binary_image_raw_start;
    for(int i = 0; i < 64;i++){
        image[i].r = *start++;
        image[i].g = *start++;
        image[i].b = *start++;
    }*/
    rgb_color image[64] ;
    for (int l = 0; l < 8; l++) {
        for (int c = 0; c < 8; c++) {
            if (l == 0 || l== 3 || l == 6 ) {
                image[l*8+c].b = 0xffU>>l;
                image[l*8+c].g = 0;
                image[l*8+c].r = 0;

            } else if (l == 1 || l == 4 || l == 7) {
                image[l*8+c].b = 0;
                image[l*8+c].g = 0xffU>>l;
                image[l*8+c].r = 0;

            } else {
                image[l*8+c].b = 0;
                image[l*8+c].g = 0;
                image[l*8+c].r = 0xffU>>l;
            }
        }
    }
}

void display_image(const rgb_color *image)
{
    
    for (int i = 0; i < 8; i++)
    {
        mat_set_row(i,image+(i*8)); 
    }
       
    
}
