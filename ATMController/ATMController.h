#pragma once
#include <vector>

#include "IBankAPI.h"
#include "IUserInterface.h"

using namespace std;


constexpr int kTryLimit = 5;

enum  { kTimeout = -2, kUnidentified =-1, kError = 0, kNoError = 1, };

class ATMController {
public:
	ATMController();
	void Initialize();
	bool Process(IBankAPI* bank, IUserInterface* user);
	string ReadInsertedCard(IBankAPI* bank, IUserInterface* user);
	bool IdentifyUser(IBankAPI* bank, IUserInterface* user, string card_number);
	string SelectAccount(IBankAPI* bank, IUserInterface* user, string card_number);
	int DoBankJob(IBankAPI* bank, IUserInterface* user, string account, string card_number);

private:
	int DoWithDraw(IBankAPI* bank, IUserInterface* user, string account, string card_number);
	bool is_identified_;
	bool is_valid_cardnumber_;

};

