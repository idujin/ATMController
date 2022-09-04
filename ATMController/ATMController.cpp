#include "ATMController.h"

ATMController::ATMController(){
	Initialize();
}

void ATMController::Initialize() {
	is_identified_ = false;
	account_ = -1;
}

bool ATMController::Process(IBankAPI* bank, IUserInterface* user) {
	const int card_number = ReadInsertedCard(bank, user);
	if (card_number < 0) return false; // invalid cardnumber. request the cardnumber again.

	is_identified_ = IdentifyUser(bank, user, card_number);
	if (!is_identified_) {
		return false; // blocked account
	}

	account_ = SelectAccount(bank, user, card_number);
	if (account_ < 0) {
		return false;
	}

	DoBankJob(bank, user, account_);
	
	Initialize();

	return true;
	

}

int ATMController::ReadInsertedCard(IBankAPI* bank, IUserInterface* user) {
	const int card_number = user->GetCardNumber();
	if (!bank->IsValidCardNumber(card_number)) // invalid cardnumber. request the cardnumber again.
		return -1;

	return card_number;
}

bool ATMController::IdentifyUser(IBankAPI* bank, IUserInterface* user, const int card_number) {
	int invalid_try_num = bank->GetCurrentInvalidTry(card_number);
	while (invalid_try_num <= kTryLimit) {
		if (bank->IsValidPIN(user->GetPIN())) {
			return true; // identified user
			break;
		}
		else {
			invalid_try_num++;
			bank->UpdateInvaidTry(card_number, invalid_try_num);
		}
	}
	return false;
}

int ATMController::SelectAccount(IBankAPI* bank, IUserInterface* user, const int card_number) {
	vector<int> accounts = bank->GetAccounts(card_number);
	int account = -1;
	if (accounts.size() == 1) {
		account = accounts[0];
	} else if (accounts.size() > 1) {
		account = accounts[user->SelectAccount(accounts)];
	} else {
		return account; // account is not connected for some reason? 
	}
	return account;
}

void ATMController::DoBankJob(IBankAPI* bank, IUserInterface* user, int account) {
	ProcType curr_job;
	do {
		curr_job = user->SelectProcType();
		switch (curr_job) {
		case ProcType::kBalance:
			int balance = bank->GetBalance(account);
			user->ShowBalance(balance);
			break;
		case ProcType::kDeposit:
			int deposit = user->GetDepositAmount();
			bank->UpdateBalance(account, bank->GetBalance(account) + deposit);
			break;
		case ProcType::kWithDraw:
			DoWithDraw(bank, user, account); // error check
			break;
		case ProcType::kExit:
			break;
		default:
			break;
		}
	} while (curr_job != ProcType::kExit);

	return;
}


bool ATMController::DoWithDraw(IBankAPI* bank, IUserInterface* user, const int account) {
	int balance;
	int get_amount;
	do {
		balance = bank->GetBalance(account);
		user->ShowBalance(balance);
		get_amount = user->GetWithDrawalAmount();
		
	} while (get_amount > balance);// invalid request. withdrawal amount should be less than current balance

	if (user->RequestWithDraw(get_amount)) {
		bank->UpdateBalance(account_, balance - get_amount);
	}
	else {
		return false; // faied for some reason? 
	}

	return true;
}
