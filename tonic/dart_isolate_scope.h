// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TONIC_DART_ISOLATE_SCOPE_H_
#define TONIC_DART_ISOLATE_SCOPE_H_

#include "base/logging.h"
#include "dart/runtime/include/dart_api.h"

namespace tonic {

// DartIsolateScope is a helper class for entering and exiting a given isolate.
class DartIsolateScope {
 public:
  explicit DartIsolateScope(Dart_Isolate isolate);
  ~DartIsolateScope();

 private:
  Dart_Isolate isolate_;
  Dart_Isolate previous_;

  DISALLOW_COPY_AND_ASSIGN(DartIsolateScope);
};

}  // namespace tonic

#endif  // TONIC_DART_ISOLATE_SCOPE_H_
