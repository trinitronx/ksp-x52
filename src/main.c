#include <stdio.h>

#include <libx52.h>
#include <krpc_cnano.h>
#include <krpc_cnano/services/krpc.h>
#include <krpc_cnano/services/space_center.h>
#include <sys/types.h>

// Saitek X52 Multi Function Display Max Width
const u_int X52_MFD_MAXWIDTH = 16;

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

// krpc_init():
//   Initialize kRPC connection
//   returns: kRPC connection handle pointer
krpc_connection_t* krpc_init(void) {
  krpc_connection_t* conn = calloc(1, sizeof(krpc_connection_t));

  krpc_error_t err;
  err = krpc_open(conn, "x52");
  if (err != KRPC_OK) {
    fputs(krpc_get_error(err), stderr);
    return NULL;
  }

  err = krpc_connect(*conn, "Saitek x52 MFD");
  if (err != KRPC_OK) {
    fputs(krpc_get_error(err), stderr);
    return NULL;
  }

  return conn;
}

int main(void) {
  libx52_device *dev;
  dev = init_libx52();

  krpc_connection_t* conn;
  conn = krpc_init();
  if (conn == NULL) {
    libx52_exit(dev);
    fputs("Error encountered during kRPC init. Exiting.", stderr);
    free(conn);
    return NULL;
  }

  // Get active vessel name and display on x52 MFD
  krpc_SpaceCenter_Vessel_t vessel;
  krpc_SpaceCenter_ActiveVessel(*conn, &vessel);
  //krpc_SpaceCenter_Vessel_set_Name(conn, vessel, "My Vessel");
  krpc_error_t err;

  char* vessel_name;
  err = krpc_SpaceCenter_Vessel_Name(*conn, &vessel_name, vessel);
  if (err != KRPC_OK) {
    fputs(krpc_get_error(err), stderr);
    libx52_exit(dev);
    free(conn);
    return NULL;
  }
  // Set MFD line 0 to display vessel_name
  libx52_set_text(dev, 0, vessel_name, strlen(vessel_name));

  // Get a handle to a Flight object for the vessel
  krpc_SpaceCenter_Flight_t flight;
  err = krpc_SpaceCenter_Vessel_Flight(*conn, &flight, vessel, KRPC_NULL);
  if (err != KRPC_OK) {
    fputs(krpc_get_error(err), stderr);
    libx52_exit(dev);
    krpc_close(*conn);
    free(conn);
    return NULL;
  }

  // Get the altiude
  double altitude;
  char* s_altitude;
  err = krpc_SpaceCenter_Flight_MeanAltitude(*conn, &altitude, flight);
  if (err != KRPC_OK) {
    fputs(krpc_get_error(err), stderr);
  }

  printf("Altitude: %.2f\n", altitude);
  // Truncate altitude to MFD max width minus 'A: ' prefix (3 chars)
  snprintf(s_altitude, X52_MFD_MAXWIDTH - 3, "A: %.2f", altitude);

  // Set MFD line 1 to display altitude
  libx52_set_text(dev, 1, s_altitude, strlen(s_altitude));

  //libx52_set_text(dev, 0, "     Saitek     ", 16);
  //libx52_set_text(dev, 1, "   X52 Flight   ", 16);
  //libx52_set_text(dev, 2, " Control System ", 16);

  // Update the display
  libx52_update(dev);

  // Close the library and any associated devices
  libx52_exit(dev);

  krpc_close(*conn);
  if (err != KRPC_OK) {
    fputs(krpc_get_error(err), stderr);
    free(conn);
    return NULL;
  }

  return 0;
}



