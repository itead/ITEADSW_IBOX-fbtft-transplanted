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


uint16_t *screenbase;



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

static inline void fb_draw_pixel(uint x, uint y, uint8_t red, uint8_t green, uint8_t blue)
{
    uint16_t color = 0;
    if( x >= 320 || y >= 240 )
    {
        debug("\nillegal paramiters\n");
        return;
    }
    
    color |= (red & 0x1F) << 11;
    color |= (green & 0x3F) << 5;
    color |= (blue & 0x1F) << 0;
    *(screenbase + (y * 320) + x) = color;
}

void main(int argc, char **argv)
{
    uint16_t i;
    uint16_t j;
    
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    
    if (argc != 2)
    {
        printf("Usage:%s /dev/fb[n]\n", argv[0]);
        return;
    }
    
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

    screenbase = (uint16_t *)mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if ( MAP_FAILED == (void *)screenbase )
    {
        debug("\nmmap failed !\n");
        exit(1);
    }

    
    
    for (i=0; i<240; i++ )
        for (j=0; j<320; j++)
        {
            fb_draw_pixel(j, i, 31, 0, 0);
        }
    sleep(2);
    
    for (i=0; i<240; i++ )
        for (j=0; j<320; j++)
        {
            fb_draw_pixel(j, i, 0, 63, 0);
        }
    sleep(2);

    
    for (i=0; i<240; i++ )
        for (j=0; j<320; j++)
        {
            fb_draw_pixel(j, i, 0, 0, 31);
        }
    sleep(2);
    
    if (munmap((void *)screenbase, finfo.smem_len) != 0)
    {
        debug("\nmunmap failed !\n");
        exit(1);
    }
    
    close(fd);
    debug("\nmain exit normally\n");
}

