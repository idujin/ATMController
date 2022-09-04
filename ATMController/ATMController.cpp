#include "ATMController.h"

void ATMController::Initialize() {
	is_identified_ = false;
	account_ = -1;
}

bool ATMController::Process() {
	const int card_number = GetCardNumber();
	if (!IsValidCardNumber(card_number)) // invalid cardnumber. request the cardnumber again.
		return false;

	int invalid_try_num = GetCurrentInvalidTry(card_number);
	while (invalid_try_num <= kTryLimit) {
		if (IsValidPIN(GetPIN())) {
			is_identified_ = true; // identified user
			break;
		} else {
			invalid_try_num++;
			UpdateInvaidTry(card_number, invalid_try_num);
		}
	}
	
	if (!is_identified_) {
		return false; // blocked account
	}

	vector<int> accounts = GetAccounts(card_number);
	if (accounts.size() == 0){
		return false; // account is not connected for some reason? 
	} else if (accounts.size() == 1) {
		account_ = accounts[0];
	} else {
		account_ = accounts[SelectAccount(accounts)];
	}

	ProcType curr_job;
	do {
		curr_job = SelectProcType();
		switch (curr_job) {
		case ProcType::kBalance:
			int balance = GetBalance(account_);
			ShowBalance(balance);
			break;
		case ProcType::kDeposit:
			int deposit = GetDepositAmount();
			UpdateBalance(account_, GetBalance(account_) + deposit);
			break;
		case ProcType::kWithDraw:
			DoWithDraw(); // error check
			break;
		case ProcType::kExit:
			break;
		default:
			break;
		}
	} while (curr_job != ProcType::kExit);
	
	Initialize();

	return true;
	

}

bool ATMController::DoWithDraw() {
	int balance;
	int get_amount;
	do {
		balance = GetBalance(account_);
		ShowBalance(balance);
		get_amount = GetWithDrawalAmount();
		
	} while (get_amount > balance);// invalid request. withdrawal amount should be less than current balance

	if (RequestWithDraw(get_amount)) {
		UpdateBalance(account_, balance - get_amount);
	}
	else {
		return false; // faied for some reason? 
	}

	return true;
}
