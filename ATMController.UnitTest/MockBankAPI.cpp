#include "pch.h"

#include "MockBankAPI.h"
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

namespace ATMControllerUnitTest {
	MockBankAPI::MockBankAPI(map<string, UserInfo> users) {
		users_ = users;	
	}

	bool MockBankAPI::IsValidCardNumber(const string card_number) {
		auto it = users_.find(card_number);
		if (it != users_.end()) {
			return true;
		}
		
		return false;
	}

	bool MockBankAPI::IsValidPIN(const string pin_number, const string card_number) {

		return (users_[card_number].pin_number_ == pin_number);
	}

	vector<string> MockBankAPI::GetAccounts(const string card_number) {
		vector<string> accounts;
		for (auto it : users_[card_number].accounts_info_) {
			accounts.push_back(it.first);
		}
		return accounts;
	}

	int MockBankAPI::GetBalance(const string account,const string card_number) {
		return users_[card_number].accounts_info_[account];
	}

	void MockBankAPI::UpdateBalance(const string account, const string card_number, const int balance) {
		users_[card_number].accounts_info_[account] = balance;
	}

	int MockBankAPI::GetCurrentInvalidTry(const string card_number) {		
		return users_[card_number].invalid_try_;
	}

	void MockBankAPI::UpdateInvaidTry(const string card_number, const int invalid_try_num) {
		users_[card_number].invalid_try_ = invalid_try_num;
	}

}