#include "MockUserInterface.h"
#include <algorithm>

namespace ATMControllerUnitTest {
    string MockUserInterface::GetCardNumber() {
        return input_card_number_;
    }

    string MockUserInterface::GetPIN() {
        return input_pin_;
    }

    string MockUserInterface::SelectAccount(vector<string> accounts) {
        if (find(accounts.begin(), accounts.end(), selected_account_) != accounts.end()) {
            return selected_account_;
        } else {
            return "";
        }       
    }

    ProcType MockUserInterface::SelectProcType() {
        return select_job_;
    }

    void MockUserInterface::ShowBalance(int balance) {
        cout << "Balance: $" << balance << endl;
    }

    int MockUserInterface::GetDepositAmount() {
        return deposit_amount_;
    }

    int MockUserInterface::GetWithDrawalAmount() {
        return withdrawal_amount_;
    }

    bool MockUserInterface::WithDraw(int get_amount) {
        return withdraw_result_;
    }

    void MockUserInterface::SetDepositAmount(const int deposit) {
        deposit_amount_ = deposit;
    }
    
    void MockUserInterface::SetWithDrawAmount(const int withdraw) {
        withdrawal_amount_ = withdraw;
    }
    
    void MockUserInterface::SetPin(const string pin){
        input_pin_ = pin;
    }
    
    void MockUserInterface::SetCardNumber(string card_number) {
        input_card_number_ = card_number;
    }
    
    void MockUserInterface::SetSelectedAccount(string account) {
        selected_account_ = account;
    }
    
    void MockUserInterface::SetJobIndex(ProcType index) {
        select_job_ = index;
    }
    void MockUserInterface::SetWithDrawResult(bool result) {
        withdraw_result_ = result;
    }
}