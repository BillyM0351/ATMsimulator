/*
Description: This program manages records for a simulated bank
Input:  CSV files with bank member information
Output: List of options for the user to access about their account
*/

#include "hashMap.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <string>
	using namespace std;

struct bankType
{
	struct transactionType
	{
		transactionType(bool t, double a)
		{
			type = t;
			amount = a;
		}

		bool type;
		double amount;
	};

	bankType(string first, string last, string acctNo,
		double balance,unsigned int pin)
	{
		name = last + ", " + first;
		this->balance = balance;
		this->pin = pin;
		this->acctNo = acctNo;
		locked = false;
	}

	bankType()
	{
		name = "";
		balance = 0;
		pin = 0;
		locked = false;
	}

	string name;
	string acctNo;
	double balance;
	unsigned int pin;
	bool locked;

	vector<transactionType> transactions;
};

/*
askToContinue function
parameters: N/A
return value: True if user wishes to continue, else false.
description: This function asks the user to continue. The program
			 terminates if the user selects no.
*/
bool askToContinue() {
	cout << "Continue? (Y/N): " << endl;
	string input;
	cin >> input;

	while (input != "Y" && input != "y" && input!= "n" && input != "N") {
		cout << "Continue? (Y/N): " << endl;
		cin >> input;
	};

	if (input == "y" || input == "Y") {return true;} 
	if (input == "n" || input == "N") {exit(0);}

	return false;
};

/*
menu function
parameters: N/A
return value: N/A
description: This function prints a menu
*/
void menu() {
	cout << "(D)eposit" << endl
	     << "(W)ithdrawal" << endl
		 << "(V)iew account" << endl
		 << "(E)xit account" << endl
		 << "Enter choice: ";
};

int main()
{
	// Declare hashMap objects and set precision to 2 for proper balance output
	cout << fixed << setprecision(2);
	hashMap<string, hashMap<string, bankType>> bank;
	hashMap<string, bool> valid;
	
	// Open and extract contents from file
	ifstream myFile;
	myFile.open("data.csv");
	string line;

	// Skip first line with labels in data.csv file
	getline(myFile, line);

	// Extract all the variables from each line in the data file
	while(!myFile.eof()) {
		getline(myFile, line);

		if (line != "") {

			string firstName = line.substr(0, line.find(","));
			line.erase(0, line.find(",") + 1);
			string lastName = line.substr(0, line.find(","));
			line.erase(0, line.find(",") + 1);
			string location = line.substr(0, line.find(","));
			line.erase(0, line.find(",") + 1);
			string acctNo = line.substr(0, line.find(","));
			line.erase(0, line.find(",") + 1);
			string stringBalance = line.substr(0, line.find(","));
			line.erase(0, line.find(",") + 1);
			string stringPin = line.substr(0, line.find("\r"));

			// Convert strings to proper data types
			double balance = stod(stringBalance);
			string acct4Digits = acctNo.substr(acctNo.size()-4, 4);
			unsigned int pin = stoi(stringPin);

			// Store line into hashMap
			bank[location][acct4Digits] = bankType(firstName, lastName, acctNo, balance, pin);

			// Initialize map of locations
			valid[location] = true;
		};
	};

	// Close file
	myFile.close();

	// Initialize lockCounter to 0
	int lockCounter = 0;
	bool fromTheTop = true;

	// This loop continues until the user selects no to end the program
	// If at any point a user elects to continue, the loop repeats
	while (fromTheTop == true) {

		// Declare and reset variables with each loop
		string location;
		string last4;
		int pin;
		bool accountFound = false;
		bool locked = false;

		// Prompt user to enter a bank branch location
		cout << endl;

		// Check if location is valid
		while (valid[location] != true) {
			fromTheTop = false;
			cout << "Please enter bank branch location: ";
			// Store input from user into location variable
			getline(cin >> ws, location);
			cout << endl;

			// Manipulate string to match case in locations hashMap
			transform(location.begin(), location.end(), location.begin(), ::tolower);
			location[0] = toupper(location[0]);

			for (int i = 0; i < location.size(); i++) {
				if (location[i] == ' ') {
					location[i+1] = toupper(location[i+1]);
				};
			};

			// Check if location exists, if not then ask user if they would like to continue
			if (valid[location] != true) {
				cout << "Bank branch not found" << endl << endl;
				fromTheTop = askToContinue();
			};
		};

		// Prompt user for last 4 digits of account to check if account exists
		while (accountFound == false && fromTheTop == false) {
			cout << "Enter last 4 digits of account: ";
			cin >> last4;
			cout << endl;

			// If pin is 4 characters long, then the account number must exist
			pin = bank[location][last4].pin;
			string temp = to_string(pin);

			if (temp.size() != 4) {
				cout << "Account not found" << endl << endl;
				fromTheTop = askToContinue();
			} else {
				accountFound = true;
			};
		};

		// Check account for a lock
		if (bank[location][last4].locked == true && fromTheTop == false) {
			cout << "Account has a lock" << endl;
			fromTheTop = askToContinue();
		};

		// If account is not locked then ask the user for the pin
		while (bank[location][last4].locked == false && fromTheTop == false) {
			cout << "Enter a pin: ";
			cin >> pin;
			cout << endl;

			// If the pin is valid then user will have access to the menu
			if (pin == bank[location][last4].pin) {
				lockCounter = 0;
				bool displayMenu = true;
				while (displayMenu = true && fromTheTop == false) {
				char selection;
				menu();
				cin >> selection;
				selection = toupper(selection);
				
				// Prompt user to make a selection
				switch (selection) {
					// If the user selects D then the balance will be increased by the deposit amount
					case 'D':
						double depositAmount;
						cout << "Enter deposit amount: ";
						cin >> depositAmount;
						bank[location][last4].transactions.push_back(bankType::transactionType(true, depositAmount));
						bank[location][last4].balance = bank[location][last4].balance + depositAmount;
						break;
					
					// If the user selects W then the balance will be decreased by the withdrawal amount
					case 'W':
						double withdrawalAmount;
						cout << "Enter withdrawal amount: ";
						cin >> withdrawalAmount;

						if (withdrawalAmount > bank[location][last4].balance) {
							cout << "Not enough funds to make transaction" << endl;
							break;
						} else {
						bank[location][last4].transactions.push_back(bankType::transactionType(false, withdrawalAmount));
						bank[location][last4].balance = bank[location][last4].balance - withdrawalAmount;
						};
						break;

					// If the user selects V then the name, balance and list of transactions will be displayed
					case 'V':
						cout << endl;
						cout << "Name: " << bank[location][last4].name << endl;
						cout << "Balance: $" << bank[location][last4].balance << endl << endl;
						if (bank[location][last4].transactions.size() == 0) {
							cout << "No transactions" << endl;
						} else {
							cout << "List of transactions" << endl;
							for (int i = 0; i < bank[location][last4].transactions.size(); i++) {
								if (bank[location][last4].transactions[i].type == true) {
									cout << "Deposit amount $" << bank[location][last4].transactions[i].amount << endl;
								} else {
									cout << "Withdrawal amount $" << bank[location][last4].transactions[i].amount << endl;
								};
							};
						};
						break;

					// If the user selects E then the user will be asked if they would like to continue
					case 'E':
						fromTheTop = askToContinue();
						break;

				};
					cout << endl;
				};
			};

			// If the pin entered was incorrect then increment lock counter by 1
			if (fromTheTop == false) {
				cout << "Invalid Pin" << endl << endl;
				lockCounter++;

				// If lock counter is equal to 3 then lock the account
				if (lockCounter == 3) {
					cout << "3 failed login attempts, account is now locked" << endl;
					bank[location][last4].locked = true;
					fromTheTop = askToContinue();
				};
			};
		};
	};

	return 0;
}
