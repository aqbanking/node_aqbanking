/* NOTE Most of the source code was copied from:
 * aqbanking/src/plugins/backends/aqhbci/tools/aqhbci-tool/addaccount.c
 *
 ***************************************************************************
 $RCSfile$
 -------------------
 cvs         : $Id: getaccounts.c 964 2006-03-17 10:35:21Z cstim $
 begin       : Tue May 03 2005
 copyright   : (C) 2005 by Martin Preuss
 email       : martin@libchipcard.de

 ***************************************************************************
 *          Please see toplevel file COPYING for license details           *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int UB::Helper::add_account(UserAccount * user_account) {
  AB_PROVIDER * pro;
  AB_USER_LIST2 * ul;
  AB_USER * u = 0;

  // TODO hard-coded make optional for user
  const char * userId = "*";
  const char * accountName = "Account";
  const char * ownerName = NULL;

  pro = AB_Banking_GetProvider(this->ab, "aqhbci");
  assert(pro);

  ul = AB_Banking_FindUsers(this->ab, AH_PROVIDER_NAME,
      "de", user_account->bankId, userId, user_account->customerId);

  if (ul) {
    if (AB_User_List2_GetSize(ul) != 1) {
      //DBG_ERROR(0, "Ambiguous customer specification");
      return 3;
    } else {
      AB_USER_LIST2_ITERATOR * cit;

      cit = AB_User_List2_First(ul);
      assert(cit);
      u=AB_User_List2Iterator_Data(cit);
      AB_User_List2Iterator_free(cit);
    }
    AB_User_List2_free(ul);
  }

  if (!u) {
    //DBG_ERROR(0, "No matching user");
    return 3;
  } else {
    AB_ACCOUNT * account;
    AB_BANKINFO_LIST2 * bl;
    AB_BANKINFO_LIST2_ITERATOR * bit;
    AB_BANKINFO * tbi;
    AB_BANKINFO * bi;
    int rv;

    bl = AB_BankInfo_List2_new();
    tbi = AB_BankInfo_new();
    AB_BankInfo_SetBankId( tbi, user_account->bankId );
    rv = AB_Banking_GetBankInfoByTemplate(this->ab, "de", tbi, bl);
    if (rv) {
      //fprintf(stderr, "Error looking for bank info: %d\n", rv);
      return 3;
    }

    bit = AB_BankInfo_List2_First(bl);
    if (bit) {
      bi=AB_BankInfo_List2Iterator_Data(bit);
      assert(bi);
      AB_BankInfo_List2Iterator_free(bit);
    } else {
      bi = NULL;
      //fprintf(stderr, "Could not find bank with id %s\n", user_account->bankId);
    }
    AB_BankInfo_List2_free(bl);

    account = AB_Banking_CreateAccount(this->ab, "aqhbci");
    assert(account);

    if (!ownerName)
      AB_Account_SetOwnerName(account, AB_User_GetUserName(u));
    else
      AB_Account_SetOwnerName(account, ownerName);

    AB_Account_SetAccountNumber(account, user_account->accountId);
    if (accountName)
      AB_Account_SetAccountName(account, accountName);
    AB_Account_SetBankCode(account, user_account->bankId);
    if (bi)
      AB_Account_SetBankName(account, AB_BankInfo_GetBankName(bi));
    AB_Account_SetUser(account, u);
    AB_Account_SetSelectedUser(account, u);

    rv = AB_Banking_AddAccount(this->ab, account);
    if (rv) {
      //DBG_ERROR(0, "Error adding account (%d)", rv);
      return 3;
    }
  }

  return 0;
}
