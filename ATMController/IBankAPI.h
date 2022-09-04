#pragma once
#include <vector>
using namespace std;
// Bank API
class IBankAPI {
public:
	virtual ~IBankAPI() = 0;
	virtual bool IsValidCardNumber(int card_number) = 0;
	virtual bool IsValidPIN(int pin_number) = 0;
	virtual vector<int> GetAccounts(int card_number) = 0;
	virtual int GetBalance(int account) = 0;
	virtual void UpdateBalance(int account, int balance) = 0;
	virtual int GetCurrentInvalidTry(int card_number) = 0;
	virtual void UpdateInvaidTry(int card_number, int invalid_try_num) = 0;
};
