#ifndef CONSTANTS_H
#define CONSTANTS_H

/**
 * @file Constants.h
 * @brief System-wide constants.
 * 
 * This file defines constants used throughout the application
 * for array sizing, memory allocation, and system limits.
 * 
 */

namespace Utils {
  namespace ArrayConstants {
    static constexpr int MAX_WIDGETS = 10; // Maximum widgets in hash table (load factor = 0.7 for optimal performance).
    static constexpr int MAX_GRATICULES = 20; // Hard limit on graticule line segments for oscilloscope grid.
  }
}

#endif