#pragma once
#include "../ATMController/IBankAPI.h"
#include <vector>
#include <string>
#include <iostream>
#include <map>
using namespace std;
namespace ATMControllerUnitTest {
	class UserInfo {
	public:
		map<string, int> accounts_info_;
		string pin_number_;
		int invalid_try_;
		UserInfo(string pin ="0000", int invalid_try =0, map<string, int> account = {}) : pin_number_(pin), invalid_try_(invalid_try), accounts_info_(account) {}
	};

	class MockBankAPI : public IBankAPI {
	public:
		MockBankAPI(map<string, UserInfo> users);
		virtual bool IsValidCardNumber(string card_number) override;
		virtual bool IsValidPIN(string pin_number, string card_number) override;
		virtual vector<string> GetAccounts(string card_number) override;
		virtual int GetBalance(string account, string card_number) override;
		virtual void UpdateBalance(string account, string card_number, int balance) override;
		virtual int GetCurrentInvalidTry(string card_number) override;
		virtual void UpdateInvaidTry(string card_number, int invalid_try_num) override;
	private:
		map<string, UserInfo> users_;
	};
}
