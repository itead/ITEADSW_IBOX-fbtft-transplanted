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
* File      :   fbtft-iteadio.h
* Desc      :   ioctl to manipulate gpio itead a10/a20 core gpio 
*               registers.
* Author    :   Wu Pengfei
* Company   :   ITEAD Intelligent Systems Co.,Ltd.(http://imall.iteadstudio.com/)
* Date      :   2014/3/11
* History   :   Version     Modified by     Date        What
*               v1.0        Wu Pengfei      2014/3/11   Create
********************************************************************/

#ifndef __LINUX_FBTFT_ITEADIO_H
#define __LINUX_FBTFT_ITEADIO_H

#include <linux/types.h>
#include <linux/printk.h>

#if 0
#define __FBTFT_ITEADIO_DEBUG
#endif

#ifdef __FBTFT_ITEADIO_DEBUG
#define debug(fmt, args...)	\
do { \
		printk("\n[ITEADIO:%s,%d,%s]-> ",__FILE__,__LINE__,__FUNCTION__);\
		printk(fmt, ##args); \
} while (0)

#else
#define debug(fmt, args...) \
do {} while(0)

#endif


extern int32_t     itead_gpio_request_one(uint32_t gpio_no, uint32_t flags);
extern void        itead_gpio_free_one(uint32_t gpio_no);


extern void         itead_gpio_set_value(uint32_t gpio_no, uint32_t value);
extern int32_t      itead_gpio_get_value(uint32_t gpio_no);



#endif /* #ifndef __LINUX_FBTFT_ITEADIO_H */

