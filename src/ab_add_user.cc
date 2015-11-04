/* NOTE Most of the source code was copied from:
 * aqbanking/src/plugins/backends/aqhbci/tools/aqhbci-tool/adduser.c
 *
 ***************************************************************************
 $RCSfile$
 -------------------
 cvs         : $Id$
 begin       : Tue May 03 2005
 copyright   : (C) 2005 by Martin Preuss
 email       : martin@libchipcard.de

 ***************************************************************************
 *          Please see toplevel file COPYING for license details           *
 ***************************************************************************/

#include <aqbanking/banking.h>
#include <aqbanking/banking_be.h>

#include <aqhbci/provider.h>
#include <aqhbci/user.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int getBankUrl(AB_BANKING *ab, AH_CRYPT_MODE cm,
    const char *bankId, GWEN_BUFFER *bufServer) {

  AB_BANKINFO *bi;

  bi=AB_Banking_GetBankInfo(ab, "de", 0, bankId);
  if (bi) {
    AB_BANKINFO_SERVICE_LIST *l;
    AB_BANKINFO_SERVICE *sv;

    l=AB_BankInfo_GetServices(bi);
    assert(l);
    sv=AB_BankInfoService_List_First(l);
    while(sv) {
      const char *st;

      st=AB_BankInfoService_GetType(sv);
      if (st && *st && strcasecmp(st, "hbci")==0) {
        const char *svm;

        svm=AB_BankInfoService_GetMode(sv);
        if (svm && *svm) {
          if (!
              ((strcasecmp(svm, "pintan")==0) ^
                       (cm==AH_CryptMode_Pintan))){
                    const char *addr;

                    addr=AB_BankInfoService_GetAddress(sv);
            if (addr && *addr) {
              GWEN_Buffer_Reset(bufServer);
              GWEN_Buffer_AppendString(bufServer, addr);
                      return 0;
            }
          }
        }
      }
      sv=AB_BankInfoService_List_Next(sv);
    }
    AB_BankInfo_free(bi);
  } /* if bank info */

  return -1;
}

int UB::Helper::add_user(UB::User * user) {
  AB_PROVIDER * pro;
  GWEN_BUFFER * nameBuffer = NULL;

  pro = AB_Banking_GetProvider(this->ab, "aqhbci");
  assert(pro);
  assert(user->userName);

  const char * lbankId;
  const char * luserId;
  const char * lcustomerId;
  const char * lserverAddr;
  AH_CRYPT_MODE cm;
  GWEN_URL * url;
  AB_USER * ab_user;

  lbankId = user->bankId;
  luserId = user->userId;
  lcustomerId = user->customerId?user->customerId:luserId;
  lserverAddr = user->server;
  cm = AH_CryptMode_Pintan;

  if (!lbankId || !*lbankId) {
    //DBG_ERROR(0, "No bank id stored and none given");
    GWEN_Buffer_free(nameBuffer);
    return 3;
  }
  if (!luserId || !*luserId) {
    //DBG_ERROR(0, "No user id (Benutzerkennung) stored and none given");
    GWEN_Buffer_free(nameBuffer);
    return 3;
  }

  ab_user = AB_Banking_FindUser(this->ab, AH_PROVIDER_NAME,
      "de", lbankId, luserId, lcustomerId);
  if (ab_user) {
    //DBG_ERROR(0, "User %s already exists", luserId);
    return 3;
  }

  ab_user = AB_Banking_CreateUser(this->ab, AH_PROVIDER_NAME);
  assert(ab_user);

  AB_User_SetUserName(ab_user, user->userName);
  AB_User_SetCountry(ab_user, "de");
  AB_User_SetBankCode(ab_user, lbankId);
  AB_User_SetUserId(ab_user, luserId);
  AB_User_SetCustomerId(ab_user, lcustomerId);
  const char * pt = "pintan";
  AH_User_SetTokenType(ab_user, pt);
  AH_User_SetTokenName(ab_user, NULL);
  AH_User_SetTokenContextId(ab_user, 0); // ??? TODO
  AH_User_SetCryptMode(ab_user, cm);


  //if (rdhType>0)
  //  AH_User_SetRdhType(user, rdhType);
  AH_User_SetRdhType(ab_user, 1); // TODO check correct val
  GWEN_Buffer_free(nameBuffer);

  //if (hbciVersion==0) {
  //  if (cm==AH_CryptMode_Pintan)
      AH_User_SetHbciVersion(ab_user, 220);
  //  else {
  //    if (rdhType>1)
  //      AH_User_SetHbciVersion(user, 300);
  //    else
  //      AH_User_SetHbciVersion(user, 210);
  //  }
  //}
  //else {
  //  AH_User_SetHbciVersion(user, hbciVersion);
  //}

  /* try to get server address from database if still unknown */
  if (!lserverAddr || *lserverAddr==0) {
    GWEN_BUFFER *tbuf;

    tbuf=GWEN_Buffer_new(0, 256, 0, 1);
    if (getBankUrl(this->ab, cm, lbankId, tbuf)) {
      //DBG_INFO(0, "Could not find server address for \"%s\"",
      //lbankId);
    }
    if (GWEN_Buffer_GetUsedBytes(tbuf)==0) {
      //DBG_ERROR(0, "No address given and none available in internal db");
      return 3;
    }
    url=GWEN_Url_fromString(GWEN_Buffer_GetStart(tbuf));
    if (url==NULL) {
      //DBG_ERROR(0, "Bad URL \"%s\" in internal db",
      //GWEN_Buffer_GetStart(tbuf));
      return 3;
    }
    GWEN_Buffer_free(tbuf);
  }
  else {
    /* set address */
    url=GWEN_Url_fromString(lserverAddr);
    if (url==NULL) {
      //DBG_ERROR(0, "Bad URL \"%s\"", lserverAddr);
      return 3;
    }
  }

  //if (cm==AH_CryptMode_Pintan) {
    GWEN_Url_SetProtocol(url, "https");
    if (GWEN_Url_GetPort(url)==0)
      GWEN_Url_SetPort(url, 443);
  //}
  AH_User_SetServerUrl(ab_user, url);
  GWEN_Url_free(url);

  AB_Banking_AddUser(this->ab, ab_user);

  return 0;
}
