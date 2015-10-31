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

#include <iostream>

#include <node.h>
#include <v8.h>

#include <gwenhywfar/cgui.h>
#include <gwenhywfar/gui_be.h>

#include <aqbanking/banking.h>
#include <aqbanking/jobgetbalance.h>
#include <aqbanking/jobgettransactions.h>

#include "ab_helpers.h"
#include "ab_gui_interface.h"

using namespace std;

int UB::Helper::init(void) {
  this->gui = GWEN_Gui_CGui_new();
  this->ab = AB_Banking_new("unholy_banking", 0, AB_BANKING_EXTENSION_NONE);

  GWEN_Gui_SetGui(this->gui);

  int rv = AB_Banking_Init(ab);
  if (rv) {
    cout << "Could not initialize (AB_Banking_Init)." << endl;
    return 3;
  }

  rv = AB_Banking_OnlineInit(ab);
  if (rv) {
    cout << "Could not do online initialize (AB_Banking_OnlineInit)." << endl;
    return 3;
  }

  //GWEN_Gui_SetGetPasswordFn(this->gui, CUSTOM_GETPASSWORD_FN);
  // TODO fix me
  //GWEN_Gui_SetCheckCertFn(this->gui, CUSTOM_CHECKCERT_FN);
  return 0;
}

int UB::Helper::close(void) {
  int rv;
  rv = AB_Banking_OnlineFini(ab);
  if (rv) {
    cout << "Could not do online deinit. (AB_Banking_OnlineFini)." << endl;
    return 3;
  }

  rv = AB_Banking_Fini(ab);
  if (rv) {
    cout << "Could not do deinit. (AB_Banking_Fini)." << endl;
    return 3;
  }

  AB_Banking_free(ab);

  return 0;
}

AB_ACCOUNT * UB::Helper::find_account(const char * account_number, const char * bank_code) {
  return AB_Banking_FindAccount(this->ab,
    "*", "*", bank_code, account_number, "*"
  );
}

v8::Local<v8::Object> UB::Helper::list_accounts(v8::Isolate * isolate) {
  v8::EscapableHandleScope scope(isolate);
  v8::Local<v8::Object> res = v8::Object::New(isolate);

  AB_ACCOUNT_LIST2 *accs;

  accs = AB_Banking_GetAccounts(ab);
  if (accs) {
    AB_ACCOUNT_LIST2_ITERATOR *it;
    it = AB_Account_List2_First(accs);
    if (it) {
      AB_ACCOUNT *a;
      int cnt = 0;
      a = AB_Account_List2Iterator_Data(it);
      while(a) {
        v8::Local<v8::Object> e = v8::Object::New(isolate);

        e->Set(v8::String::NewFromUtf8(isolate, "accountNumber"),
            v8::String::NewFromUtf8(isolate, AB_Account_GetAccountNumber(a)));
        e->Set(v8::String::NewFromUtf8(isolate, "bankCode"),
            v8::String::NewFromUtf8(isolate, AB_Account_GetBankCode(a)));
        e->Set(v8::String::NewFromUtf8(isolate, "bankName"),
            v8::String::NewFromUtf8(isolate, AB_Account_GetBankName(a)));
        e->Set(v8::String::NewFromUtf8(isolate, "accountName"),
            v8::String::NewFromUtf8(isolate, AB_Account_GetAccountName(a)));

        res->Set(cnt, e);

        a = AB_Account_List2Iterator_Next(it);
        cnt++;
      }
      AB_Account_List2Iterator_free(it);
    }
    AB_Account_List2_free(accs);
  }
  return scope.Escape(res);
}

v8::Local<v8::Array> UB::Helper::transactions(AB_ACCOUNT * a, v8::Isolate * isolate) {
  v8::EscapableHandleScope scope(isolate);
  v8::Local<v8::Array> res = v8::Array::New(isolate);

  if (!a) {
    // no account found
    return scope.Escape(res);
  }

  GWEN_TIME * gwTime;
  const char * dateFrom = NULL, * dateTo = NULL;

  AB_JOB * job = 0;
  AB_JOB_LIST2 * jl = 0;
  AB_IMEXPORTER_CONTEXT * ctx = 0;
  AB_IMEXPORTER_ACCOUNTINFO * ai;

  job = AB_JobGetTransactions_new(a);
  if (dateFrom != NULL) {
    gwTime = GWEN_Time_fromString(dateFrom, "YYYYMMDD");
    AB_JobGetTransactions_SetFromTime(job, gwTime);
  }
  if (dateTo != NULL) {
    gwTime = GWEN_Time_fromString(dateTo, "YYYYMMDD");
    AB_JobGetTransactions_SetToTime(job, gwTime);
  }
  if (AB_Job_CheckAvailability(job)) {
    return scope.Escape(res);
  }

  jl = AB_Job_List2_new();
  AB_Job_List2_PushBack(jl, job);
  ctx = AB_ImExporterContext_new();
  if (AB_Banking_ExecuteJobs(ab, jl, ctx)) {
    return scope.Escape(res);
  }

  ai = AB_ImExporterContext_GetFirstAccountInfo (ctx);
  while(ai) {
    int cnt = 0;
    const AB_TRANSACTION * t;
    t = AB_ImExporterAccountInfo_GetFirstTransaction(ai);
    while(t) {
      const AB_VALUE *v;
      //AB_TRANSACTION_STATUS state;
      v = AB_Transaction_GetValue(t);
      if (v) {
        const GWEN_STRINGLIST * sl;
        const GWEN_TIME * tdtime;
        const char * purpose;
        sl = AB_Transaction_GetPurpose(t);
        if (sl) {
          purpose = GWEN_StringList_FirstString(sl);
          if (purpose == NULL) {
            purpose = "";
          }
        } else {
          purpose = "";
        }

        v8::Local<v8::Object> obj = v8::Object::New(isolate);

        tdtime = AB_Transaction_GetDate(t);
        GWEN_Time_Seconds(tdtime);

        if (AB_Transaction_GetRemoteIban(t)) {
          obj->Set(v8::String::NewFromUtf8(isolate, "remoteIban"),
              v8::String::NewFromUtf8(isolate, AB_Transaction_GetRemoteIban(t)));
        }
        if (AB_Transaction_GetRemoteBic(t)) {
          obj->Set(v8::String::NewFromUtf8(isolate, "remoteBic"),
              v8::String::NewFromUtf8(isolate, AB_Transaction_GetRemoteBic(t)));
        }
        if (AB_Transaction_GetRemoteAccountNumber(t)) {
          obj->Set(v8::String::NewFromUtf8(isolate, "remoteAccountNumber"),
              v8::String::NewFromUtf8(isolate, AB_Transaction_GetRemoteAccountNumber(t)));
        }
        if (AB_Transaction_GetRemoteBankCode(t)) {
          obj->Set(v8::String::NewFromUtf8(isolate, "remoteBankCode"),
              v8::String::NewFromUtf8(isolate, AB_Transaction_GetRemoteBankCode(t)));
        }

        obj->Set(v8::String::NewFromUtf8(isolate, "purpose"),
            v8::String::NewFromUtf8(isolate, purpose));

        if (AB_Transaction_GetRemoteBankCode(t)) {
          obj->Set(v8::String::NewFromUtf8(isolate, "remoteBankCode"),
              v8::String::NewFromUtf8(isolate, AB_Transaction_GetRemoteBankCode(t)));
        }

        // TODO do we need checks here or is that obsolete
        obj->Set(v8::String::NewFromUtf8(isolate, "localAccountNumber"),
            v8::String::NewFromUtf8(isolate, AB_Transaction_GetLocalAccountNumber(t)));
        obj->Set(v8::String::NewFromUtf8(isolate, "localBankCode"),
            v8::String::NewFromUtf8(isolate, AB_Transaction_GetLocalBankCode(t)));
        obj->Set(v8::String::NewFromUtf8(isolate, "localIban"),
            v8::String::NewFromUtf8(isolate, AB_Transaction_GetLocalIban(t)));
        obj->Set(v8::String::NewFromUtf8(isolate, "localBic"),
            v8::String::NewFromUtf8(isolate, AB_Transaction_GetLocalBic(t)));
        obj->Set(v8::String::NewFromUtf8(isolate, "value"),
            v8::Number::New(isolate, AB_Value_GetValueAsDouble(v)));
        obj->Set(v8::String::NewFromUtf8(isolate, "currency"),
            v8::String::NewFromUtf8(isolate, AB_Value_GetCurrency(v)));

        /* TODO not working some assertation failure
        const char * transDate = NULL;
        if (GWEN_Time_fromString(transDate, "YYYYMMDD") && transDate) {
          trans->Set(v8::String::NewFromUtf8(isolate, "transactionDate"),
              v8::String::NewFromUtf8(isolate, transDate));
        }*/

        res->Set(cnt, obj);
        cnt++;
      }
      t = AB_ImExporterAccountInfo_GetNextTransaction(ai);
    }
    ai = AB_ImExporterContext_GetNextAccountInfo(ctx);
  }

  AB_Job_free(job);
  AB_Job_List2_free(jl);
  AB_ImExporterContext_free(ctx);

  return res;
}
