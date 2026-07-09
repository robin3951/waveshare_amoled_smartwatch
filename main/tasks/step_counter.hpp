/**
 * @file step_counter.hpp
 * @author @robin3951
 * @brief Software step counter implementing a magnitude-based peak detection
 *        pipeline.
 * @date 2026-07-09
 *
 * Pipeline (all stages run at 50 Hz / every 20 ms):
 *   1. Magnitude   sqrt(x²+y²+z²)
 *   2. DC-removal  IIR high-pass (α=0.98, τ≈1 s) removes gravity offset
 *   3. Low-pass    IIR low-pass  (β=0.15, fc≈1.3 Hz) smooths noise
 *   4. Peak/Valley state machine detects full step cycles
 *   5. Entry guard (N=4) discards the first few peaks to avoid false starts
 *
 * @see docs/diagrams/step_counter_class.drawio    — class structure and
 *                                                    relationships
 * @see docs/diagrams/step_counter_pipeline.drawio — signal processing pipeline
 * @see docs/diagrams/step_counter_states.drawio   — peak detection state
 * machine
 * @see docs/diagrams/step_counter_signal.drawio   — annotated signal trace
 */

#pragma once
#include <cstdint>

/**
 * @brief Software step counter — call update() at 50 Hz, read getSteps()
 *        anytime.
 *
 * All internal state (filter history, peak detector, step count) is
 * encapsulated. The class is NOT thread-safe; read the step count from other
 * tasks via the atomic g_step_count exported by pedometer_task.h.
 *
 * @image html step_counter_pipeline.svg "Signal Processing Pipeline"
 * @image html step_counter_states.svg   "Peak Detection State Machine"
 */
class StepCounter {
 public:
  StepCounter();

  /**
   * @brief Process one accelerometer sample through the full pipeline.
   * @param ax Acceleration X in g.
   * @param ay Acceleration Y in g.
   * @param az Acceleration Z in g.
   */
  void update(float ax, float ay, float az);

  /** @brief Returns the cumulative step count since the last reset(). */
  uint32_t getSteps() const;

  /**
   * @brief Resets step count and peak detector state.
   *        Does NOT reset filter history to avoid transient on restart.
   */
  void reset();

 private:
  // ── Pipeline constants ────────────────────────────────────────────────────
  static constexpr float DC_ALPHA = 0.98f;  ///< DC-removal coefficient
  static constexpr float LP_BETA = 0.15f;   ///< Low-pass coefficient
  static constexpr float THRESHOLD_HIGH =
      0.20f;  ///< Filtered signal peak threshold (g)
  static constexpr float THRESHOLD_LOW =
      -0.10f;  ///< Filtered signal valley threshold (g)
  static constexpr float MIN_AMPLITUDE =
      0.15f;  ///< Min peak-to-valley swing (g)
  static constexpr uint32_t MIN_HALF_PERIOD_TICKS = 5;  ///< 5 × 20 ms = 100 ms
  static constexpr uint32_t MAX_HALF_PERIOD_TICKS =
      40;                                      ///< 40 × 20 ms = 800 ms
  static constexpr int ENTRY_GUARD_COUNT = 4;  ///< Ignored peaks at start

  // ── State machine ─────────────────────────────────────────────────────────
  enum class PeakState { IDLE, AFTER_PEAK, AFTER_VALLEY };

  // ── Filter state ──────────────────────────────────────────────────────────
  float _prev_mag;  ///< x[n-1] for DC-removal
  float _prev_dc;   ///< y[n-1] for DC-removal
  float _prev_lp;   ///< y[n-1] for low-pass

  // ── Peak detector state ───────────────────────────────────────────────────
  PeakState _state;
  float _last_extremum;       ///< Magnitude of last tracked peak or valley
  uint32_t _last_event_tick;  ///< _tick value at last peak/valley transition

  // ── Counter ───────────────────────────────────────────────────────────────
  int _entry_guard;  ///< Remaining peaks to discard (entry guard)
  uint32_t _steps;   ///< Confirmed step count
  uint32_t _tick;    ///< Sample counter (× 20 ms = elapsed time)

  // ── Internal pipeline stages ──────────────────────────────────────────────
  float computeMagnitude(float ax, float ay, float az) const;
  float applyDCRemoval(float mag);
  float applyLowPass(float signal);
  void detectPeak(float filtered);
};
