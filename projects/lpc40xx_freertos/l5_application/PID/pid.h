#pragma once

typedef struct {
  float kp;          // Proportional gain
  float ki;          // Integral gain
  float kd;          // Derivative gain
  float setpoint;    // Setpoint
  float sample_time; // Sampling time in seconds
  float integral;    // Integral term
  float prev_error;  // Previous error
} pid_controller_t;

void pid_init(pid_controller_t *pid, float kp, float ki, float kd, float setpoint, float sample_time);
float pid_compute(pid_controller_t *pid, float input);
