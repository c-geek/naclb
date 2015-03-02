#include <node.h>
#include <v8.h>
#include <stdio.h>
#include <stdlib.h>
#include "tweetnacl.h"

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef long long i64;
typedef i64 gf[16];

const int crypto_sign_BYTES = 64;

using namespace v8;

/**
* Verifies a signature using Ed25519 scheme.
*
* arg0 (Uint8Array): clear message to be verified
* arg1 (Uint8Array): signature to check message against
* arg2 (Uint8Array): public key to use for verification
*/
void Verify(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  // Reading clear message
  Local<Object> msg = args[0]->ToObject();
  u64 mlen = msg->GetIndexedPropertiesExternalArrayDataLength();
  u8* m = static_cast<u8*>(msg->GetIndexedPropertiesExternalArrayData());

  // Reading detached signature
  Local<Object> sig = args[1]->ToObject();
  u64 smlen = sig->GetIndexedPropertiesExternalArrayDataLength();
  const u8* sm = static_cast<u8*>(sig->GetIndexedPropertiesExternalArrayData());

  // Reading public key
  Local<Object> pub = args[2]->ToObject();
  const u8* pubk = static_cast<u8*>(pub->GetIndexedPropertiesExternalArrayData());

  // Verifying authenticity
  int res = crypto_sign_open(m,&mlen,sm,smlen,pubk);
  if (res == 0)
    // Good signature
    args.GetReturnValue().Set(Boolean::New(isolate, true));
  else
    // Wrong signature or error
    args.GetReturnValue().Set(Boolean::New(isolate, false));
}

/**
* Signs a message using Ed25519 scheme.
*
* arg0 (Uint8Array): clear message to be signed
* arg1 (Uint8Array): sec key to use for verification
*/
void Sign(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  // Reading clear message
  Local<Object> msg = args[0]->ToObject();
  u64 mlen = msg->GetIndexedPropertiesExternalArrayDataLength();
  const u8* m = static_cast<u8*>(msg->GetIndexedPropertiesExternalArrayData());

  // Reading public key
  Local<Object> sec = args[1]->ToObject();
  const u8* seck = static_cast<u8*>(sec->GetIndexedPropertiesExternalArrayData());
  
  u8* sm;
  u64 smlen = 0;

  sm = (u8*) malloc(mlen + crypto_sign_BYTES);

  // Signing
  crypto_sign(sm,&smlen,m,mlen,seck);

  // Result
  Local<Value> size = Integer::NewFromUnsigned(isolate, smlen);
  Local<Object> array = Array::New(isolate, size->IntegerValue());

  for (int i = 0; i < size->IntegerValue(); i++) {
    array->Set(i, Integer::NewFromUnsigned(isolate, sm[i]));
  }

  args.GetReturnValue().Set(array);
}

void Init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "verify", Verify);
  NODE_SET_METHOD(exports, "sign", Verify);
}

NODE_MODULE(nacl, Init)
