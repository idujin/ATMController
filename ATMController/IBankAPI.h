#pragma once
#include <vector>
#include <string>
using namespace std;
// Bank API
class IBankAPI {
public:
	virtual ~IBankAPI() = 0;
	virtual bool IsValidCardNumber(string card_number) = 0;
	virtual bool IsValidPIN(int pin_number) = 0;
	virtual vector<int> GetAccounts(string card_number) = 0;
	virtual int GetBalance(int account) = 0;
	virtual void UpdateBalance(int account, int balance) = 0;
	virtual int GetCurrentInvalidTry(string card_number) = 0;
	virtual void UpdateInvaidTry(string card_number, int invalid_try_num) = 0;
};
