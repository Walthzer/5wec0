#include <iic.h>
extern void iic_init(const iic_index_t iic){};
extern void iic_destroy(const iic_index_t iic){};
extern bool iic_read_register(const iic_index_t iic, const uint8_t addr,
                              const uint8_t reg, uint8_t *data,
                              uint16_t length){};
extern bool iic_write_register(const iic_index_t iic, const uint8_t addr,
                               const uint8_t reg, uint8_t *data,
                               uint16_t length){};
