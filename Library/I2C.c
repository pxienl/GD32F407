#include "I2C.h"
#include "GPIO.h"
#include "systick.h"

void I2C_hardware_init(uint32_t i2c_periph,uint32_t speed){
         if(i2c_periph == I2C0) rcu_periph_clock_enable(RCU_I2C0);
    else if(i2c_periph == I2C1) rcu_periph_clock_enable(RCU_I2C1);
    else if(i2c_periph == I2C2) rcu_periph_clock_enable(RCU_I2C2);
    i2c_deinit(i2c_periph);
    i2c_clock_config(i2c_periph,speed,I2C_DTCY_2);
    i2c_enable(i2c_periph);
    i2c_ack_config(i2c_periph,I2C_ACK_ENABLE);
}


static uint8_t I2C_hardware_wait(uint32_t i2c_periph, uint32_t flag) {
    uint16_t cnt = 0;

    while(!i2c_flag_get(i2c_periph, flag)) {
        cnt++;
        if(cnt > I2C_HARDWARE_TIMEOUT) return 1;
    }
    return 0;
}

static uint8_t I2C_hardware_waitn(uint32_t i2c_periph, uint32_t flag) {
    uint16_t cnt = 0;

    while(i2c_flag_get(i2c_periph, flag)) {
        cnt++;
        if(cnt > I2C_HARDWARE_TIMEOUT) return 1;
    }
	return 0;
}

uint8_t I2C_hardware_start(uint32_t i2c_periph,uint8_t checkBSY){
    // 等待I2C闲置
    if(checkBSY && I2C_hardware_waitn(i2c_periph,I2C_FLAG_I2CBSY)) return 1;
    // start
    i2c_start_on_bus(i2c_periph);
    // 等待I2C主设备成功发送起始信号
    if(I2C_hardware_wait(i2c_periph,I2C_FLAG_SBSEND)) return 2;
    return 0;
}

uint8_t I2C_hardware_stop(uint32_t i2c_periph){
    // stop
    i2c_stop_on_bus(i2c_periph);

    if(I2C_hardware_waitn(i2c_periph,I2C_CTL0(i2c_periph)&I2C_CTL0_STOP)) return 8;
    return 0;
}

/*!
    \brief      master sends slave address
    \param[in]  i2c_periph: I2Cx(x=0,1,2)
    \param[in]  addr: slave address
    \param[in]  trandirection: transmitter or receiver
                only one parameter can be selected which is shown as below:
      \arg        I2C_TRANSMITTER: transmitter
      \arg        I2C_RECEIVER: receiver
    \param[out] none
    \retval     none
*/
uint8_t I2C_hardware_address(uint32_t i2c_periph, uint32_t addr, uint32_t trandirection){
    // 发送设备地址
    i2c_master_addressing(i2c_periph, addr, trandirection);
    // 等待地址发送完成
    if(I2C_hardware_wait(i2c_periph,I2C_FLAG_ADDSEND)) return 3;
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);
    return 0;
}

uint8_t I2C_hardware_send(uint32_t i2c_periph, uint8_t data){
    // 等待发送数据缓冲区为空
    if(I2C_hardware_wait(i2c_periph,I2C_FLAG_TBE)) return 6;

    // 发送数据
    i2c_data_transmit(i2c_periph, data);

    // 等待数据发送完成
    if(I2C_hardware_wait(i2c_periph,I2C_FLAG_BTC)) return 7;
    return 0;
}

static uint8_t I2C_hardware_recv(uint32_t i2c_periph, uint8_t* data, uint32_t len){
    i2c_ack_config(i2c_periph,I2C_ACK_ENABLE);
    i2c_ackpos_config(i2c_periph,I2C_ACKPOS_CURRENT);
    if(I2C_hardware_wait(i2c_periph,I2C_CTL0(i2c_periph) & I2C_CTL0_ACKEN)) return 4;

    for (uint32_t i = 0; i < len; i++)
    {
        /* code */
        if(i == len - 1){
            i2c_ack_config(i2c_periph,I2C_ACK_DISABLE);
        }
        if(I2C_hardware_wait(i2c_periph,I2C_FLAG_RBNE)) return 5;
        data[i] = i2c_data_receive(i2c_periph);
    }
    return 0;
}

void I2C_hardware_write(uint32_t i2c_periph, uint32_t addr, uint32_t reg,uint8_t* data, uint32_t len){
    I2C_hardware_start(i2c_periph,1);
    I2C_hardware_address(i2c_periph,addr,I2C_TRANSMITTER);
    I2C_hardware_send(i2c_periph,reg);
    for (uint32_t i = 0; i < len; i++)
    {
        I2C_hardware_send(i2c_periph,data[i]);
    }
    I2C_hardware_stop(i2c_periph);
}

void I2C_hardware_read(uint32_t i2c_periph, uint32_t addr, uint32_t reg,uint8_t* data, uint32_t len){
    I2C_hardware_start(i2c_periph,1);
    I2C_hardware_address(i2c_periph,addr & ~1,I2C_TRANSMITTER);
    I2C_hardware_send(i2c_periph,reg);

    I2C_hardware_start(i2c_periph,0);
    I2C_hardware_address(i2c_periph,addr | 1,I2C_RECEIVER);

    I2C_hardware_recv(i2c_periph,data,len);
    
    I2C_hardware_stop(i2c_periph);
}

static uint32_t I2C_soft_delay(uint32_t freq){
    return 1000 / freq / 2;
}

static inline void I2C_soft_output(I2C_soft_struct* i2c){
    GPIO_output_init(i2c->SCL_GPIO,GPIO_PUPD_NONE,GPIO_OTYPE_OD,i2c->SCL_PIN);
    GPIO_output_init(i2c->SDA_GPIO,GPIO_PUPD_NONE,GPIO_OTYPE_OD,i2c->SDA_PIN);
}

static inline void I2C_soft_input(I2C_soft_struct* i2c){
    GPIO_output_init(i2c->SCL_GPIO,GPIO_PUPD_NONE,GPIO_OTYPE_OD,i2c->SCL_PIN);
    GPIO_input_init(i2c->SDA_GPIO,GPIO_PUPD_NONE,i2c->SDA_PIN);
}

I2C_soft_struct I2C_soft_init(uint32_t SCL_GPIO, uint32_t SCL_PIN,uint32_t SDA_GPIO, uint32_t SDA_PIN, uint32_t freq){
    I2C_soft_struct i2c;
    i2c.SCL_GPIO = SCL_GPIO;
    i2c.SCL_PIN  = SCL_PIN;
    i2c.SDA_GPIO = SDA_GPIO;
    i2c.SDA_PIN  = SDA_PIN;
    i2c.delay_us = I2C_soft_delay(freq);
    GPIO_output_init(SCL_GPIO,GPIO_PUPD_NONE,GPIO_OTYPE_OD,SCL_PIN);
    GPIO_output_init(SDA_GPIO,GPIO_PUPD_NONE,GPIO_OTYPE_OD,SDA_PIN);

    return i2c;
}

static inline void I2C_soft_start(I2C_soft_struct* i2c){
    I2C_soft_output(i2c);

    gpio_bit_reset(i2c->SCL_GPIO,i2c->SCL_PIN);
    gpio_bit_set(i2c->SDA_GPIO,i2c->SDA_PIN);
    delay_1us(i2c->delay_us);

    gpio_bit_set(i2c->SCL_GPIO,i2c->SCL_PIN);
    delay_1us(i2c->delay_us);

    gpio_bit_reset(i2c->SDA_GPIO,i2c->SDA_PIN);
    delay_1us(i2c->delay_us);

    gpio_bit_reset(i2c->SCL_GPIO,i2c->SCL_PIN);
    delay_1us(i2c->delay_us);
}

static inline void I2C_soft_stop(I2C_soft_struct* i2c){
    I2C_soft_output(i2c);

    gpio_bit_reset(i2c->SCL_GPIO,i2c->SCL_PIN);
    gpio_bit_reset(i2c->SDA_GPIO,i2c->SDA_PIN);
    delay_1us(i2c->delay_us);

    gpio_bit_set(i2c->SCL_GPIO,i2c->SCL_PIN);
    delay_1us(i2c->delay_us);

    gpio_bit_set(i2c->SDA_GPIO,i2c->SDA_PIN);
    delay_1us(i2c->delay_us);
}

static inline void I2C_soft_send_bit(I2C_soft_struct* i2c,uint8_t bit){
    I2C_soft_output(i2c);

    gpio_bit_reset(i2c->SCL_GPIO,i2c->SCL_PIN);
    gpio_bit_write(i2c->SDA_GPIO,i2c->SDA_PIN,!bit);

    gpio_bit_write(i2c->SDA_GPIO,i2c->SDA_PIN,bit);
    gpio_bit_set(i2c->SCL_GPIO,i2c->SCL_PIN);
    delay_1us(i2c->delay_us);

    gpio_bit_reset(i2c->SCL_GPIO,i2c->SCL_PIN);
    gpio_bit_write(i2c->SDA_GPIO,i2c->SDA_PIN,!bit);
    delay_1us(i2c->delay_us);
}

static inline void I2C_soft_send_byte(I2C_soft_struct* i2c,uint8_t byte){
    I2C_soft_output(i2c);

    for (uint8_t i = 0; i < 8; i++)
    {
        I2C_soft_send_bit(i2c,byte & (0x80>>i));
    }
    
}

static uint8_t I2C_soft_recv_bit(I2C_soft_struct* i2c){
    uint8_t bit;
    I2C_soft_input(i2c);

    gpio_bit_set(i2c->SCL_GPIO,i2c->SCL_PIN);
    delay_1us(i2c->delay_us);

    bit = gpio_input_bit_get(i2c->SDA_GPIO,i2c->SDA_PIN);
    gpio_bit_reset(i2c->SCL_GPIO,i2c->SCL_PIN);
    delay_1us(i2c->delay_us);

    return bit;
}

static uint8_t I2C_soft_recv_byte(I2C_soft_struct* i2c){
    I2C_soft_input(i2c);
    uint8_t byte;
    for (uint8_t i = 0; i < 8; i++)
    {
        byte <<= 1;
        byte += I2C_soft_recv_bit(i2c);
    }
    return byte;
}

static uint8_t I2C_soft_wait_ack(I2C_soft_struct* i2c){
    I2C_soft_output(i2c);

    gpio_bit_reset(i2c->SCL_GPIO,i2c->SCL_PIN);
    gpio_bit_reset(i2c->SDA_GPIO,i2c->SDA_PIN);
    delay_1us(i2c->delay_us);

    gpio_bit_set(i2c->SDA_GPIO,i2c->SDA_PIN);
    delay_1us(i2c->delay_us);

    I2C_soft_input(i2c);
    gpio_bit_set(i2c->SCL_GPIO,i2c->SCL_PIN);
    delay_1us(i2c->delay_us);

    if(gpio_input_bit_get(i2c->SDA_GPIO,i2c->SDA_PIN)) return 1;
    
    gpio_bit_reset(i2c->SCL_GPIO,i2c->SCL_PIN);
    I2C_soft_output(i2c);
    gpio_bit_set(i2c->SDA_GPIO,i2c->SDA_PIN);
    delay_1us(i2c->delay_us);

    return 0;
}

static uint8_t I2C_soft_send_ack(I2C_soft_struct* i2c){
    I2C_soft_output(i2c);

    gpio_bit_reset(i2c->SCL_GPIO,i2c->SCL_PIN);
    gpio_bit_reset(i2c->SDA_GPIO,i2c->SDA_PIN);
    delay_1us(i2c->delay_us);

    gpio_bit_set(i2c->SCL_GPIO,i2c->SCL_PIN);
    delay_1us(i2c->delay_us);

    gpio_bit_reset(i2c->SCL_GPIO,i2c->SCL_PIN);
    delay_1us(i2c->delay_us);

    return 0;
}

static uint8_t I2C_soft_send_nack(I2C_soft_struct* i2c){
    I2C_soft_output(i2c);

    gpio_bit_reset(i2c->SCL_GPIO,i2c->SCL_PIN);
    gpio_bit_set(i2c->SDA_GPIO,i2c->SDA_PIN);
    delay_1us(i2c->delay_us);

    gpio_bit_set(i2c->SCL_GPIO,i2c->SCL_PIN);
    delay_1us(i2c->delay_us);

    gpio_bit_reset(i2c->SCL_GPIO,i2c->SCL_PIN);
    delay_1us(i2c->delay_us);

    return 0;
}

void I2C_soft_write(I2C_soft_struct* i2c,uint32_t addr, uint32_t reg,uint8_t* data, uint32_t len){

    I2C_soft_start(i2c);

    I2C_soft_send_byte(i2c,addr);
    if(I2C_soft_wait_ack(i2c)) return;

    I2C_soft_send_byte(i2c,reg);
    if(I2C_soft_wait_ack(i2c)) return;

    uint8_t* p = data;
    while(len--){
        I2C_soft_send_byte(i2c,*p);
        if(I2C_soft_wait_ack(i2c)) return;
        p++;
    }

    I2C_soft_stop(i2c);

}

void I2C_soft_read(I2C_soft_struct* i2c,uint32_t addr, uint32_t reg,uint8_t* data, uint32_t len){

    I2C_soft_start(i2c);

    I2C_soft_send_byte(i2c,addr & ~1);
    if(I2C_soft_wait_ack(i2c)) return;

    I2C_soft_send_byte(i2c,reg);
    if(I2C_soft_wait_ack(i2c)) return;

    I2C_soft_start(i2c);

    I2C_soft_send_byte(i2c,addr | 1);
    if(I2C_soft_wait_ack(i2c)) return;

    for (uint32_t i = 0; i < len; i++)
    {
        data[i] = I2C_soft_recv_byte(i2c);
        if(i != len-1) I2C_soft_send_ack(i2c);
        else I2C_soft_send_nack(i2c);
    }
    
    I2C_soft_stop(i2c);

}
