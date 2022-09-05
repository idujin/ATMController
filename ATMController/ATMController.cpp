#include "ATMController.h"

ATMController::ATMController(){
	Initialize();
}

void ATMController::Initialize() {
	is_identified_ = false;
	is_valid_cardnumber_ = false;
}

bool ATMController::Process(IBankAPI* bank, IUserInterface* user) {
	const string card_number = ReadInsertedCard(bank, user);
	if (card_number.empty()) {
		return false; // invalid cardnumber. request the cardnumber again.
	} else {
		is_valid_cardnumber_ = true;
	}

	if (!IdentifyUser(bank, user, card_number)) {
		return false; // blocked account
	}

	string account = SelectAccount(bank, user, card_number);
	if (account.empty()) {
		Initialize();
		return false;
	}

	if (DoBankJob(bank, user, account, card_number) != kNoError) {
		Initialize();
		return false;
	}
	
	Initialize();

	return true;
	

}

string ATMController::ReadInsertedCard(IBankAPI* bank, IUserInterface* user) {
	const string card_number = user->GetCardNumber();
	if (!bank->IsValidCardNumber(card_number)) // invalid card number. request the cardnumber again.
		return "";

	return card_number;
}

bool ATMController::IdentifyUser(IBankAPI* bank, IUserInterface* user, const string card_number) {
	if (!is_valid_cardnumber_) {
		if (!bank->IsValidCardNumber(card_number))
			return false;
		else
			is_valid_cardnumber_ = true;
	}

	int invalid_try_num = bank->GetCurrentInvalidTry(card_number);
	while (invalid_try_num <= kTryLimit) {
		if (bank->IsValidPIN(user->GetPIN(), card_number)) {
			bank->UpdateInvaidTry(card_number, 0);// invalid try number reset
			is_identified_ = true; // identified user
			return true; 
		}
		else {
			invalid_try_num++;
			bank->UpdateInvaidTry(card_number, invalid_try_num);
		}
	}
	return false;
}

string ATMController::SelectAccount(IBankAPI* bank, IUserInterface* user, const string card_number) {
	string account = "";
	if (!is_identified_ || !is_valid_cardnumber_)
		return account;

	vector<string> accounts = bank->GetAccounts(card_number);
	
	if (accounts.size() == 1) {
		account = accounts[0];
	} else if (accounts.size() > 1) {
		account = user->SelectAccount(accounts);
	} else {
		return account; // account is not connected for some reason? 
	}
	return account;
}

int ATMController::DoBankJob(IBankAPI* bank, IUserInterface* user, const string account, const string card_number) {
	if (!is_identified_ || !is_valid_cardnumber_)
		return kUnidentified;

	int result = kNoError;

	ProcType curr_job;
	curr_job = user->SelectProcType();
	switch (curr_job) {
	case ProcType::kBalance: {
			int balance = bank->GetBalance(account, card_number);
			user->ShowBalance(balance);
			break;
		}
	case ProcType::kDeposit: {
		int deposit = user->GetDepositAmount();
		bank->UpdateBalance(account, card_number, bank->GetBalance(account, card_number) + deposit);
		break;
	}
	case ProcType::kWithDraw:
		result = DoWithDraw(bank, user, account, card_number);
		break;
	case ProcType::kExit:
	default:
		break;
	}
	return result;
}


int ATMController::DoWithDraw(IBankAPI* bank, IUserInterface* user, const string account, const string card_number) {
	if (!is_identified_)
		return kUnidentified;

	int balance;
	int get_amount;
	int try_num = 0;
	do {
		balance = bank->GetBalance(account, card_number);
		user->ShowBalance(balance);
		get_amount = user->GetWithDrawalAmount();
		try_num++;
		
	} while (get_amount > balance && try_num < kTryLimit);// invalid request. withdrawal amount should be less than current balance

	if (try_num >= kTryLimit) {
		return kTimeout;
	}

	if (user->WithDraw(get_amount)) {
		bank->UpdateBalance(account, card_number, balance - get_amount);
	} else {
		return kError; // faied for some reason? 
	}

	return kNoError;
}
