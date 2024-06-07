/* KSP x52                                                          {#mainpage}
   =======

   KSP-x52 provides a simple integration of Kerbal Space Program and the
   [Saitek x52 H.O.T.A.S.][x52-homepage]

   [x52-homepage]: https://www.logitechg.com/en-us/products/space/x52-space-flight-simulator-controller
*/
#include <stdio.h>
#include <sys/types.h>

#include <libx52.h>
#include <krpc_cnano.h>
#include <krpc_cnano/services/krpc.h>
#include <krpc_cnano/services/space_center.h>


/**
 * @brief Saitek X52 Multi Function Display Max Width (per line)
 *
 */
const u_int X52_MFD_MAXWIDTH = 16;


/**
 * @brief Set the up libx52 object and connect to the Saitek X52 device
 *
 * @return libx52_device*
 */
libx52_device* setup_libx52(void)
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
        return NULL;
    }

    return dev;
}

/**
 * @brief Disconnects and cleans up libx52 device object
 *
 * @param dev
 */
void cleanup_libx52(libx52_device *dev)
{
  libx52_exit(dev); // Disconnect & free(dev) happens in here
}

/**
 * @brief Initialize kRPC connection
 *
 * @return krpc_connection_t* kRPC connection handle pointer
 */
krpc_connection_t* krpc_init(void) {
  krpc_connection_t* conn = calloc(1, sizeof(krpc_connection_t));

  krpc_error_t err;
  err = krpc_open(conn, "/dev/pts/3");
  if (err != KRPC_OK) {
    fputs(krpc_get_error(err), stderr);
    fputs("\n", stderr);
    return NULL;
  }

  err = krpc_connect(*conn, "Saitek x52 MFD");
  if (err != KRPC_OK) {
    fputs(krpc_get_error(err), stderr);
    fputs("\n", stderr);
    return NULL;
  }

  return conn;
}

int main(void) {
  libx52_device *dev;
  dev = setup_libx52();
  if (dev == NULL) {
    libx52_exit(dev);
    fputs("Error encountered during kRPC init. Exiting.", stderr);
    fputs("\n", stderr);
    // TODO: implement retry wait loop
    return EXIT_FAILURE;
  }

  krpc_connection_t* conn;
  conn = krpc_init();
  if (conn == NULL) {
    libx52_exit(dev);
    fputs("Error encountered during kRPC init. Exiting.", stderr);
    fputs("\n", stderr);
    free(conn);
    return EXIT_FAILURE;
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
    fputs("\n", stderr);
    libx52_exit(dev);
    free(conn);
    return EXIT_FAILURE;
  }

  printf("Vessel Name: %s\n", vessel_name);

  // Set MFD line 0 to display vessel_name
  libx52_set_text(dev, 0, vessel_name, strlen(vessel_name));

  // Get a handle to a Flight object for the vessel
  krpc_SpaceCenter_Flight_t flight;
  err = krpc_SpaceCenter_Vessel_Flight(*conn, &flight, vessel, KRPC_NULL);
  if (err != KRPC_OK) {
    fputs(krpc_get_error(err), stderr);
    fputs("\n", stderr);
    libx52_exit(dev);
    krpc_close(*conn);
    free(conn);
    return EXIT_FAILURE;
  }

  // Get the altiude
  double altitude;
  char* s_altitude = NULL;
  err = krpc_SpaceCenter_Flight_MeanAltitude(*conn, &altitude, flight);
  if (err != KRPC_OK) {
    fputs(krpc_get_error(err), stderr);
    fputs("\n", stderr);
  }

  printf("Altitude: %.2f\n", altitude);
  // Truncate altitude to MFD max width minus 'A: ' prefix (3 chars)
  asprintf(&s_altitude, "A: %.2f", altitude);

  // Set MFD line 1 to display altitude
  libx52_set_text(dev, 1, s_altitude, strlen(s_altitude));
  free(s_altitude);

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
    fputs("\n", stderr);
    free(conn);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}



