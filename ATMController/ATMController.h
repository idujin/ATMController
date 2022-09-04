#pragma once
#include <vector>
using namespace std;


constexpr int kTryLimit = 5;
enum class ProcType { kBalance = 0, kDeposit, kWithDraw, kExit };
enum class ATMResults {kInvalidCardNumber = 2, };

// Bank API
class IBankAPI {
public:
	virtual ~IBankAPI() = 0;
	virtual bool IsValidCardNumber(int card_number) =0;
	virtual bool IsValidPIN(int pin_number) = 0;
	virtual vector<int> GetAccounts(int card_number) = 0;
	virtual int GetBalance(int account) = 0;
	virtual void UpdateBalance(int account, int balance) = 0;
	virtual int GetCurrentInvalidTry(int card_number) = 0;
	virtual void UpdateInvaidTry(int card_number, int invalid_try_num) = 0;
};

// User interface
class IUserInterface {
public:
	virtual ~IUserInterface() = 0;
	virtual int GetCardNumber() = 0;
	virtual int GetPIN() = 0;
	virtual int SelectAccount(vector<int> accounts) = 0;
	virtual ProcType SelectProcType() = 0;
	virtual void ShowBalance(int balance) = 0;
	virtual int GetDepositAmount() = 0;
	virtual int GetWithDrawalAmount() = 0;
	virtual bool RequestWithDraw(int get_amount) = 0;
};


class ATMController {
public:
	ATMController();
	void Initialize();
	bool Process(IBankAPI* bank, IUserInterface* user);
	int ReadInsertedCard(IBankAPI* bank, IUserInterface* user);
	bool IdentifyUser(IBankAPI* bank, IUserInterface* user, int card_number);
	int SelectAccount(IBankAPI* bank, IUserInterface* user, int card_number);
	void DoBankJob(IBankAPI* bank, IUserInterface* user, int account);

private:
	bool DoWithDraw(IBankAPI* bank, IUserInterface* user, int account);
	bool is_identified_;
	int account_;

};

