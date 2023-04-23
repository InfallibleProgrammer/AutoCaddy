#include "SoftwareTimer.h"
#include "sys_time.h"

#define MS_TO_US 1000
void SoftwareTimer_init(software_timer_s *timer) {
  timer->startTime = 0;
  timer->endTime = 0;
  timer->isExpired = false;
}
void SoftwareTimer_startTime(software_timer_s *timer, uint64_t timeMS) {
  timer->startTime = sys_time__get_uptime_us();
  timer->endTime = timer->startTime + (timeMS * MS_TO_US);
}
bool SoftwareTimer_hasTimeExpired(software_timer_s *timer) {
  printf("Time %li, Diff%li ", sys_time__get_uptime_us(), timer->endTime);
  if (sys_time__get_uptime_us() >= timer->endTime) {
    printf("FUCK THIS SHIT");
    timer->isExpired = true;
  }
  return timer->isExpired;
}
