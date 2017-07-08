extern "C" {

#include <FreeRTOS.h>
#include <task.h>
#include <esp/uart.h>

}

void setup_coap();
void setup_ap();

extern "C" void user_init(void)
{
    uart_set_baud(0, 115200);

    setup_ap();
    setup_coap();
}