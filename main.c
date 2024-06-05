#include <stdio.h>

#include <libx52.h>

libx52_device* init_libx52(void)
{
    libx52_device *dev;
    int rc;

    // Initialize the libx52 library
    rc = libx52_init(&dev);
    if (rc != LIBX52_SUCCESS) {
        fputs(libx52_strerror(rc), stderr);
        return NULL;
    }

    // Connect to the supported joystick
    rc = libx52_connect(dev);
    if (rc != LIBX52_SUCCESS) {
        fputs(libx52_strerror(rc), stderr);
        // A failure usually just means that there is no joystick connected
        // Look at the return codes for more information.
    }

    return dev;
}

int main(void) {
  libx52_device *dev;
  dev = init_libx52();

  libx52_set_text(dev, 0, "     Saitek     ", 16);
  libx52_set_text(dev, 1, "   X52 Flight   ", 16);
  libx52_set_text(dev, 2, " Control System ", 16);
  libx52_update(dev);

  // Close the library and any associated devices
  libx52_exit(dev);
  return 0;
}
