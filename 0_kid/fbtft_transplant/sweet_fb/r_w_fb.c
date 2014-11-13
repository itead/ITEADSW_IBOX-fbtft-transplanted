#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdint.h>

#define SWEET_FB_DEBUG

#ifdef SWEET_FB_DEBUG
#define debug(fmt, args...)		\
    do \
    { \
    	printf("\n[DEBUG:%s,%d,%s]-> ",__FILE__,__LINE__,__FUNCTION__); \
    	printf(fmt, ##args); \
    } while (0)
#else
#define debug(fmt, args...)
#endif

static void print_fb_bitfield(struct fb_bitfield *bf)
{
    printf("\nfb_bitfield\n{");
    printf("\noffset = %u", bf->offset);
    printf("\nlength = %u", bf->length);
    printf("\nmsb_right = %u", bf->msb_right);
    printf("\n}\n");
}

static void print_fb_var_screeninfo(struct fb_var_screeninfo *vinfo)
{
    printf("\nfb_var_screeninfo\n{");
    printf("\nxres = %u", vinfo->xres);
    printf("\nyres = %u", vinfo->yres);   
    printf("\nbits_per_pixel = %u", vinfo->bits_per_pixel);
    printf("\ngrayscale = %u", vinfo->grayscale);
    printf("\nrotate = %u", vinfo->rotate);
    printf("\nred =");
    print_fb_bitfield(&vinfo->red);
    printf("\ngreen =");
    print_fb_bitfield(&vinfo->green);
    printf("\nblue =");
    print_fb_bitfield(&vinfo->blue);
    printf("\ntransp =");
    print_fb_bitfield(&vinfo->transp);
    printf("\n}\n");
}

static void print_fb_fix_screeninfo(struct fb_fix_screeninfo *finfo)
{
    printf("\nfb_fix_screeninfo\n{");
    printf("\nid = %s", finfo->id);
    printf("\nsmem_start = %lu", finfo->smem_start);
    printf("\nsmem_len = %u", finfo->smem_len);
    printf("\nline_length = %u", finfo->line_length);
    printf("\n}\n");
}

void main(int argc, char **argv)
{
    uint16_t i;
    uint16_t j;
    uint16_t color;
    uint16_t inc;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    
    
    if (argc != 3)
    {
        printf("Usage:%s /dev/fb[n] number\n", argv[0]);
        return;
    }

    inc = (uint16_t)atoi(argv[2]);
    
    int fd = open(argv[1],O_RDWR);
    if (fd < 0) 
    {
        debug("\nERROR: open file %s failed !\n", argv[1]);
        exit(1);
    }

    if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) != 0)
    {
        debug("\nERROR: ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) failed !\n");
        exit(1);
    }

    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) != 0)
    {
        debug("\nERROR: ioctl(fd, FBIOGET_FSCREENINFO, &finfo) failed !\n");
        exit(1);
    }

    print_fb_var_screeninfo(&vinfo);
    print_fb_fix_screeninfo(&finfo);

    color = 0;
    for (i=0; i<240; i++)
    {
        color += inc;
        for (j=0; j<320; j++)
        {
            if (sizeof(color) != write(fd, (void *)(&color), sizeof(color)))
            {
                debug("write err (i=%u, j=%u)\n", i, j);
            }
        }
        
    }
    
    close(fd);
    debug("\nmain exit normally\n");
}

