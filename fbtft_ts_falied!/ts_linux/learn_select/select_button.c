#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>



/*
int select(int nfd_btns, fd_set *readfds, fd_set *writefds,
          fd_set *exceptfds, struct timeval *timeout);

void FD_CLR(int fd, fd_set *set);
int  FD_ISSET(int fd, fd_set *set);
void FD_SET(int fd, fd_set *set);
void FD_ZERO(fd_set *set);
*/

#define BUTTON_DEV      "/sys/class/gpio/gpio2_ph10/value"
#define LED_DEV         "/sys/class/gpio/gpio1_ph20/value"
#define F_LED_ON          '1'
#define F_LED_OFF         '0'
#define LED_ON()          led_control(F_LED_ON)
#define LED_OFF()         led_control(F_LED_OFF)




fd_set fds[1];


void led_control(char flag)
{
    int fd_led; 
    fd_led = open(LED_DEV, O_RDWR);
    if (fd_led < 0) 
    {
        printf("open %s failed !\n", LED_DEV);
        exit(1);
    }
    write(fd_led, &flag, sizeof(flag));
    close(fd_led);
}

void main(int argc, char **argv)
{
    int fd_btn;
    
    int ret;
    unsigned char buffer[10] = {0};

    while(1)    
    {
        fd_btn = open(BUTTON_DEV, O_RDWR);
        if (fd_btn < 0) 
        {
            printf("open %s failed !\n", BUTTON_DEV);
            exit(1);
        }

       
        
        FD_ZERO(fds);
        FD_SET(fd_btn, fds);


        printf("select ... ");
        ret = select(fd_btn + 1, fds, NULL, NULL, NULL);
        printf("select done\n");
    	if (ret < 0) 
    	{
    	    printf("select error!\n");
    	    exit(1);
    	}
    	if (FD_ISSET(fd_btn, fds)) 
    	{
    	    read(fd_btn, buffer, 1);			
    	    printf("value= %c\n", buffer[0]);
    	    

    	    if (buffer[0] == '0') 
    	    {
    	        LED_ON();
    	    }
    	    else if (buffer[0] == '1') 
    	    {
    	        LED_OFF();
    	    }
    	    else 
    	    {
    	        printf("buffer[0]=%u ERROR!!!\n", buffer[0]);
    	        exit(1);
    	    }
    	    buffer[0] = '#';
    	}

        
    	close(fd_btn);
    }
}


