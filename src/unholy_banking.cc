/*
 * NodeJS wrapper-plugin for Aqbanking
 * Copyright (C) 2015  Lukas Matt <lukas@zauberstuhl.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <node.h>
#include <v8.h>

#include <iostream>
#include <aqbanking/banking.h>

#include "ab_helpers.h"

using namespace std;
using namespace v8;

void AB_createaccount(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  Local<Value> bankId = args[0], accountId = args[1], customerId = args[2];
  if (!bankId->IsString() || !customerId->IsString() || !accountId->IsString()) {
    cout << "Please provide bankId, accountId and customerId as String-parameter!" << endl;
    return;
  }

  v8::String::Utf8Value bankId8(bankId->ToString(isolate)),
    customerId8(customerId->ToString(isolate)),
    accountId8(accountId->ToString(isolate));

  UB::Helper helper;

  int rv = helper.add_account(
    string(*bankId8).c_str(),
    string(*customerId8).c_str(),
    string(*accountId8).c_str()
  );

  args.GetReturnValue().Set(rv);

  // close class and free stuff
  if (helper.close() != 0) {
    cout << "Something went wrong while closing.." << endl;
  }
}

void AB_createuser(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  Local<Value> bankId = args[0], userId = args[1], customerId = args[2];
  Local<Value> server = args[3], userName = args[4];
  if (!bankId->IsString() || !userId->IsString() ||
      !customerId->IsString() || !server->IsString() ||
      !userName->IsString()) {

    cout << "Please provide bankId, userId, customerId, server and userName as String-parameter!" << endl;
    return;
  }

  v8::String::Utf8Value bankId8(bankId->ToString(isolate)),
    userId8(userId->ToString(isolate)),
    customerId8(customerId->ToString(isolate)),
    server8(server->ToString(isolate)),
    userName8(userName->ToString(isolate));

  UB::Helper helper;

  int rv = helper.add_user(
    string(*bankId8).c_str(),
    string(*userId8).c_str(),
    string(*customerId8).c_str(),
    string(*server8).c_str(),
    string(*userName8).c_str()
  );

  args.GetReturnValue().Set(rv);

  // close class and free stuff
  if (helper.close() != 0) {
    cout << "Something went wrong while closing.." << endl;
  }
}

void AB_listaccounts(const v8::FunctionCallbackInfo<Value>& args) {
  UB::Helper helper;
  Isolate* isolate = args.GetIsolate();
  args.GetReturnValue().Set(helper.list_accounts(isolate));
  // close class and free stuff
  if (helper.close() != 0) {
    cout << "Something went wrong while closing.." << endl;
  }
}

void AB_getransactions(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  // following parameters should be provided:
  // accountNumber, bankCode
  Local<Value> v1 = args[0], v2 = args[1];
  if (!v1->IsString() || !v2->IsString()) {
    cout << "Only strings allowed!" << endl;
    return;
  }
  v8::String::Utf8Value uv1(v1->ToString(isolate));
  v8::String::Utf8Value uv2(v2->ToString(isolate));

  UB::Helper helper;
  AB_ACCOUNT * a = helper.find_account(
      std::string(*uv1).c_str(),
      std::string(*uv2).c_str()
  );
  args.GetReturnValue().Set(helper.transactions(a, isolate));

  // close class and free stuff
  if (helper.close() != 0) {
    cout << "Something went wrong while closing.." << endl;
  }
}

void Init(Handle<Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();
  // register createUser function
  exports->Set(String::NewFromUtf8(isolate, "createUser"),
      FunctionTemplate::New(isolate, AB_createuser)->GetFunction());
  // register createAccount function
  exports->Set(String::NewFromUtf8(isolate, "createAccount"),
      FunctionTemplate::New(isolate, AB_createaccount)->GetFunction());
  // register listAccounts function
  exports->Set(String::NewFromUtf8(isolate, "listAccounts"),
      FunctionTemplate::New(isolate, AB_listaccounts)->GetFunction());
  // register getTransactions function
  exports->Set(String::NewFromUtf8(isolate, "getTransactions"),
      FunctionTemplate::New(isolate, AB_getransactions)->GetFunction());
}

NODE_MODULE(unholy_banking, Init)
