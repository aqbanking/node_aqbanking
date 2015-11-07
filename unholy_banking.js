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

var addon = require('bindings')('unholy_banking');

var accountNumber = '1234567890';
var bankCode = '9987665';
var userId = "TEST01";
var customerId = "KUNDE01";
var server = "https://hbci-pintan-bw.s-hbci.de/PinTanServlet;";
var userName = "unholybanking";

/*
 * createUser(...)
 *
 * bankId - Bank code
 * userId - Unique user identifier e.g. TEST01
 * customerId - Unique customer identifier e.g. KUNDE01
 * server - Server address (bank backend)
 * userName - Username e.g. unholybanking
 *
 * returns zero if the user was created successfully
 */
if (addon.createUser(bankCode, userId, customerId, server, userName) == 0) {
  console.log("User was created successfully!");
} else {
  console.log("User exists or something went wrong..");
}

/*
 * createAccount(...)
 *
 * bankId - Bank code
 * customerId - Unique customer identifier e.g. KUNDE01
 * accountId - Bank unique account identifier
 *
 * returns zero if the account was added successfully
 */
var rv = addon.createAccount(bankCode, accountNumber, customerId);
if (!rv) {
  console.log("Account was added to "+customerId);
} else {
  console.log("Something went wrong! "+ rv);
}

/*
 * listAccounts()
 *
 * returns all configured accounts on the system:
 *
 * { '0':
 *   { accountNumber: '1234567890',
 *     bankCode: '66050101',
 *     bankName: 'Sparkasse Karlsruhe Ettlingen',
 *     accountName: 'Sichteinlagen' } }
 */
console.log(addon.listAccounts());

/*
 * getTransactions()
 *
 * returns all transactions for the specified account as array:
 *
 * [ { remoteAccountNumber: 'XXXXXXXXXXXXXXXXX',
 *   remoteBankCode: 'XXXXXXXX',
 *   purpose: '2000-66-66T66.66.66 Karte1 2066-66',
 *   localAccountNumber: '1234567890',
 *   localBankCode: '66050101',
 *   localIban: 'DE123123123123123',
 *   localBic: 'XXXXXXXXX',
 *   value: -50,
 *   currency: 'EUR' } ]
 */
console.log(addon.getTransactions(accountNumber, bankCode));
