# ATMController
A simple ATM controller
## Description
This repository is for a simple ATM controller class written in C++.

The ATM controller class can do following flow.
You can insert card => PIN number => Select Account => See balance/deposit/withdraw.

There is only 1 dollar in this world!

## Getting Started
### Download
Clone the project, which contains the ATMController and its unit tests.
```sh
git clone https://github.com/idujin/ATMController
cd ATMController
```
### Prerequisites
Visual studio 2022 (v143)

### Build
Open `ATMController.sln` with VS2022 and Build with Debug/Release x64 configuration. 

or with Developement Command Prompt 

```
msbuild ATMController.sln /p:Configuration={Release or Debug},Platform=x64
```

### Run 
You need BankAPI and UserInterface class which are inherited from IBankAPI and IUserInterface to use ATMController. 
The interfaces are defined in IBankAPI.h and IUserInterface.h.

If you have the BankAPI and the UserInterface class, call Process method in ATMController with the classes.

The process handles reading a card from a user => recieving PIN => selecting account => doing bank jobs (balance/deposit/withraw).
During the process, if the user insert an invalid card or information, returns false. If it finished sucessfully, it returns true.
Please refer ATMController.UnitTest.cpp. There are some examples to use for several input cases.

### Test
Open Test Explorer and Run all tests

or on command line
```
vstest.console.exe ATMController.UnitTest.dll
```

The ATMController.UnitTest project includes the mock classes that implement the interfaces of the BankAPI and UserInterface. (See the files MockBankAPI.h and MockUserInterface.h.)
There are 19 test methods in ATMController.UnitTest.cpp for each different input cases. 
For example, the method of ProcessTestDepositAndWithdraw handles the user who wants to deposit $300 and withdraw $500 for the account with $200.




