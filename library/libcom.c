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

void com_init(com_t *com, int label)
{
  //Init the com
  com_t init_com = {label, {0}, {0}};
  *com = init_com;
  
  //Init the switchbox
  switchbox_init();
  switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL);
  switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA);

  //Init IIC
  iic_init(COM_IIC_INDEX);

  if(com->label != 0)
  {
    if(iic_set_slave_mode(COM_IIC_INDEX, COM_BASE_ADRESS + com->label, com->mmap, COM_MMAPSIZE) != 1)
      pynq_error("com_init::Couldn't set IIC into slave mode!");
  }
}
void com_destroy(com_t *com)
{
  com->label = com->label;
  //Destroy IIC
  iic_destroy(COM_IIC_INDEX);

  //Destroy switchbox -> crashes.
  //switchbox_destroy();
}
void com_run(com_t *com)
{
  //Master
  if (com->label == 0)
  {
    //Read from heartbeat
    iic_read_register(COM_IIC_INDEX, COM_BASE_ADRESS + HEARTBEAT, 0, (uint8_t*)(com->mmap + HEARTBEAT), 1);
    
    //Read from crying
    iic_read_register(COM_IIC_INDEX, COM_BASE_ADRESS + CRYING, 0, (uint8_t*)(com->mmap + CRYING), 1);

    //Send to motors
    iic_write_register(COM_IIC_INDEX, COM_BASE_ADRESS + MOTOR, 0, (uint8_t*)(com->mmap + MOTOR), 1);
    return;
  }

  //Slave
  iic_slave_mode_handler(COM_IIC_INDEX);
  
}
bool com_get(com_t *com, int label, uint32_t *data)
{
  if (label > COM_MAXSLAVES)
  {
    pynq_error("com_get::Label out of bounds! MAX: %d  IS: %d", COM_MAXSLAVES, label);
    return false;
  }
  
  *data = com->mmap[label];
  if(com->recv_buffer[label] != com->mmap[label])
  {
    com->recv_buffer[label] = com->mmap[label];
    return true;
  }
  return false;
}

void com_put(com_t *com, int label, uint32_t data)
{
  if (label > COM_MAXSLAVES)
  {
    pynq_error("com_put::Label out of bounds! MAX: %d  IS: %d", COM_MAXSLAVES, label);
    return;
  }

  com->mmap[label] = data;
}


