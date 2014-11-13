s3c24xxfb_probe
    1. fbinfo = framebuffer_alloc(sizeof(struct s3c2410fb_info), &pdev->dev);
    2. 资源获取与申请，以及各个初始化
        。。。。
        fbinfo->fbops		    = &s3c2410fb_ops;
        。。。。
    3. ret = register_framebuffer(fbinfo);

s3c2410fb_remove
    1. unregister_framebuffer(fbinfo);
    2. 释放资源等扫尾工作
    
        
     