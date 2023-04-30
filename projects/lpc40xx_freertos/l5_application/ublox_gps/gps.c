#include "gps.h"

static void priv_enableChip(gps_spi_e gpsCS) {
  (gpsCS.activeLevel == ACTIVE_HIGH) ? gpio__set(gpsCS.chipselect) : gpio__reset(gpsCS.chipselect);
}

static void priv_disableChip(gps_spi_e gpsCS) {
  (gpsCS.activeLevel == ACTIVE_HIGH) ? gpio__reset(gpsCS.chipselect) : gpio__set(gpsCS.chipselect);
}

void GPS_init(gps_spi_e gps_spi_configuration) {
  gpio__construct_as_output(gps_spi_configuration.chipselect.port_number, gps_spi_configuration.chipselect.port_number);
  priv_disableChip(gps_spi_configuration);
  ssp2__set_max_clock(gps_spi_configuration.clockRate);
}