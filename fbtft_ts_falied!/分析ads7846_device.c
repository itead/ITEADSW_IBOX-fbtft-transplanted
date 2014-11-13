分析ads7846_device.c中需要移植的地方只有一个

    /* !!!
	 * 这个地方真的是很为难，gpio映射到中断号，
	 * 很多处理器相关的内核代码都不支持这样的操作
	 * !!!
	 */
	irq = irq ? irq : (gpio_to_irq(gpio_pendown));
	if(irq < 0) {
		pr_err(DRVNAME": Unable to get IRQ assigned to gpio_pendown'\n");
		return -EINVAL;
	}
	
	