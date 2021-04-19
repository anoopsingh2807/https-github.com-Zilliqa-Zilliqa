/*
 * Copyright (C) 2021 Zilliqa
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <malloc.h>
#include <chrono>

#include "libUtils/CommonUtils.h"
#include "libUtils/Logger.h"

using namespace std;

void CommonUtils ::ReleaseSTLMemoryCache() {
  LOG_MARKER();
  // Remove tim diff calculation and log
  auto startTime = chrono::high_resolution_clock::now();
  malloc_trim(0);
  auto endTime = chrono::high_resolution_clock::now();
  auto ms_int =
      chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
  LOG_GENERAL(INFO, "Time diff malloc_trim()= " << ms_int.count() << " ms");
}