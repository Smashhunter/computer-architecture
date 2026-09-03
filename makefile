# Переменные
ASM = nasm
QEMU = qemu-system-x86_64
SRC = test-loader.asm
IMG = test.img

# Цель по умолчанию (запускается при вводе 'make')
all: build run

# Компиляция бинарного файла
build: $(SRC)
	$(ASM) -f bin -o $(IMG) $(SRC)

# Запуск в QEMU
run: $(IMG)
	$(QEMU) -drive format=raw,file=$(IMG)

# Дамп памяти в hex-виде (чтобы посмотреть как легли байты, включая 55 AA в конце)
hex: $(IMG)
	hexdump -C $(IMG)

debug: $(IMG)
	$(QEMU) -drive format=raw,file=$(IMG) -s -S

gdb:
	gdb -ex "target remote localhost:1234" \
	    -ex "architecture i8086" \
	    -ex "set disassembly-flavor intel" \
	    -ex "b *0x7c00" \
	    -ex "c"

# Очистка созданных файлов
clean:
	rm -f $(IMG)

.PHONY: all build run hex clean
