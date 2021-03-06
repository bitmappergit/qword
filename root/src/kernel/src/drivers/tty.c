#include <stdint.h>
#include <stddef.h>
#include <tty.h>
#include <klib.h>
#include <vbe.h>
#include <vga_textmode.h>
#include <vbe_tty.h>
#include <cmdline.h>
#include <kbd.h>
#include <dev.h>
#include <lock.h>

static int use_vbe = 0;

void tty_putchar(char c) {
    tty_write(0, &c, 0, 1);
}

int tty_write(int magic, const void *data, uint64_t loc, size_t count) {
    char *buf = (char *)data;

    if (use_vbe) {
        if (!vbe_available || !vbe_tty_available) {
            text_write(buf, count);
        } else {
            vbe_tty_write(buf, count);
        }
    } else {
        text_write(buf, count);
    }

    return (int)count;
}

int tty_read(int magic, void *data, uint64_t loc, size_t count) {
    int res = (int)kbd_read(data, count);

    return res;
}

/* Stub for now */
static int tty_flush(int dev) {
    return 0;
}

void init_tty(void) {
    char *tty_cmdline;

    if ((tty_cmdline = cmdline_get_value("display"))) {
        if (!kstrcmp(tty_cmdline, "vga")) {
            use_vbe = 0;
        } else if (!kstrcmp(tty_cmdline, "vbe")) {
            use_vbe = 1;
        } else {
            for (;;);
        }
    } else {
        use_vbe = 1;
    }

    dev_t dev = device_add("tty", 0xdead, 0, &tty_read, &tty_write, &tty_flush);
    if (dev == -1) {
        return;
    }

    return;
}
