#include <itead.h>

int main(int argc, char **argv)
{
    uint16_t pin = atoi(argv[1]);
    pinMode(pin, OUTPUT);
    while(1)
    {
        digitalWrite(pin,HIGH);
        delay(10);
        digitalWrite(pin,LOW);
        delay(10);
    }
    return 0;
}
