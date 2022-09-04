#pragma once
#include <vector>

#include "IBankAPI.h"
#include "IUserInterface.h"

using namespace std;


constexpr int kTryLimit = 5;

enum  { kInvalidCardNumber = -2, kUnidentified =-1, kError = 0, kNoError = 1, };

class ATMController {
public:
	ATMController();
	void Initialize();
	bool Process(IBankAPI* bank, IUserInterface* user);
	string ReadInsertedCard(IBankAPI* bank, IUserInterface* user);
	bool IdentifyUser(IBankAPI* bank, IUserInterface* user, string card_number);
	int SelectAccount(IBankAPI* bank, IUserInterface* user, string card_number);
	int DoBankJob(IBankAPI* bank, IUserInterface* user, int account);

private:
	int DoWithDraw(IBankAPI* bank, IUserInterface* user, int account);
	bool is_identified_;
	bool is_valid_cardnumber_;

};

