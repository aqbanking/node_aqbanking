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

/*
 * listAccounts()
 *
 * returns all configured accounts on the system:
 */
console.log(addon.listAccounts());
/* Output:
 *
 * { '0':
 *   { accountNumber: '1234567890',
 *     bankCode: '66050101',
 *     bankName: 'Sparkasse Karlsruhe Ettlingen',
 *     accountName: 'Sichteinlagen' } }
 */

//console.log(addon.getTransactions());
