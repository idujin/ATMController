#pragma once
#include <vector>
#include <string>
using namespace std;
// Bank API
class IBankAPI {
public:
	virtual ~IBankAPI() = 0;
	virtual bool IsValidCardNumber(string card_number) = 0;
	virtual bool IsValidPIN(string pin_number, string card_number) = 0;
	virtual vector<string> GetAccounts(string card_number) = 0;
	virtual int GetBalance(string account) = 0;
	virtual void UpdateBalance(string account, int balance) = 0;
	virtual int GetCurrentInvalidTry(string card_number) = 0;
	virtual void UpdateInvaidTry(string card_number, int invalid_try_num) = 0;
};
