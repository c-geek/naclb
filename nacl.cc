#include <node.h>
#include <nan.h>
#include "functions.h"

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::String;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

NAN_MODULE_INIT(InitAll) {
  Set(target, New<String>("verify").ToLocalChecked(), GetFunction(New<FunctionTemplate>(verify)).ToLocalChecked());
  Set(target, New<String>("sign").ToLocalChecked(), GetFunction(New<FunctionTemplate>(sign)).ToLocalChecked());
}

NODE_MODULE(nacl, InitAll)
