/*
 * Copyright (C) 2013 Noralf Tronnes
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/********************************************************************
* File		:	fbtft-iteadio.c
* Desc		:	ioctl to manipulate gpio itead a10/a20 core gpio 
*				registers.
* Author	: 	Wu Pengfei
* Company	:	ITEAD Intelligent Systems Co.,Ltd.(http://imall.iteadstudio.com/)
* Date		:	2014/3/11
* History	:	Version		Modified by		Date		What
*				v1.0		Wu Pengfei		2014/3/11	Create
********************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/gpio.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/backlight.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/dma-mapping.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/ioctl.h>


#include "fbtft-iteadio.h"


#define HIGH		0x1
#define LOW			0x0

#define INPUT 		0x0
#define OUTPUT 		0x1

#define GPIO_STATE_BUSY 0x1
#define GPIO_STATE_IDLE 0x0


#define PORT_A	 	0
#define PORT_B		1
#define PORT_C		2
#define PORT_D		3
#define PORT_E		4
#define PORT_F	 	5
#define PORT_G		6
#define PORT_H		7
#define PORT_I		8

#define PORT_NONE	65535
#define INDEX_NONE	65535

#define A10_A20_PIO_BASE				0x01C20800


typedef struct pin_no_port_index_map{
	uint16_t port_no;
	uint16_t index;
} pin_no_port_index_map;


static const pin_no_port_index_map pnp[] = {
/*	port_no	  index				pin_no 		*/    
	{PORT_NONE,	INDEX_NONE}	,	// 0
	{PORT_NONE,	INDEX_NONE}	,	// 1
	{PORT_NONE,	INDEX_NONE}	,	// 2
	{PORT_NONE,	INDEX_NONE}	,	// 3
	{PORT_NONE,	INDEX_NONE}	,	// 4
	{PORT_NONE,	INDEX_NONE}	,	// 5
	{PORT_NONE,	INDEX_NONE}	,	// 6
	{PORT_NONE,	INDEX_NONE}	,	// 7
	{PORT_NONE,	INDEX_NONE}	,	// 8
	{PORT_D,	0}	,	// 9
	{PORT_D,	1}	,	// 10
	{PORT_D,	2}	,	// 11
	{PORT_D,	3}	,	// 12
	{PORT_D,	4}	,	// 13
	{PORT_D,	5}	,	// 14
	{PORT_D,	6}	,	// 15
	{PORT_D,	7}	,	// 16
	{PORT_D,	8}	,	// 17
	{PORT_D,	9}	,	// 18
	{PORT_D,	10}	,	// 19
	{PORT_D,	11}	,	// 20
	{PORT_D,	12}	,	// 21
	{PORT_D,	13}	,	// 22
	{PORT_D,	14}	,	// 23
	{PORT_D,	15}	,	// 24
	{PORT_D,	16}	,	// 25
	{PORT_D,	17}	,	// 1		+ 25
	{PORT_D,	18}	,	// 2
	{PORT_D,	19}	,	// 3
	{PORT_D,	20}	,	// 4
	{PORT_D,	21}	,	// 5
	{PORT_D,	22}	,	// 6
	{PORT_D,	23}	,	// 7
	{PORT_D,	24}	,	// 8
	{PORT_D,	25}	,	// 9
	{PORT_D,	26}	,	// 10
	{PORT_D,	27}	,	// 11
	{PORT_H,	0}	,	// 12
	{PORT_H,	1}	,	// 13
	{PORT_H,	2}	,	// 14
	{PORT_H,	3}	,	// 15
	{PORT_H,	4}	,	// 16
	{PORT_H,	5}	,	// 17
	{PORT_H,	6}	,	// 18
	{PORT_H,	7}	,	// 19
	{PORT_H,	8}	,	// 20
	{PORT_H,	9}	,	// 21
	{PORT_H,	10}	,	// 22
	{PORT_H,	11}	,	// 23
	{PORT_H,	12}	,	// 24
	{PORT_H,	13}	,	// 25
	{PORT_H,	14}	,	// 1		+50
	{PORT_H,	15}	,	// 2
	{PORT_H,	16}	,	// 3
	{PORT_H,	17}	,	// 4
	{PORT_H,	18}	,	// 5
	{PORT_H,	19}	,	// 6
	{PORT_H,	20}	,	// 7
	{PORT_H,	21}	,	// 8
	{PORT_H,	22}	,	// 9
	{PORT_H,	23}	,	// 10
	{PORT_H,	24}	,	// 11
	{PORT_H,	25}	,	// 12
	{PORT_H,	26}	,	// 13
	{PORT_H,	27}	,	// 14
	{PORT_NONE,	INDEX_NONE}	,	// 15
	{PORT_NONE,	INDEX_NONE}	,	// 16
	{PORT_B,	18}	,	// 17
	{PORT_B,	19}	,	// 18
	{PORT_B,	20}	,	// 19
	{PORT_B,	21}	,	// 20
	{PORT_B,	22}	,	// 21
	{PORT_B,	23}	,	// 22
	{PORT_NONE,	INDEX_NONE}	,	// 23
	{PORT_NONE,	INDEX_NONE}	,	// 24
	{PORT_I,	19}	,	// 25
	{PORT_I,	18}	,	// 1		+75
	{PORT_I,	17}	,	// 2
	{PORT_I,	16}	,	// 3
	{PORT_G,	0}	,	// 4
	{PORT_G,	1}	,	// 5
	{PORT_G,	2}	,	// 6
	{PORT_G,	3}	,	// 7
	{PORT_G,	4}	,	// 8
	{PORT_G,	5}	,	// 9
	{PORT_G,	6}	,	// 10
	{PORT_G,	7}	,	// 11
	{PORT_G,	8}	,	// 12
	{PORT_G,	9}	,	// 13
	{PORT_G,	10}	,	// 14
	{PORT_G,	11}	,	// 15
	{PORT_E,	0}	,	// 16
	{PORT_E,	1}	,	// 17
	{PORT_E,	2}	,	// 18
	{PORT_E,	3}	,	// 19
	{PORT_E,	4}	,	// 20
	{PORT_E,	5}	,	// 21
	{PORT_E,	6}	,	// 22
	{PORT_E,	7}	,	// 23
	{PORT_E,	8}	,	// 24
	{PORT_E,	9}	,	// 25
	{PORT_E,	10}	,	// 1		+100
	{PORT_E,	11}	,	// 2
	{PORT_I,	0}	,	// 3
	{PORT_I,	1}	,	// 4
	{PORT_I,	2}	,	// 5
	{PORT_NONE,	INDEX_NONE}	,	// 6
	{PORT_I,	3}	,	// 7
	{PORT_B,	2}	,	// 8
	{PORT_I,	10}	,	// 9
	{PORT_I,	11}	,	// 10
	{PORT_I,	12}	,	// 11
	{PORT_I,	13}	,	// 12
	{PORT_I,	4}	,	// 13
	{PORT_I,	5}	,	// 14
	{PORT_I,	6}	,	// 15
	{PORT_I,	7}	,	// 16
	{PORT_I,	8}	,	// 17
	{PORT_I,	9}	,	// 18
	{PORT_I,	20}	,	// 19
	{PORT_I,	21}	,	// 20
	{PORT_B,	3}	,	// 21
	{PORT_B,	4}	,	// 22
	{PORT_B,	5}	,	// 23
	{PORT_B,	6}	,	// 24
	{PORT_B,	7}	,	// 25
	{PORT_B,	8}	,	// 1		+125
	{PORT_B,	9}	,	// 2
	{PORT_B,	10}	,	// 3
	{PORT_B,	11}	,	// 4
	{PORT_B,	12}	,	// 5
	{PORT_B,	13}	,	// 6
	{PORT_NONE,	INDEX_NONE}	,	// 7
	{PORT_B,	14}	,	// 8
	{PORT_B,	15}	,	// 9
	{PORT_B,	16}	,	// 10
	{PORT_B,	17}	,	// 11
	{PORT_I,	14}	,	// 12
	{PORT_I,	15}	,	// 13
};



/*
 * Global variables in file scope are here.
 */
static volatile uint32_t *cureg = NULL;
static volatile uint32_t *a10_pio_base = NULL;
static          uint16_t port_no;
static          uint16_t index;
static          uint8_t gpio_busy[sizeof(pnp)/sizeof(pnp[0])] = {0};
static DEFINE_MUTEX(gpio_lock);



/*
 * @name	: vertify_pin
 * @desc	: vertify the availability of pin
 * @param	: pin - the number of specific pin.
 * @return	: 1 if available, 0 if unavailable.
 */
static inline uint32_t vertify_pin(uint16_t pin)
{
    if (pin >= (sizeof(pnp)/sizeof(pnp[0]))) {
        printk("array pnp index exceeded pin=%u\n", pin);
        return 0;
    }
        
	if (pnp[pin].port_no != PORT_NONE 
		&& pnp[pin].index != INDEX_NONE) {
		return 1;
	} else {
		return 0;
	}
}

/*
 * @name	: pin_mode
 * @desc	: config pin to the specific mode.
 * @param	: pin - the number of specific pin.
 *			  mode - INPUT or OUTPUT.
 * @return	: none.
 */
static inline void pin_mode(uint16_t pin, uint8_t mode) 
{

	debug("ping_mode(%u,%u) begin\n",pin,mode);
	/* get port_no and index by pin_no */
	port_no = pnp[pin].port_no;
	index   = pnp[pin].index;
	
	/* find the register address need to operate */
	cureg = (volatile uint32_t *)((uint32_t)a10_pio_base 
		+ port_no*0x24 + 4*(index/8));
	
	if( mode == INPUT ) {
		(*cureg) &= ~(0xF<<((index%8)*4));
		(*cureg) |= INPUT << ((index%8)*4);
	} else if(mode == OUTPUT) {
		(*cureg) &= ~(0xF<<((index%8)*4));
		(*cureg) |= OUTPUT << ((index%8)*4);
	} else {
		printk("\nIllegal mode\n");
	}
	
}

/*
 * @name	: digital_write
 * @desc	: write val to the bit of data register corresponding to the pin.
 * @param	: pin - the number of specific pin.
 *			  val - HIGH or LOW.
 * @return	: none.
 */
static inline void digital_write(uint16_t pin, uint8_t val) 
{
	/* get port_no and index by pin_no */
	port_no = pnp[pin].port_no;
	index   = pnp[pin].index;
	
	/* find the register address need to operate */
	cureg = (volatile uint32_t *)((uint32_t)a10_pio_base 
		+ port_no*0x24 + 4*4);
	if( val == HIGH) {
		(*cureg) |= (1 << index);
	} else if (val == LOW) {
		(*cureg) &= ~(1 << index);
	} else {
		printk("\nIllegal val\n");
	}
}

/*
 * @name	: digital_read
 * @desc	: read val from the bit of data register corresponding to the pin.
 * @param	: pin - the number of specific pin.
 * @return	: HIGH or LOW.
 */
static inline uint32_t digital_read(uint16_t pin) {
	/* get port_no and index by pin_no */
	port_no = pnp[pin].port_no;
	index   = pnp[pin].index;
	
	/* find the register address need to operate */
	cureg = (volatile uint32_t *)((uint32_t)a10_pio_base 
		+ port_no*0x24 + 4*4);
	
	return ((*cureg) & (1<<index)) ? HIGH : LOW;
}


/*
 * @param gpio_no - the number of GPIO you want.
 * @param flags   - direction and init value
 * @retval 0      - success
 * @retval Negative     - failed !
 */
int32_t itead_gpio_request_one(uint32_t gpio_no, uint32_t flags)
{
    debug("request GPIO%u as %u\n", gpio_no, flags);
    if (!vertify_pin((uint16_t)gpio_no)) {
        printk("Invalid gpio = %u\n", gpio_no);
        return -EINVAL;
    }
    
    if(!mutex_trylock(&gpio_lock)) {
		return -EBUSY;
	}
	
    if (gpio_busy[gpio_no] == GPIO_STATE_IDLE) {
        gpio_busy[gpio_no] = GPIO_STATE_BUSY;
        mutex_unlock(&gpio_lock);
    } else {
        mutex_unlock(&gpio_lock);
        return -EBUSY;
    }

    
    if (flags == GPIOF_OUT_INIT_LOW) {
        pin_mode(gpio_no, OUTPUT);
        digital_write(gpio_no, LOW);
        return 0;
    } else if (flags == GPIOF_OUT_INIT_HIGH) {
        pin_mode(gpio_no, OUTPUT);
        digital_write(gpio_no, HIGH);
        return 0;
    } else if (flags == GPIOF_IN) {
        pin_mode(gpio_no, INPUT);
        return 0;
    } else {
        return -EINVAL;
    }
    
    return 0;
}
EXPORT_SYMBOL(itead_gpio_request_one);


/*
 * @param gpio_no - the number of GPIO you want.
 */
void itead_gpio_free_one(uint32_t gpio_no)
{
    debug("free GPIO%u\n", gpio_no);
    mutex_trylock(&gpio_lock);
	gpio_busy[gpio_no] = GPIO_STATE_IDLE;
	mutex_unlock(&gpio_lock);
}
EXPORT_SYMBOL(itead_gpio_free_one);


/*
 * @param gpio_no - the number of GPIO you want
 * @param value   - 0 (LOW) or 1 (HIGH) only !
 */
void itead_gpio_set_value(uint32_t gpio_no, uint32_t value)
{
    digital_write(gpio_no, value);
}
EXPORT_SYMBOL(itead_gpio_set_value);

/*
 * @param  gpio_no  - the number of GPIO you want
 * @retval 0        - LOW level
 * @retval 1        - HIGH level
 */
int32_t itead_gpio_get_value(uint32_t gpio_no)
{
    return digital_read(gpio_no);
}
EXPORT_SYMBOL(itead_gpio_get_value);


static int __init fbtft_init(void)
{    
    int ret = 0;
    
	a10_pio_base = (volatile uint32_t *)ioremap(A10_A20_PIO_BASE, 360);
	if (!a10_pio_base) {
	    ret = -ENOMEM;
	    return ret;
	}
	debug("fbtft_init done\n");
	return ret;
}

static void __exit fbtft_exit(void)
{
	iounmap(a10_pio_base);
	debug("fbtft_exit done\n");
}

module_init(fbtft_init);
module_exit(fbtft_exit);
MODULE_DESCRIPTION("fbtft core module");
MODULE_AUTHOR("Wu Pengfei");
MODULE_LICENSE("GPL");
