#pragma once
#include <vector>
using namespace std;

enum class ProcType { kBalance = 0, kDeposit, kWithDraw, kExit };

// User interface
class IUserInterface {
public:
	virtual ~IUserInterface() = 0;
	virtual string GetCardNumber() = 0;
	virtual string GetPIN() = 0;
	virtual int SelectAccount(vector<string> accounts) = 0;
	virtual ProcType SelectProcType() = 0;
	virtual void ShowBalance(int balance) = 0;
	virtual int GetDepositAmount() = 0;
	virtual int GetWithDrawalAmount() = 0;
	virtual bool WithDraw(int get_amount) = 0;
};
