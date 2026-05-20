#include <stdint.h>

#define PERIPH_BASE           0x40000000UL
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000UL)
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000UL)
#define APB1PERIPH_BASE       PERIPH_BASE

#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800UL)
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000UL)
#define USART2_BASE           (APB1PERIPH_BASE + 0x4400UL)
#define SPI1_BASE             (APB2PERIPH_BASE + 0x3000UL)
#define TIM1_BASE             (APB2PERIPH_BASE + 0x0000UL)

#define RCC_AHB1ENR     (*(volatile uint32_t*)(RCC_BASE + 0x30))
#define RCC_APB1ENR     (*(volatile uint32_t*)(RCC_BASE + 0x40))
#define RCC_APB2ENR     (*(volatile uint32_t*)(RCC_BASE + 0x44))

#define GPIOA_MODER     (*(volatile uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_AFRL      (*(volatile uint32_t*)(GPIOA_BASE + 0x20))
#define GPIOA_AFRH      (*(volatile uint32_t*)(GPIOA_BASE + 0x24))
#define GPIOA_BSRR      (*(volatile uint32_t*)(GPIOA_BASE + 0x18))

#define SPI1_CR1        (*(volatile uint32_t*)(SPI1_BASE + 0x00))
#define SPI1_SR         (*(volatile uint32_t*)(SPI1_BASE + 0x08))
#define SPI1_DR         (*(volatile uint32_t*)(SPI1_BASE + 0x0C))

#define TIM1_CR1        (*(volatile uint32_t*)(TIM1_BASE + 0x00))
#define TIM1_CCMR1      (*(volatile uint32_t*)(TIM1_BASE + 0x18))
#define TIM1_CCER       (*(volatile uint32_t*)(TIM1_BASE + 0x20))
#define TIM1_PSC        (*(volatile uint32_t*)(TIM1_BASE + 0x28))
#define TIM1_ARR        (*(volatile uint32_t*)(TIM1_BASE + 0x2C))
#define TIM1_CCR1       (*(volatile uint32_t*)(TIM1_BASE + 0x34))
#define TIM1_BDTR       (*(volatile uint32_t*)(TIM1_BASE + 0x44))
#define TIM1_EGR        (*(volatile uint32_t*)(TIM1_BASE + 0x14))

#define MCP_RESET 0xC0
#define MCP_READ  0x03
#define MCP_WRITE 0x02

#define CANCTRL   0x0F
#define CNF1      0x2A
#define CNF2      0x29
#define CNF3      0x28

#define RXB0CTRL  0x60
#define RXB0SIDH  0x61
#define RXB0SIDL  0x62
#define RXB0D0    0x66
#define RXB0D1    0x67
#define CANINTF   0x2C

void delay_ms(uint32_t ms) {

    volatile uint32_t count = ms * 4000;

    while(count--);
}

void SPI1_Init() {

    RCC_APB2ENR |= (1 << 12);

    GPIOA_MODER |= (2U<<10)|(2U<<12)|(2U<<14)|(1U<<8);

    GPIOA_AFRL  |= (5U<<20)|(5U<<24)|(5U<<28);

    GPIOA_BSRR = (1<<4);

    SPI1_CR1 = (1<<2)|(7<<3)|(1<<8)|(1<<9);

    SPI1_CR1 |= (1<<6);
}

uint8_t SPI1_Transfer(uint8_t data) {

    while(!(SPI1_SR & (1<<1)));

    *(volatile uint8_t*)&SPI1_DR = data;

    while(!(SPI1_SR & (1<<0)));

    return *(volatile uint8_t*)&SPI1_DR;
}

#define CS_LOW()   (GPIOA_BSRR = (1<<20))
#define CS_HIGH()  (GPIOA_BSRR = (1<<4))

void MCP_Write(uint8_t reg, uint8_t val) {

    CS_LOW();

    SPI1_Transfer(MCP_WRITE);
    SPI1_Transfer(reg);
    SPI1_Transfer(val);

    CS_HIGH();
}

uint8_t MCP_Read(uint8_t reg) {

    uint8_t val;

    CS_LOW();

    SPI1_Transfer(MCP_READ);
    SPI1_Transfer(reg);

    val = SPI1_Transfer(0xFF);

    CS_HIGH();

    return val;
}

void MCP_Init() {

    CS_LOW();

    SPI1_Transfer(MCP_RESET);

    CS_HIGH();

    delay_ms(10);

    MCP_Write(CANCTRL, 0x80);

    MCP_Write(CNF1, 0x03);
    MCP_Write(CNF2, 0x90);
    MCP_Write(CNF3, 0x02);

    MCP_Write(RXB0CTRL, 0x00);

    MCP_Write(CANCTRL, 0x00);
}

void Motor_Init() {

    RCC_APB2ENR |= (1<<0);

    GPIOA_MODER |= (2U<<16);

    GPIOA_AFRH  |= (1U<<0);

    TIM1_PSC = 15;
    TIM1_ARR = 100;

    TIM1_CCMR1 |= (6<<4) | (1<<3);

    TIM1_CCER |= (1<<0);

    TIM1_BDTR |= (1<<15);

    TIM1_EGR |= (1<<0);

    TIM1_CR1 |= (1<<0);
}

uint8_t CAN_Read(uint16_t *value) {

    uint8_t flag = MCP_Read(CANINTF);

    if(!(flag & 1))
        return 0;

    uint16_t id =
        (MCP_Read(RXB0SIDH) << 3) |
        (MCP_Read(RXB0SIDL) >> 5);

    if(id != 0x123) {

        MCP_Write(CANINTF, flag & ~1);

        return 0;
    }

    uint8_t d0 = MCP_Read(RXB0D0);
    uint8_t d1 = MCP_Read(RXB0D1);

    *value = (d0 << 8) | d1;

    MCP_Write(CANINTF, flag & ~1);

    return 1;
}

int main() {

    uint16_t potValue;
    uint32_t duty;

    SPI1_Init();

    MCP_Init();

    Motor_Init();

    while(1) {

        if(CAN_Read(&potValue)) {

            if(potValue <= 1023)
                duty = 25;

            else if(potValue <= 2047)
                duty = 50;

            else if(potValue <= 3071)
                duty = 75;

            else
                duty = 100;

            TIM1_CCR1 = duty;
        }
    }
}
