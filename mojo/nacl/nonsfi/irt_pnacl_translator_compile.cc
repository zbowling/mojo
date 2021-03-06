// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <fcntl.h>

#include "base/files/file_util.h"
#include "base/logging.h"
#include "mojo/nacl/nonsfi/irt_mojo_nonsfi.h"
#include "mojo/public/cpp/bindings/array.h"
#include "mojo/public/cpp/bindings/string.h"
#include "mojo/public/cpp/bindings/strong_binding.h"
#include "mojo/public/cpp/utility/run_loop.h"
#include "native_client/src/untrusted/irt/irt_dev.h"
#include "services/nacl/nonsfi/pnacl_compile.mojom.h"

namespace {

// Implements a mojom interface which allows the content handler to communicate
// with the nexe compiler service.
class PexeCompilerImpl : public mojo::nacl::PexeCompiler {
 public:
  PexeCompilerImpl(mojo::ScopedMessagePipeHandle handle,
                   const struct nacl_irt_pnacl_compile_funcs* funcs)
      : funcs_(funcs), strong_binding_(this, handle.Pass()) {}
  void PexeCompile(const mojo::String& pexe_file_name, const
                   mojo::Callback<void(mojo::Array<mojo::String>)>& callback)
      override {
    // TODO(smklein): This number is arbitrary, but matches the translator
    // in Chrome. Use a better heuristic for number of threads/object files.
    const static uint32_t num_threads = 4;
    size_t obj_file_count = num_threads;
    auto obj_file_names = mojo::Array<mojo::String>::New(obj_file_count);
    int obj_file_fds[obj_file_count];

    for (size_t i = 0; i < obj_file_count; i++) {
      base::FilePath obj_file_name;
      CHECK(CreateTemporaryFile(&obj_file_name))
          << "Could not make temporary object file";
      obj_file_fds[i] = open(obj_file_name.value().c_str(), O_RDWR, O_TRUNC);
      CHECK_GE(obj_file_fds[i], 0)
          << "Could not create temporary file for compiled pexe";
      obj_file_names[i] = mojo::String(obj_file_name.value());
    }

    // Non-SFI mode requries PIC.
    char relocation_model[] = "-relocation-model=pic";
    // Since we are compiling pexes, the bitcode format is 'pnacl'.
    char bitcode_format[] = "-bitcode-format=pnacl";
    // Number of modules should be equal to the number of object files for
    // stability. Use a char buffer more than large enough to hold any expected
    // argument size.
    char split_module[100];
    CHECK_GE(snprintf(split_module, sizeof(split_module), "-split-module=%d",
                      obj_file_count), 0) << "Error formatting split_module";
    char* args[] = { relocation_model, bitcode_format, split_module, nullptr };
    size_t argc = 3;
    funcs_->init_callback(num_threads, obj_file_fds, obj_file_count,
                          args, argc);

    // Read the pexe using fread, and write the pexe into the callback function.
    static const size_t kBufferSize = 0x100000;
    scoped_ptr<char[]> buf(new char[kBufferSize]);
    FILE* pexe_file_stream = fopen(pexe_file_name.get().c_str(), "r");
    // Once the pexe has been opened, it is no longer needed, so we unlink it.
    CHECK(!unlink(pexe_file_name.get().c_str()))
        << "Could not unlink temporary pexe file";
    CHECK(pexe_file_stream) << "Could not open pexe for reading";
    // TODO(smklein): Remove these LOG statements once translation speed
    // is improved.
    LOG(INFO) << "Starting compilation of pexe into nexe";
    for (;;) {
      size_t num_bytes_from_pexe = fread(buf.get(), 1, kBufferSize,
                                         pexe_file_stream);
      CHECK(!ferror(pexe_file_stream)) << "Error reading from pexe file stream";
      if (num_bytes_from_pexe == 0) {
        break;
      }
      funcs_->data_callback(buf.get(), num_bytes_from_pexe);
      LOG(INFO) << "Compiled " << num_bytes_from_pexe << " bytes";
    }
    buf.reset();

    CHECK(!fclose(pexe_file_stream)) << "Failed to close pexe file stream";
    funcs_->end_callback();

    // Return the name of the object file.
    callback.Run(std::move(obj_file_names));
    mojo::RunLoop::current()->Quit();
  }
 private:
  const struct nacl_irt_pnacl_compile_funcs* funcs_;
  mojo::StrongBinding<mojo::nacl::PexeCompiler> strong_binding_;
};

void ServeTranslateRequest(const struct nacl_irt_pnacl_compile_funcs* funcs) {
  // Acquire the handle -- this is our mechanism to contact the
  // content handler which called us.
  MojoHandle handle;
  nacl::MojoGetInitialHandle(&handle);

  // Convert the MojoHandle into a ScopedMessagePipeHandle, and use that to
  // implement the PexeCompiler interface.
  PexeCompilerImpl impl(
      mojo::ScopedMessagePipeHandle(mojo::MessagePipeHandle(handle)).Pass(),
      funcs);
  mojo::RunLoop::current()->Run();
}

}  // namespace anonymous

namespace nacl {

const struct nacl_irt_private_pnacl_translator_compile
    nacl_irt_private_pnacl_translator_compile = {
  ServeTranslateRequest
};

}  // namespace nacl
