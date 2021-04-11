arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -c -o uart-print.o uart-print.c 
arm-none-eabi-ld -T blink.ld -o uart-print.elf uart-print.o
arm-none-eabi-objdump --section-headers --syms uart-print.elf 1>>elf.txt
arm-none-eabi-objcopy -O binary uart-print.elf uart-print.bin

pause