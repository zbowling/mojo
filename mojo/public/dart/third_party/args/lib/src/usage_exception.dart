// Copyright (c) 2014, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

library args.usage_exception;

class UsageException implements Exception {
  final String message;
  final String usage;

  UsageException(this.message, this.usage);

  String toString() => "$message\n\n$usage";
}
