#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint64_t startTime;
  uint64_t endTime;
  bool isExpired;
} software_timer_s;

void SoftwareTimer_init(software_timer_s *timer);
void SoftwareTimer_startTime(software_timer_s *timer, uint64_t timeMS);
bool SoftwareTimer_hasTimeExpired(software_timer_s *timer);
