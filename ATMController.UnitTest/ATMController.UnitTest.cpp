#include "CppUnitTest.h"

#include "MockBankAPI.h"
#include "MockUserInterface.h"

#include "../ATMController/ATMController.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ATMControllerUnitTest {
	TEST_CLASS(ATMControllerUnitTest) {
public:
	ATMControllerUnitTest() {
		// inserted card number, selected account
		string valid_card_number1 = "1234123412341234";
		string valid_card_number2 = "1234567890123456";
		string valid_card_number3 = "4312876509874321";

		string account1 = "123456789012";
		string account2 = "210987654321";
		string account3 = "123412345678";

		input_lists_.push_back(make_pair(valid_card_number1, account1));
		input_lists_.push_back(make_pair(valid_card_number2, account2));
		input_lists_.push_back(make_pair(valid_card_number3, ""));

		// user info in bank. bankAPI uses those info internally.
		// map< account, balance>
		map<string, int> acc_info1;
		acc_info1[account1] = 100;
		map<string, int> acc_info2; // a person can have more than one account.
		acc_info2[account2] = 200;
		acc_info2[account3] = 500;


		// UserInfo (pin,invalid try, <accout, balance>)
		UserInfo user1("1234", 0, acc_info1);
		UserInfo user2("4321", 4, acc_info2);
		UserInfo user3("9999", 0);

		virtual_users_[valid_card_number1] = user1;
		virtual_users_[valid_card_number2] = user2;
		virtual_users_[valid_card_number3] = user3;

		mock_user_inter_ = new MockUserInterface();
	};

	~ATMControllerUnitTest() {
		delete mock_user_inter_;
	}

	/// <summary>
	/// test for process which includes ReadInsertedCard, IdentifyUser, SelectAccount, DoBankJob and initialize
	/// </summary>
	TEST_METHOD(ProcessTestDepositAndWithdraw) {
		ATMController atm_controller;
		auto mock_bank = new MockBankAPI(virtual_users_);

		// set mock user interface
		// the user will select the account with $200 and deposit $300.
		// after then the user will try to withraw $500.

		string valid_card_number = input_lists_[1].first;
		mock_user_inter_->SetCardNumber(valid_card_number);
		string valid_pin = virtual_users_[valid_card_number].pin_number_;
		string valid_account = input_lists_[1].second;
		mock_user_inter_->SetPin(valid_pin);
		mock_user_inter_->SetSelectedAccount(valid_account);
		mock_user_inter_->SetJobIndex(ProcType::kDeposit);
		mock_user_inter_->SetDepositAmount(300);
		mock_user_inter_->SetWithDrawResult(true);

		auto actual_ret = atm_controller.Process(mock_bank, mock_user_inter_);
		Assert::IsTrue(actual_ret);

		auto actual_balance1 = mock_bank->GetBalance(valid_account, valid_card_number);
		const int expected_balance1 = 500;
		Assert::AreEqual(expected_balance1, actual_balance1);

		mock_user_inter_->SetWithDrawAmount(500);
		mock_user_inter_->SetJobIndex(ProcType::kWithDraw);

		auto actual_ret2 = atm_controller.Process(mock_bank, mock_user_inter_);
		Assert::IsTrue(actual_ret2);

		auto actual_balance2 = mock_bank->GetBalance(valid_account, valid_card_number);
		const int expected_balance2 = 0;
		Assert::AreEqual(expected_balance2, actual_balance2);

		delete mock_bank;
	}

	/// <summary>
	/// test for process with a user having a invalid card
	/// </summary>
	TEST_METHOD(ProcessTestInvalidCardNumber) {
		ATMController atm_controller;
		auto mock_bank = new MockBankAPI(virtual_users_);

		// set mock user interface
		string invalid_card_number = "1234123412340000";
		mock_user_inter_->SetCardNumber(invalid_card_number);

		auto actual_ret = atm_controller.Process(mock_bank, mock_user_inter_);
		Assert::IsFalse(actual_ret);

		delete mock_bank;
	}

	/// <summary>
	/// test for process with a user exceeding max try limit
	/// </summary>
	TEST_METHOD(ProcessTestExceedMaxTry) {
		ATMController atm_controller;
		auto mock_bank = new MockBankAPI(virtual_users_);

		// set mock user interface
		string valid_card_number = input_lists_[1].first;
		mock_user_inter_->SetCardNumber(valid_card_number);
		string invalid_pin = "9999";
		string valid_account = input_lists_[1].second;
		mock_user_inter_->SetPin(invalid_pin);

		auto actual_ret = atm_controller.Process(mock_bank, mock_user_inter_);
		Assert::IsFalse(actual_ret);

		string valid_pin = virtual_users_[valid_card_number].pin_number_;
		mock_user_inter_->SetPin(valid_pin);

		auto actual_ret2 = atm_controller.Process(mock_bank, mock_user_inter_);
		Assert::IsFalse(actual_ret2);


		delete mock_bank;
	}

	/// <summary>
	/// test for a user with not connected account
	/// </summary>
	TEST_METHOD(ProcessTestNoAccount) {
		ATMController atm_controller;
		auto mock_bank = new MockBankAPI(virtual_users_);

		// set mock user interface
		string valid_card_number = input_lists_[2].first;
		mock_user_inter_->SetCardNumber(valid_card_number);
		string invalid_pin = virtual_users_[valid_card_number].pin_number_;
		string valid_account = input_lists_[2].second;
		mock_user_inter_->SetPin(invalid_pin);

		auto actual_ret = atm_controller.Process(mock_bank, mock_user_inter_);
		Assert::IsFalse(actual_ret);

		delete mock_bank;
	}

	/// <summary>
	/// test for a user failed to withdraw for broken ATM
	/// </summary>
	TEST_METHOD(ProcessTestWithdrawFailure) {
		ATMController atm_controller;
		auto mock_bank = new MockBankAPI(virtual_users_);

		// set mock user interface
		string valid_card_number = input_lists_[1].first;
		mock_user_inter_->SetCardNumber(valid_card_number);
		string valid_pin = virtual_users_[valid_card_number].pin_number_;
		string valid_account = input_lists_[1].second;
		mock_user_inter_->SetPin(valid_pin);
		mock_user_inter_->SetSelectedAccount(valid_account);
		mock_user_inter_->SetWithDrawAmount(100);
		mock_user_inter_->SetJobIndex(ProcType::kWithDraw);
		mock_user_inter_->SetWithDrawResult(false);

		auto actual_ret = atm_controller.Process(mock_bank, mock_user_inter_);
		Assert::IsFalse(actual_ret);

		delete mock_bank;
	}

	/// <summary>
	/// test for a user having a valid card.
	/// </summary>
	TEST_METHOD(ReadInsertedCardTest) {
		auto mock_bank = new MockBankAPI(virtual_users_);

		ATMController atm_controller;
		string valid_card_number = input_lists_[0].first;
		mock_user_inter_->SetCardNumber(valid_card_number);

		auto read_number = atm_controller.ReadInsertedCard(mock_bank, mock_user_inter_);

		Assert::AreEqual(valid_card_number, read_number);

		delete mock_bank;
	}

	/// <summary>
	/// test for a user having a invalid card.
	/// </summary>
	TEST_METHOD(ReadInsertedCardTestN) { // if the card number is not valid, retuns ""
		ATMController atm_controller;
		auto mock_bank = new MockBankAPI(virtual_users_);

		string invalid_card_number = "0000000000000000";
		mock_user_inter_->SetCardNumber(invalid_card_number);

		auto read_number = atm_controller.ReadInsertedCard(mock_bank, mock_user_inter_);
		Assert::AreNotEqual(invalid_card_number, read_number);

		delete mock_bank;	
	}

	/// <summary>
	/// test for a user with a valid pin.
	/// </summary>
	TEST_METHOD(IdentifyUserTest) {
		ATMController atm_controller;
		auto mock_bank = new MockBankAPI(virtual_users_);

		// set user
		string valid_card_number = input_lists_[0].first;
		mock_user_inter_->SetCardNumber(valid_card_number);
		string valid_pin = virtual_users_[valid_card_number].pin_number_;
		mock_user_inter_->SetPin(valid_pin);

		bool actual = atm_controller.IdentifyUser(mock_bank, mock_user_inter_, valid_card_number);
		Assert::IsTrue(actual);

		delete mock_bank;
	}

	TEST_METHOD(IdentifyUserTestInvalidCard) {
		ATMController atm_controller;
		auto mock_bank = new MockBankAPI(virtual_users_);

		// set user
		string invalid_card_number = "0000000000001234";
		mock_user_inter_->SetCardNumber(invalid_card_number);

		bool actual = atm_controller.IdentifyUser(mock_bank, mock_user_inter_, invalid_card_number);
		Assert::IsFalse(actual);

		delete mock_bank;	
	}

	/// <summary>
	/// test for a user with invalid pins more than 5 times.
	/// </summary>
	TEST_METHOD(IdentifyUserTestInvalidPins) {
		ATMController atm_controller;
		auto mock_bank = new MockBankAPI(virtual_users_);

		string valid_card_number = input_lists_[0].first;
		mock_user_inter_->SetCardNumber(valid_card_number);
		string valid_pin = virtual_users_[valid_card_number].pin_number_;

		vector<string> invalid_pins = { "0001", valid_pin };
		bool actual = false;
		for (auto pin : invalid_pins) {
			mock_user_inter_->SetPin(pin);
			actual = atm_controller.IdentifyUser(mock_bank, mock_user_inter_, valid_card_number);
		}

		Assert::IsFalse(actual);

		delete mock_bank;	
	}

	/// <summary>
	/// test for a person having one account
	/// </summary>
	TEST_METHOD(SelectAccountTestOneAccount) {
		auto mock_bank = new MockBankAPI(virtual_users_);

		// set user
		string valid_card_number = input_lists_[0].first;
		mock_user_inter_->SetCardNumber(valid_card_number);
		string valid_pin = virtual_users_[valid_card_number].pin_number_;
		mock_user_inter_->SetPin(valid_pin);

		string expected_account = input_lists_[0].second;

		// identity user
		ATMController atm_controller;
		atm_controller.IdentifyUser(mock_bank, mock_user_inter_, valid_card_number);

		string actual_account = atm_controller.SelectAccount(mock_bank, mock_user_inter_, valid_card_number);
		Assert::AreEqual(expected_account, actual_account);

		delete mock_bank;	
	}

	/// <summary>
	/// test for a person having two accounts
	/// </summary>
	TEST_METHOD(SelectAccountTestTwoAccounts) {
		auto mock_bank = new MockBankAPI(virtual_users_);

		// set mock user interface
		string valid_card_number = input_lists_[1].first;
		mock_user_inter_->SetCardNumber(valid_card_number);
		string valid_pin = virtual_users_[valid_card_number].pin_number_;
		mock_user_inter_->SetPin(valid_pin);
		mock_user_inter_->SetSelectedAccount(input_lists_[1].second);

		string expected_account = input_lists_[1].second;

		// identity user
		ATMController atm_controller;
		atm_controller.IdentifyUser(mock_bank, mock_user_inter_, valid_card_number);

		string actual_account = atm_controller.SelectAccount(mock_bank, mock_user_inter_, valid_card_number);
		Assert::AreEqual(expected_account, actual_account);

		delete mock_bank;	
	}

	/// <summary>
	/// test for no user identification.
	/// </summary>
	TEST_METHOD(SelectAccountTestNoUserId) {
		// set mock user interface
		auto mock_bank = new MockBankAPI(virtual_users_);

		string valid_card_number = input_lists_[0].first;
		mock_user_inter_->SetCardNumber(valid_card_number);
		string valid_pin = virtual_users_[valid_card_number].pin_number_;
		mock_user_inter_->SetPin(valid_pin);
		mock_user_inter_->SetSelectedAccount(input_lists_[0].second);
		string expected_account = input_lists_[0].second;

		// missing identifying user
		ATMController atm_controller;
		string actual_account = atm_controller.SelectAccount(mock_bank, mock_user_inter_, valid_card_number);

		Assert::AreNotEqual(expected_account, actual_account);

		delete mock_bank;
	}

	/// <summary>
	/// test for no account.
	/// </summary>
	TEST_METHOD(SelectAccountTestNoAccount) {
		// set mock user interface
		auto mock_bank = new MockBankAPI(virtual_users_);

		string valid_card_number = input_lists_[2].first;
		mock_user_inter_->SetCardNumber(valid_card_number);
		string valid_pin = virtual_users_[valid_card_number].pin_number_;
		mock_user_inter_->SetPin(valid_pin);


		ATMController atm_controller;
		atm_controller.IdentifyUser(mock_bank, mock_user_inter_, valid_card_number);
		string actual_account = atm_controller.SelectAccount(mock_bank, mock_user_inter_, valid_card_number);

		string expected_account = "";
		Assert::AreEqual(expected_account, actual_account);

		delete mock_bank;
	}

	/// <summary>
	/// test for checking balance
	/// </summary>
	TEST_METHOD(DoBankJobTestBalance) {
		// set mock user interface
		auto mock_bank = new MockBankAPI(virtual_users_);

		string valid_card_number = input_lists_[0].first;
		mock_user_inter_->SetCardNumber(valid_card_number);
		string valid_pin = virtual_users_[valid_card_number].pin_number_;
		mock_user_inter_->SetPin(valid_pin);
		int select_account_index = 0;
		mock_user_inter_->SetSelectedAccount(input_lists_[0].second);
		mock_user_inter_->SetJobIndex(ProcType::kBalance);

		// identity user
		ATMController atm_controller;
		atm_controller.IdentifyUser(mock_bank, mock_user_inter_, valid_card_number);
		auto ret = atm_controller.DoBankJob(mock_bank, mock_user_inter_, input_lists_[0].second, valid_card_number);
		
		Assert::AreEqual(ret, static_cast<int>(kNoError));

		delete mock_bank;	
	}

	/// <summary>
	/// test for deposit
	/// </summary>
	TEST_METHOD(DoBankJobTestDeposit) {
		// set mock user interface
		auto mock_bank = new MockBankAPI(virtual_users_);

		string valid_card_number = input_lists_[0].first;
		string selected_account = input_lists_[0].second;
		mock_user_inter_->SetCardNumber(valid_card_number);
		string valid_pin = virtual_users_[valid_card_number].pin_number_;
		mock_user_inter_->SetPin(valid_pin);
		int select_account_index = 0;
		mock_user_inter_->SetSelectedAccount(selected_account);
		mock_user_inter_->SetJobIndex(ProcType::kDeposit);
		int deposit_amount = 100;
		mock_user_inter_->SetDepositAmount(deposit_amount);

		// identity user
		ATMController atm_controller;
		atm_controller.IdentifyUser(mock_bank, mock_user_inter_, valid_card_number);
		int before_balance = mock_bank->GetBalance(selected_account, valid_card_number);

		auto ret = atm_controller.DoBankJob(mock_bank, mock_user_inter_, selected_account, valid_card_number);

		int actual_balance = mock_bank->GetBalance(selected_account, valid_card_number);
		int expected_balance = before_balance + deposit_amount;
		Assert::AreEqual(expected_balance, actual_balance);

		delete mock_bank;	
	}

	/// <summary>
	/// test for withdraw
	/// </summary>
	TEST_METHOD(DoBankJobTestWithdraw) {
		// set mock user interface
		auto mock_bank = new MockBankAPI(virtual_users_);

		string valid_card_number = input_lists_[0].first;
		string selected_account = input_lists_[0].second;
		mock_user_inter_->SetCardNumber(valid_card_number);
		string valid_pin = virtual_users_[valid_card_number].pin_number_;
		mock_user_inter_->SetPin(valid_pin);
		mock_user_inter_->SetSelectedAccount(selected_account);
		mock_user_inter_->SetJobIndex(ProcType::kWithDraw);
		int withdraw_amount = 100;
		mock_user_inter_->SetWithDrawAmount(withdraw_amount);
		mock_user_inter_->SetWithDrawResult(true);

		// identity user
		ATMController atm_controller;
		atm_controller.IdentifyUser(mock_bank, mock_user_inter_, valid_card_number);
		int before_balance = mock_bank->GetBalance(selected_account, valid_card_number);

		auto ret = atm_controller.DoBankJob(mock_bank, mock_user_inter_, selected_account, valid_card_number);

		int actual_balance = mock_bank->GetBalance(selected_account, valid_card_number);
		int expected_balance = before_balance - withdraw_amount;
		Assert::AreEqual(expected_balance, actual_balance);

		delete mock_bank;	
	}

	/// <summary>
	/// test for withdraw retry
	/// </summary>
	TEST_METHOD(DoBankJobTestWithdrawRetry) {
		// set mock user interface
		auto mock_bank = new MockBankAPI(virtual_users_);

		string valid_card_number = input_lists_[0].first;
		string selected_account = input_lists_[0].second;
		mock_user_inter_->SetCardNumber(valid_card_number);
		string valid_pin = virtual_users_[valid_card_number].pin_number_;
		mock_user_inter_->SetPin(valid_pin);
		mock_user_inter_->SetSelectedAccount(selected_account);
		mock_user_inter_->SetJobIndex(ProcType::kWithDraw);
		int withdraw_amount = 200;
		mock_user_inter_->SetWithDrawAmount(withdraw_amount);
		mock_user_inter_->SetWithDrawResult(true);
		// identity user
		ATMController atm_controller;
		atm_controller.IdentifyUser(mock_bank, mock_user_inter_, valid_card_number);
		int expected_balance = mock_bank->GetBalance(selected_account, valid_card_number);

		auto ret = atm_controller.DoBankJob(mock_bank, mock_user_inter_, selected_account, valid_card_number);
		Assert::AreEqual(static_cast<int>(kTimeout), ret);

		int actual_balance = mock_bank->GetBalance(selected_account, valid_card_number);
		Assert::AreEqual(expected_balance, actual_balance);

		delete mock_bank;
	}

	/// <summary>
	/// test for trying withdrawal of an unidentified user
	/// </summary>
	TEST_METHOD(DoBankJobTestWithdrawNoUserId) {
		// set mock user interface
		auto mock_bank = new MockBankAPI(virtual_users_);

		string valid_card_number = input_lists_[0].first;
		string selected_account = input_lists_[0].second;
		mock_user_inter_->SetCardNumber(valid_card_number);

		mock_user_inter_->SetJobIndex(ProcType::kWithDraw);

		// identity user
		ATMController atm_controller;

		auto ret = atm_controller.DoBankJob(mock_bank, mock_user_inter_, selected_account, valid_card_number);


		delete mock_bank;
	}

private:
	MockUserInterface* mock_user_inter_;

	// <card_number, user info>
	map<string, UserInfo> virtual_users_;
	vector<pair<string, string>> input_lists_;
	};
}