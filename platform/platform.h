/*
Copyright (c) 2023 Eindhoven University of Technology

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#define axi_gpio_0 (uint32_t)0x41200000
#define axi_iic_0 (uint32_t)0x41600000
#define axi_iic_1 (uint32_t)0x41610000
#define axi_timer_1 (uint32_t)0x42810000
#define xadc_wiz_0 (uint32_t)0x43C10000
#define axi_timer_0 (uint32_t)0x42800000
#define axi_uartlite_1 (uint32_t)0x42C10000
#define axi_quad_spi_0 (uint32_t)0x41E00000
#define axi_uartlite_0 (uint32_t)0x42C00000
#define io_switch_0 (uint32_t)0x42000000
#define axi_quad_spi_1 (uint32_t)0x41E10000
#define axi_intc_0 (uint32_t)0x43C80000

#define axi_pwm_base (uint32_t)0x43C20000
#define axi_version_0 (uint32_t)0x40000000

#endif
