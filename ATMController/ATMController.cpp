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

	int account = SelectAccount(bank, user, card_number);
	if (account == kUnidentified) {
		Initialize();
		return false;
	}

	if (DoBankJob(bank, user, account) != kNoError) {
		Initialize();
		return false;
	}
	
	Initialize();

	return true;
	

}

string ATMController::ReadInsertedCard(IBankAPI* bank, IUserInterface* user) {
	const string card_number = user->GetCardNumber();
	if (!bank->IsValidCardNumber(card_number)) // invalid cardnumber. request the cardnumber again.
		return "";

	return card_number;
}

bool ATMController::IdentifyUser(IBankAPI* bank, IUserInterface* user, const string card_number) {
	if (!is_valid_cardnumber_) {
		if (!bank->IsValidCardNumber(card_number))
			return false;
	}

	int invalid_try_num = bank->GetCurrentInvalidTry(card_number);
	while (invalid_try_num <= kTryLimit) {
		if (bank->IsValidPIN(user->GetPIN())) { 
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

int ATMController::SelectAccount(IBankAPI* bank, IUserInterface* user, const string card_number) {
	int account = kUnidentified;
	if (!is_identified_ || !is_valid_cardnumber_)
		return account;

	vector<int> accounts = bank->GetAccounts(card_number);
	
	if (accounts.size() == 1) {
		account = accounts[0];
	} else if (accounts.size() > 1) {
		account = accounts[user->SelectAccount(accounts)];
	} else {
		return account; // account is not connected for some reason? 
	}
	return account;
}

int ATMController::DoBankJob(IBankAPI* bank, IUserInterface* user, const int account) {
	if (!is_identified_ || !is_valid_cardnumber_)
		return kUnidentified;

	int result = kNoError;

	ProcType curr_job;
	curr_job = user->SelectProcType();
	switch (curr_job) {
	case ProcType::kBalance: {
			int balance = bank->GetBalance(account);
			user->ShowBalance(balance);
			break;
		}
	case ProcType::kDeposit: {
		int deposit = user->GetDepositAmount();
		bank->UpdateBalance(account, bank->GetBalance(account) + deposit);
		break;
	}
	case ProcType::kWithDraw:
		result = DoWithDraw(bank, user, account);
		break;
	case ProcType::kExit:
	default:
		break;
	}
	return result;
}


int ATMController::DoWithDraw(IBankAPI* bank, IUserInterface* user, const int account) {
	if (!is_identified_)
		return kUnidentified;

	int balance;
	int get_amount;
	do {
		balance = bank->GetBalance(account);
		user->ShowBalance(balance);
		get_amount = user->GetWithDrawalAmount();
		
	} while (get_amount > balance);// invalid request. withdrawal amount should be less than current balance

	if (user->WithDraw(get_amount)) {
		bank->UpdateBalance(account, balance - get_amount);
	}
	else {
		return kError; // faied for some reason? 
	}

	return kNoError;
}
