#include <nan.h>
#include "shm.h"

void InitAll(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
  SharedMemory::Init(module);
}

NODE_MODULE(addon, InitAll)
