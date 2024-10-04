#include "I2C.h"
#include "GPIO.h"
#include "systick.h"

void I2C_hardware_init(){

    rcu_periph_clock_enable(RCU_I2C0);
    i2c_deinit(I2C0);
    i2c_clock_config(I2C0,400000,I2C_DTCY_2);
    i2c_enable(I2C0);
    i2c_ack_config(I2C0,I2C_ACK_ENABLE);
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

static uint8_t I2C_hardware_start(uint32_t i2c_periph,uint8_t checkBSY){
    // 等待I2C闲置
    if(checkBSY && I2C_hardware_waitn(i2c_periph,I2C_FLAG_I2CBSY)) return 1;
    // start
    i2c_start_on_bus(i2c_periph);
    // 等待I2C主设备成功发送起始信号
    if(I2C_hardware_wait(i2c_periph,I2C_FLAG_SBSEND)) return 2;
    return 0;
}

static uint8_t I2C_hardware_stop(uint32_t i2c_periph){
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
static uint8_t I2C_hardware_address(uint32_t i2c_periph, uint32_t addr, uint32_t trandirection){
    // 发送设备地址
    i2c_master_addressing(i2c_periph, addr, trandirection);
    // 等待地址发送完成
    if(I2C_hardware_wait(i2c_periph,I2C_FLAG_ADDSEND)) return 3;
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);
    return 0;
}

static uint8_t I2C_hardware_send(uint32_t i2c_periph, uint8_t data){
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

void I2C_hardware_write(uint32_t addr, uint32_t reg,uint8_t* data, uint32_t len){
    I2C_hardware_start(I2C0,1);
    I2C_hardware_address(I2C0,addr<<1,I2C_TRANSMITTER);
    I2C_hardware_send(I2C0,reg);
    for (uint32_t i = 0; i < len; i++)
    {
        I2C_hardware_send(I2C0,data[i]);
    }
    I2C_hardware_stop(I2C0);
}

void I2C_hardware_read(uint32_t addr, uint32_t reg,uint8_t* data, uint32_t len){
    I2C_hardware_start(I2C0,1);
    I2C_hardware_address(I2C0,addr<<1,I2C_TRANSMITTER);
    I2C_hardware_send(I2C0,reg);

    I2C_hardware_start(I2C0,0);
    I2C_hardware_address(I2C0,(addr<<1) | 1,I2C_RECEIVER);

    I2C_hardware_recv(I2C0,data,len);
    
    I2C_hardware_stop(I2C0);
}