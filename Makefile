# 変数宣言
## ソース情報
SRCDIR := src
OUTDIR := bin
TARGET := $(OUTDIR)/out
HDRS := $(wildcard $(SRCDIR)/*.h) $(wildcard $(SRCDIR)/*.hpp)
SRCS := $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(addprefix $(OUTDIR)/,$(patsubst %.c,%.o,$(SRCS)))
DEPS := $(OBJS:%.o=%.d)
## AVR デバイス情報
AVR_CHIP = attiny2313
AVR_WRIT = avrisp2
AVR_PORT = /dev/ttyACM0
## コンパイラ情報
CC = avr-gcc
CFLAGS = -Wall -Os -mmcu=$(AVR_CHIP)
## 他

$(TARGET).hex: $(OBJS)
	echo $(OBJS)
	echo $(SRCS)
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET).elf
	avr-objcopy -F ihex $(TARGET).elf $(TARGET).hex

$(OUTDIR)/%.o: %.c $(HEDS)
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) -g -c $< -o $@

write: $(TARGET).hex
	avrdude -c $(AVR_WRIT) -P $(AVR_PORT) -p $(AVR_CHIP) -U flash:w:$(TARGET).hex:i

# オブジェクトファイルと実行ファイル及び HEX ファイル削除
.PHONY: clean
clean:
	rm $(OUTDIR) -rf
