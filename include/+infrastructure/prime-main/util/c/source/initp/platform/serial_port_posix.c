#include <initp/system/debug.h>
#include <initp/platform/serial_port.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#ifdef SYSTEM_TRACE_SERIAL_PORT
#include <initp/tools/format.h>
#endif // SYSTEM_TRACE_SERIAL_PORT

int set_interface_attribs(int fd, uint32_t baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity, uint8_t signal) {

    struct termios tty;
    memset (&tty, 0, sizeof tty);

    if (tcgetattr(fd, &tty) != 0) {
        if (signal) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "set_interface_attribs", "Error %d from tcgetattr", errno);
        }
        return -1;
    }

    speed_t speed;
    switch (baudrate) {
        case 1200:      speed = B1200;      break;
        case 2400:      speed = B2400;      break;
        case 4800:      speed = B4800;      break;
        case 9600:      speed = B9600;      break;
        case 19200:     speed = B19200;     break;
        case 38400:     speed = B38400;     break;
        case 57600:     speed = B57600;     break;
        case 115200:    speed = B115200;    break;
        case 230400:    speed = B230400;    break;
        case 460800:    speed = B460800;    break;
        case 576000:    speed = B576000;    break;
        case 921600:    speed = B921600;    break;
        case 1152000:   speed = B1152000;   break;
        default:
            if (signal) {
                sys_debug_print(SYSTEM_LEVEL_WARNING, "set_interface_attribs", "Unknown baud rate, set default");
            }
            speed = B9600;
            break;
    }
    // (baud == 38400 && tty->alt_speed)
    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    size_t PARITY = 0;
    if (parity == 1) {
        PARITY = PARENB | PARODD;
    } else if (parity == 2) {
        PARITY = PARENB;
    }
    // PARENB|PARODD|CMSPAR mark parity
    // PARENB|CMSPAR        space parity

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | ((databits == 7) ? CS7 : CS8);
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~(IGNBRK|BRKINT);// disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
                                    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 0;            // disable read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
    tty.c_iflag &= ~(ISTRIP | INLCR | IGNCR | ICRNL); // disable CR processing

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= PARITY;
    tty.c_cflag &= ~CSTOPB;
    if (stopbits == 2)
        tty.c_cflag |= CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        if (signal) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "set_interface_attribs", "Error %d from tcsetattr", errno);
        }
        return -1;
    }
    return 0;
}

int set_blocking(int fd, int should_block) {

    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(fd, &tty) != 0) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "set_blocking", "Error %d from tggetattr", errno);
        return -1;
    }

    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    // 0.5 seconds read timeout in blocking mode
    tty.c_cc[VTIME] = should_block ? 5 : 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "set_blocking", "Error %d from tcsetattr", errno);
        return -1;
    }
    return 0;
}

int posix_open_serial(const char* path, uint32_t baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity, uint8_t signal) {

    int fd = open(path, O_RDWR | O_NOCTTY | O_SYNC);

    if (fd == -1) {
        if (signal) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "posix_open_serial", "Error %d from open", errno);
        }
        return 0;
    }
    if (set_interface_attribs(fd, baudrate, databits, stopbits, parity, signal) != 0) {
        close(fd);
        return 0;
    }
    return fd;
}

void sys_init_serial_port(sys_serial_port_tp object) {
    memset(object, 0x0, sizeof(sys_serial_port_t));
}

uint8_t sys_configure_serial_port(sys_serial_port_tp object, const char* path, uint32_t baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_configure_serial_port", "Serial port pointer is null");
        return 0;
    }
    if (!path) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_configure_serial_port", "Serial port path is null");
        return 0;
    }

    size_t length = strlen(path);
    if (length < 1) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_configure_serial_port", "Serial port path is empty");
        return 0;
    }
    if (length > MAX_SERIAL_PORT_PATH_LENGTH) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_configure_serial_port", "Serial port path too long");
        return 0;
    }

    memcpy(object->path, path, length + 1);
    object->baudrate = baudrate;
    object->databits = databits;
    object->stopbits = stopbits;
    object->parity = parity;
    object->configured = 1;
    return 1;
}

uint8_t sys_open_serial_port(sys_serial_port_tp object, const char* path, uint32_t baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_open_serial_port", "Serial port pointer is null");
        return 0;
    }
    if (!path) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_open_serial_port", "Serial port path is null");
        return 0;
    }

    if (!sys_configure_serial_port(object, path, baudrate, databits, stopbits, parity)) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_open_serial_port", "Failed to configure serial port");
        return 0;
    }

    sys_close_serial_port(object);

    object->opened = 1;
    object->fd = posix_open_serial(path, baudrate, databits, stopbits, parity, 1);
    if (!object->fd) {
        return 0;
    } else return 1;
}

uint8_t sys_reopen_serial_port(sys_serial_port_tp object) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_reopen_serial_port", "Serial port pointer is null");
        return 0;
    }

    if (object->fd) return 1;
    if (object->opened) return 1;
    if (object->configured) {
        object->opened = 1;
        return 1;
    }
    return 0;
}

void sys_close_serial_port(sys_serial_port_tp object) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_close_serial_port", "Serial port pointer is null");
        return;
    }

    if (object->fd > 0) {
        close(object->fd);
        object->fd = 0;
    }
    object->opened = 0;
}

size_t sys_read_serial_port(sys_serial_port_tp object, char* data, size_t size) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_read_serial_port", "Serial port pointer is null");
        return 0;
    }

    if (!size) return size;

    if (!object->fd) {
        if (object->opened && object->configured) {
            object->fd = posix_open_serial(object->path, object->baudrate, object->databits, object->stopbits, object->parity, 0);
            if (!object->fd) return 0;
        } else return 0;
    }

    int n = read(object->fd, data, size);
    if (n < 0) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_read_serial_port", "Error %d from read", errno);
        if (object->fd) {
            close(object->fd);
            object->fd = 0;
        }
        return 0;
    }

    #ifdef SYSTEM_TRACE_SERIAL_PORT
    if (n > 0) {
        sys_trace_array("sys_read_serial_port", data, n);
    }
    #endif // SYSTEM_TRACE_SERIAL_PORT

    return (size_t)n;
}

size_t sys_write_serial_port(sys_serial_port_tp object, const char* data, size_t length) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_write_serial_port", "Serial port pointer is null");
        return 0;
    }
    if (length < 1) return length;

    if (!object->fd) {
        if (object->opened && object->configured) {
            object->fd = posix_open_serial(object->path, object->baudrate, object->databits, object->stopbits, object->parity, 0);
            if (!object->fd) return 0;
        } else return 0;
    }

    int n = write(object->fd, data, length);
    if (n < 0) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_write_serial_port", "Error %d from write", errno);
        if (object->fd) {
            close(object->fd);
            object->fd = 0;
        }
        return 0;
    }

    #ifdef SYSTEM_TRACE_SERIAL_PORT
    if (n > 0) {
        sys_trace_array("sys_write_serial_port", data, n);
    }
    #endif // SYSTEM_TRACE_SERIAL_PORT

    return (size_t)n;
}
