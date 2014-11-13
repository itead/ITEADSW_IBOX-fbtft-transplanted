本文分析fbmem.c的调用关系

fbmem_init
{
    1. proc_create("fb", 0, NULL, &fb_proc_fops);
    2. register_chrdev(FB_MAJOR,"fb",&fb_fops)
    3. class_create(THIS_MODULE, "graphics");
        注意，此时还没有在/dev/目录下生成fb%d喔，这在register_framebuffer中生成
}

fbmem_exit(void)
{
	remove_proc_entry("fb", NULL);
	class_destroy(fb_class);
	unregister_chrdev(FB_MAJOR, "fb");
}


分析do_register_framebuffer函数

static int do_register_framebuffer(struct fb_info *fb_info)

    1. 在全局fb注册表(registered_fb)中找出可用的位置索引
        num_registered_fb++;
    	for (i = 0 ; i < FB_MAX; i++)
    		if (!registered_fb[i])
    			break;
    	fb_info->node = i;
    
    2. 	创建设备节点fb[i]  /dev/fb1 ....
        fb_info->dev = device_create(fb_class, fb_info->device, MKDEV(FB_MAJOR, i), NULL, "fb%d", i);
    
    3. fb_init_device(fb_info);
    
    4. 初始化一些数据或方法，如果需要的话
    
    5. registered_fb[i] = fb_info;


分析do_unregister_framebuffer函数
static int do_unregister_framebuffer(struct fb_info *fb_info)
    1. 释放一些资源
    2. registered_fb[i] = NULL;
        num_registered_fb--;
    

针对/dev/fb1系统调用open的分析

============================================================
User space:
     fd = open("/dev/fb1",O_RD_WR)   //通过系统调用陷入内核
            ||
            \/
============  ==============================================
Kernel:
    Layer 1: 虚拟文件系统(VFS)
    
    Layer 2: file_operations->open(struct inode *inode, struct file *file) 即：
        fb_open(struct inode *inode, struct file *file)
            // 获取已经使用register_framebuffer注册过的fb_finfo对象
            // fb_info = registered_fb[idx];
            1. struct fb_info *info = get_fb_info(iminor(inode)); 
            2. file->private_data = info;
            3. if (info->fbops->fb_open) // 这时候就体现出fbops的作用了，这就是注册过的与硬件相关的挂钩函数
                info->fbops->fb_open(info,1);
============================================================



针对/dev/fb1系统调用close的分析

============================================================
User space:
     close(fd)   //通过系统调用陷入内核
            ||
            \/
============  ==============================================
Kernel:
    Layer 1: 虚拟文件系统(VFS)
    
    Layer 2: file_operations->release(struct inode *inode, struct file *file) 即：
        fb_release(struct inode *inode, struct file *file)
            1. if (info->fbops->fb_release)
		        info->fbops->fb_release(info,1);
============================================================


针对/dev/fb1系统调用read的分析

============================================================
User space:
     read(fd, buffer, len)   //通过系统调用陷入内核
            ||
            \/
============  ==============================================
Kernel:
    Layer 1: 虚拟文件系统(VFS)
    
    Layer 2: file_operations->read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
        static ssize_t fb_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
          1. if (info->fbops->fb_read)
		        return info->fbops->fb_read(info, buf, count, ppos);
		     else
		        从info->screenbase拷贝数据到用户空间
		  
============================================================

针对/dev/fb1系统调用write的分析

============================================================
User space:
     write(fd, buffer, len)   //通过系统调用陷入内核
            ||
            \/
============  ==============================================
Kernel:
    Layer 1: 虚拟文件系统(VFS)
    
    Layer 2: file_operations->write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
        static ssize_t fb_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
            1. if (info->fbops->fb_write)
		            return info->fbops->fb_write(info, buf, count, ppos);		
		       else
		            从用户空间拷贝数据到info->screenbase
============================================================

针对/dev/fb1系统调用ioctl的分析

============================================================
User space:
     ioctl(fd, CMD, ARGS)   //通过系统调用陷入内核
            ||
            \/
============  ==============================================
Kernel:
    Layer 1: 虚拟文件系统(VFS)
    
    Layer 2: file_operations->ioctl(struct file *file, unsigned int cmd, unsigned long arg)
        static long fb_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
            case FBIOGET_VSCREENINFO:
                var = info->var;
                ret = copy_to_user(argp, &var, sizeof(var)) ? -EFAULT : 0;
            case FBIOPUT_VSCREENINFO:
                info->flags |= FBINFO_MISC_USEREVENT;
		        ret = fb_set_var(info, &var);
		        info->flags &= ~FBINFO_MISC_USEREVENT;
		    case FBIOGET_FSCREENINFO:
		        fix = info->fix;
		        ret = copy_to_user(argp, &fix, sizeof(fix)) ? -EFAULT : 0;
        	default:    /* fall through */
        		fb = info->fbops;
        		if (fb->fb_ioctl)
        			ret = fb->fb_ioctl(info, cmd, arg);
        		else
        			ret = -ENOTTY;
============================================================

针对/dev/fb1系统调用mmap的分析

============================================================
User space:
     mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); //通过系统调用陷入内核
            ||
            \/
============  ==============================================
Kernel:
    Layer 1: 虚拟文件系统(VFS)
    
    Layer 2: file_operations->mmap(struct file *file, struct vm_area_struct * vma)
        static int fb_mmap(struct file *file, struct vm_area_struct * vma)
            1. struct fb_info *info = file_fb_info(file);
                fb = info->fbops;
            2. if (fb->fb_mmap)
            		int res;
            		res = fb->fb_mmap(info, vma);
                    return res;
               else
                    ...
                    return vm_iomap_memory(vma, start, len);
============================================================

