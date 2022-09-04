#pragma once
#include <vector>
using namespace std;


constexpr int kTryLimit = 5;
enum class ProcType { kBalance = 0, kDeposit, kWithDraw, kExit };
enum class ATMResults {kInvalidCardNumber = 2, };

// Bank API
bool IsValidCardNumber(int card_number);
bool IsValidPIN(int pin_number);
vector<int> GetAccounts(int card_number);
int GetBalance(int account);
void UpdateBalance(int account, int balance);
int GetCurrentInvalidTry(int card_number);
void UpdateInvaidTry(int card_number, int invalid_try_num);



// User interface
int GetCardNumber();
int GetPIN();
int SelectAccount(vector<int> accounts);
ProcType SelectProcType();
void ShowBalance(int balance);
int GetDepositAmount();
int GetWithDrawalAmount();
bool RequestWithDraw(int get_amount);


class ATMController {
public:
	void Initialize();
	bool Process();
private:
	bool DoWithDraw();
	bool is_identified_;
	int account_;
	
};

