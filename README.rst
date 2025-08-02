BLESB Bootloader
################

Ultra-minimal bootloader for BLESB wireless co-processor that selects firmware based on SPDT switch position.

Overview
********

This bootloader reads an SPDT switch at boot time and jumps to the appropriate firmware:

* **BLE Mode**: Switch connected to GND → Jump to HCI-UART firmware
* **ESB Mode**: Switch floating (pull-up) → Jump to ESB PTX firmware

Features
********

* **Ultra-small**: ~5.6KB flash, 4.2KB RAM
* **Fast boot**: <1ms switching time
* **Hardware-driven**: SPDT switch provides reliable selection
* **No validation**: Direct firmware jumping for speed

Memory Layout
*************

.. code-block::

   Flash Memory (192KB):
   ┌─────────────────────────────────────┐ 0x00000000
   │    BOOTLOADER (~6KB)                │
   ├─────────────────────────────────────┤ 0x00004000  
   │    BLE FIRMWARE (144KB)             │
   ├─────────────────────────────────────┤ 0x00028000
   │    ESB FIRMWARE (32KB)              │
   └─────────────────────────────────────┘ 0x00030000

Building
********

.. code-block:: console

   west build --board BLESB/nrf52805 -- -DBOARD_ROOT=/path/to/BLESB/Board

Hardware
********

* **SPDT Switch Pin**: P0.02 (configurable)
* **Target**: nRF52805 BLESB board
* **Power**: 3.3V operation