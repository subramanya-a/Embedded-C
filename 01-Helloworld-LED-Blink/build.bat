arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -c -o blink.o blink.c 
arm-none-eabi-ld -T blink.ld -o blink.elf blink.o
arm-none-eabi-objdump --section-headers --syms blink.elf 1>>elf.txt
arm-none-eabi-objcopy -O binary blink.elf blink.bin

pause