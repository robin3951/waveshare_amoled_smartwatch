/**
 * @file step_counter.cpp
 * @author @robin3951
 * @brief Implementation of the software step counter pipeline.
 * @date 2026-07-05
 */

#include "step_counter.hpp"

#include <cmath>

#include "esp_log.h"

static const char* TAG = "step_counter";

// ── Constructor
// ───────────────────────────────────────────────────────────────

StepCounter::StepCounter()
    : _prev_mag(1.0f),  // assume device starts upright (1 g on Z)
      _prev_dc(0.0f),
      _prev_lp(0.0f),
      _state(PeakState::IDLE),
      _last_extremum(0.0f),
      _last_event_tick(0),
      _entry_guard(ENTRY_GUARD_COUNT),
      _steps(0),
      _tick(0) {}

// ── Public API
// ────────────────────────────────────────────────────────────────

void StepCounter::update(float ax, float ay, float az) {
  ++_tick;
  float mag      = computeMagnitude(ax, ay, az);
  float dc       = applyDCRemoval(mag);
  float filtered = applyLowPass(dc);
  detectPeak(filtered);

#ifdef PEDOMETER_SIGNAL_CAPTURE
  // CSV output for SerialPlot: mag, dc-removed, low-pass, state (0-2), steps
  printf("%.4f,%.4f,%.4f,%d,%lu\n",
         mag, dc, filtered,
         static_cast<int>(_state),
         static_cast<unsigned long>(_steps));
#endif
}

uint32_t StepCounter::getSteps() const { return _steps; }

void StepCounter::reset() {
  _steps = 0;
  _entry_guard = ENTRY_GUARD_COUNT;
  _state = PeakState::IDLE;
  _last_event_tick = _tick;  // prevent stale timing on restart
}

// ── Pipeline stages
// ───────────────────────────────────────────────────────────

float StepCounter::computeMagnitude(float ax, float ay, float az) const {
  return sqrtf(ax * ax + ay * ay + az * az);
}

float StepCounter::applyDCRemoval(float mag) {
  // IIR high-pass: y[n] = α * (y[n-1] + x[n] - x[n-1])
  float dc = DC_ALPHA * (_prev_dc + mag - _prev_mag);
  _prev_mag = mag;
  _prev_dc = dc;
  return dc;
}

float StepCounter::applyLowPass(float signal) {
  // IIR low-pass: y[n] = β * x[n] + (1-β) * y[n-1]
  float lp = LP_BETA * signal + (1.0f - LP_BETA) * _prev_lp;
  _prev_lp = lp;
  return lp;
}

void StepCounter::detectPeak(float filtered) {
  uint32_t ticks_since = _tick - _last_event_tick;

  switch (_state) {
    case PeakState::IDLE:
      if (filtered > THRESHOLD_HIGH) {
        _last_extremum   = filtered;
        _last_event_tick = _tick;
        _state           = PeakState::AFTER_PEAK;
        ESP_LOGD(TAG, "IDLE → AFTER_PEAK  lp=%.3f", filtered);
      }
      break;

    case PeakState::AFTER_PEAK:
      if (filtered > _last_extremum) {
        _last_extremum = filtered;  // track true peak
      }
      if (filtered < THRESHOLD_LOW && ticks_since >= MIN_HALF_PERIOD_TICKS) {
        float amplitude = _last_extremum - filtered;
        if (ticks_since <= MAX_HALF_PERIOD_TICKS && amplitude >= MIN_AMPLITUDE) {
          // Valid peak→valley half-cycle
          ESP_LOGD(TAG, "AFTER_PEAK → AFTER_VALLEY  amp=%.3f  Δt=%lums",
                   amplitude, static_cast<unsigned long>(ticks_since * 20));
          _last_extremum   = filtered;
          _last_event_tick = _tick;
          _state           = PeakState::AFTER_VALLEY;
        } else {
          ESP_LOGD(TAG, "AFTER_PEAK → IDLE  (amp=%.3f OR timeout %lums)",
                   amplitude, static_cast<unsigned long>(ticks_since * 20));
          _state = PeakState::IDLE;
        }
      } else if (ticks_since > MAX_HALF_PERIOD_TICKS) {
        ESP_LOGD(TAG, "AFTER_PEAK → IDLE  (timeout %lums)",
                 static_cast<unsigned long>(ticks_since * 20));
        _state = PeakState::IDLE;
      }
      break;

    case PeakState::AFTER_VALLEY:
      if (filtered < _last_extremum) {
        _last_extremum = filtered;  // track true valley
      }
      if (filtered > THRESHOLD_HIGH && ticks_since >= MIN_HALF_PERIOD_TICKS) {
        if (ticks_since <= MAX_HALF_PERIOD_TICKS) {
          // Full step cycle confirmed
          if (_entry_guard > 0) {
            --_entry_guard;
            ESP_LOGD(TAG, "AFTER_VALLEY → AFTER_PEAK  entry_guard=%d (discarding)",
                     _entry_guard);
          } else {
            ++_steps;
            ESP_LOGI(TAG, "Step #%lu", static_cast<unsigned long>(_steps));
          }
          _last_extremum   = filtered;
          _last_event_tick = _tick;
          _state           = PeakState::AFTER_PEAK;
        } else {
          ESP_LOGD(TAG, "AFTER_VALLEY → IDLE  (timeout %lums)",
                   static_cast<unsigned long>(ticks_since * 20));
          _state = PeakState::IDLE;
        }
      } else if (ticks_since > MAX_HALF_PERIOD_TICKS) {
        ESP_LOGD(TAG, "AFTER_VALLEY → IDLE  (timeout %lums)",
                 static_cast<unsigned long>(ticks_since * 20));
        _state = PeakState::IDLE;
      }
      break;
  }
}
