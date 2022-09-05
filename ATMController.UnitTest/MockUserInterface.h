#pragma once
#include "../ATMController/IUserInterface.h"
#include <string>
#include <iostream>
#include <vector>
using namespace std;

namespace ATMControllerUnitTest {
	class MockUserInterface : public IUserInterface {
	public:
		virtual string GetCardNumber() override;
		virtual string GetPIN() override;
		virtual string SelectAccount(vector<string> accounts) override;
		virtual ProcType SelectProcType() override;
		virtual void ShowBalance(int balance) override;
		virtual int GetDepositAmount() override;
		virtual int GetWithDrawalAmount() override;
		virtual bool WithDraw(int get_amount) override;

		void SetDepositAmount(int deposit);
		void SetWithDrawAmount(int withdraw);
		void SetPin(string pin);
		void SetCardNumber(string card_number);
		void SetSelectedAccount(string account);
		void SetJobIndex(ProcType index);
		void SetWithDrawResult(bool result);
	private:
		string input_pin_;
		string input_card_number_;
		string selected_account_;
		ProcType select_job_;
		int deposit_amount_;
		int withdrawal_amount_;
		bool withdraw_result_;


	};
}
