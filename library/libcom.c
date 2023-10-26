 /*
 *  TU/e 5WEC0::PYNQ Communication system
 *
 *  Written by: Walthzer
 * 
 */

#include <libcom.h>
#include <stdlib.h>

//PYNQ LIBS
#include <libpynq.h>

#undef LOG_DOMAIN
#define LOG_DOMAIN "com"

bool data_in_bounds(int label, uint32_t data)
{
  switch (label)
  {
  case HEARTBEAT:
    return (data >= COM_HEARTBEAT_LOW_B && COM_HEARTBEAT_HIGH_B >= data);
  case CRYING:
    return (data >= COM_CRYING_LOW_B && COM_CRYING_HIGH_B >= data);
  case MOTOR:
    return (data >= COM_MOTOR_LOW_B && COM_MOTOR_HIGH_B >= data);
  
  default:
    return false;
  }
}

void com_init(com_t *com, int label)
{
  //Init the com
  com_t init_com = {label, {0}, {0}, {0}};
  *com = init_com;
  
  //Init the IIC pins
  switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL);
  switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA);

  //Init IIC
  iic_init(COM_IIC_INDEX);

  if(com->label != 0)
  {
    if(iic_set_slave_mode(COM_IIC_INDEX, COM_BASE_ADRESS + label, com->slave_buffer, COM_BUFFERSIZE) != 1)
      pynq_error("com_init::Couldn't set IIC into slave mode!");
  }
}
void com_destroy(com_t *com)
{
  com->label = com->label;
  //Destroy IIC
  iic_destroy(COM_IIC_INDEX);
}
void com_run(com_t *com)
{
  //Master
  if (com->label == 0)
  {
    //Read from heartbeat
    iic_read_register(COM_IIC_INDEX, COM_BASE_ADRESS + HEARTBEAT, HEARTBEAT, (com->mmap + HEARTBEAT*4), 4);
    
    //Read from crying
    iic_read_register(COM_IIC_INDEX, COM_BASE_ADRESS + CRYING, CRYING, (com->mmap + CRYING*4), 4);

    //Send to motor
    iic_write_register(COM_IIC_INDEX, COM_BASE_ADRESS + MOTOR, MOTOR, (com->mmap + MOTOR*4), 4);
  } else {
    //Slave
    iic_slave_mode_handler(COM_IIC_INDEX);
  }
}
bool com_get(com_t *com, int label, uint32_t *data)
{
  if (label > COM_MAXSLAVES)
  {
    pynq_error("com_get::Label out of bounds! MAX: %d  IS: %d", COM_MAXSLAVES, label);
    return false;
  }
  
  if(com->label == 0)
  {
    //Use 4 bytes to set the uint32
    uint8_t *v4 = &com->mmap[label*4];
    //NONCONFIDENCE
    *data = v4[0] | (v4[1] << 8) | (v4[2] << 16) | (v4[3] << 24);
  } else 
  {
    *data = com->slave_buffer[label];
  }

  //Check if the data is new.
  if(com->recv_buffer[label] != *data)
  {
    com->recv_buffer[label] = *data;
    return true;
  }
  return false;
}
bool com_put(com_t *com, int label, uint32_t data)
{
  if (label > COM_MAXSLAVES)
  {
    pynq_error("com_put::Label out of bounds! MAX: %d  IS: %d", COM_MAXSLAVES, label);
    return false;
  }
  //Do not update data if its out-of-bounds
  //label dependend bounds
  if(!data_in_bounds(label, data))
    return false;

  //Master
  if(com->label == 0)
  {
    //Sets 4 bytes of the mmap with one uint32 value
    memcpy(&com->mmap[label*4], &data, sizeof(data));
  } else {
    //slave
    com->slave_buffer[label] = data;
  }
  return true;
}

bool com_getm(com_t *com, int label, uint16_t *freq_p, uint16_t *ampl_p)
{
  if (label > COM_MAXSLAVES)
  {
    pynq_error("com_getm::Label out of bounds! MAX: %d  IS: %d", COM_MAXSLAVES, label);
    return false;
  }
  
  uint32_t data;
  if(com->label == 0)
  {
    //Use 4 bytes to set the uint32
    uint8_t *v4 = &com->mmap[label*4];
    data = v4[2] | (v4[3] << 8) | (v4[0] << 16) | (v4[1] << 24);
    
    *freq_p = (data >> 16);
    *ampl_p = data;
  } else 
  {
    data = com->slave_buffer[label];
    *freq_p = data;
    *ampl_p = (data >> 16);
  }

  //Check if the data is new.
  if(com->recv_buffer[label] != data)
  {
    com->recv_buffer[label] = data;
    return true;
  }
  return false;
}

bool com_putm(com_t *com, int label, uint16_t freq, uint16_t ampl)
{
  if (label > COM_MAXSLAVES)
  {
    pynq_error("com_putm::Label out of bounds! MAX: %d  IS: %d", COM_MAXSLAVES, label);
    return false;
  }
  //Do not update data if its out-of-bounds
  //label dependend bounds
  if(!data_in_bounds(label, freq) || !data_in_bounds(label, ampl))
    return false;

  //Master
  if(com->label == 0)
  {
    //Sets 4 bytes of the mmap with one uint32 value
    memcpy(&com->mmap[label*4], &freq, sizeof(freq));
    memcpy(&com->mmap[label*4 + 2], &ampl, sizeof(ampl));
  } else {
    //slave
    com->slave_buffer[label] = (freq << 16) | ampl;
  }
  return true;
}

