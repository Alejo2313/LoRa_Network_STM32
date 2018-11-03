openocd -f default.cfg -f stm32l0x.cfg | arm-none-eabi-gdb -nx --batch \
  -ex 'target remote localhost:3333' \
  -ex 'monitor reset halt' \
  -ex 'load' \
  -ex 'monitor reset halt' \
  LoRaNODE.elf
