#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

// SPDT test pin (replace with actual SPDT pins later)
#define SPDT_GPIO_NODE   DT_NODELABEL(gpio0)
#define SPDT_PIN         2  // P0.02 for testing

// Firmware base addresses  
#define BLE_FIRMWARE_BASE    0x00004000
#define ESB_FIRMWARE_BASE    0x00028000

static const struct device *gpio_dev;

typedef enum {
    MODE_BLE = 0,    // SPDT connected to GND (LOW)
    MODE_ESB = 1     // SPDT floating (HIGH via pull-up)
} boot_mode_t;

static boot_mode_t read_spdt_switch(void)
{
    // Step 1: Discharge any lingering charge
    gpio_pin_configure(gpio_dev, SPDT_PIN, GPIO_OUTPUT_LOW);
    k_busy_wait(100);  // 100μs discharge

    // Step 2: Configure as input with pull-up
    gpio_pin_configure(gpio_dev, SPDT_PIN, GPIO_INPUT | GPIO_PULL_UP);
    k_busy_wait(10);   // 10μs settle

    // Step 3: Read pin state
    int pin_state = gpio_pin_get(gpio_dev, SPDT_PIN);
    
    return (pin_state == 0) ? MODE_BLE : MODE_ESB;
}

static void jump_to_firmware(uint32_t firmware_base)
{
    // Disable interrupts
    __disable_irq();
    
    // Set vector table to firmware location
    SCB->VTOR = firmware_base;
    
    // Get firmware's stack pointer and reset handler
    uint32_t *firmware_vector = (uint32_t*)firmware_base;
    uint32_t firmware_sp = firmware_vector[0];
    uint32_t firmware_reset = firmware_vector[1];
    
    // Set stack pointer and jump
    __set_MSP(firmware_sp);
    ((void(*)())firmware_reset)();
    
    // Never reached
    while(1);
}

int main(void)
{
    // Initialize GPIO device
    gpio_dev = DEVICE_DT_GET(SPDT_GPIO_NODE);
    if (!device_is_ready(gpio_dev)) {
        // Fallback to BLE mode if GPIO fails
        jump_to_firmware(BLE_FIRMWARE_BASE);
    }
    
    // Read SPDT switch state
    boot_mode_t mode = read_spdt_switch();
    
    // Jump to appropriate firmware
    if (mode == MODE_ESB) {
        jump_to_firmware(ESB_FIRMWARE_BASE);
    } else {
        jump_to_firmware(BLE_FIRMWARE_BASE);
    }
    
    // Never reached
    return 0;
}