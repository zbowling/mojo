// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "base/files/file_path.h"
#include "jni/Bootstrap_jni.h"
#include "shell/android/run_android_application_function.h"

namespace shell {

void Bootstrap(JNIEnv* env,
               jobject,
               jobject j_context,
               jlong j_id,
               jstring j_native_library_path,
               jint j_handle,
               jlong j_run_application_ptr) {
  base::FilePath app_path(
      base::android::ConvertJavaStringToUTF8(env, j_native_library_path));
  RunAndroidApplicationFn run_android_application_fn =
      reinterpret_cast<RunAndroidApplicationFn>(j_run_application_ptr);
  run_android_application_fn(env, j_context, j_id, app_path, j_handle);
}

bool RegisterBootstrapJni(JNIEnv* env) {
  return RegisterNativesImpl(env);
}

}  // namespace shell

JNI_EXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  base::android::InitVM(vm);
  JNIEnv* env = base::android::AttachCurrentThread();

  if (!shell::RegisterBootstrapJni(env))
    return -1;

  return JNI_VERSION_1_4;
}
