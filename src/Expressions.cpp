#include "Epressions.h"
#include "Variables.h"
#include"ExpressionInfo.h"
#include "VariableInfo.h"
#include <fstream>

Expressions::Expressions() {
	numExpresssions = 0;
}
int Expressions::getNumExpressions() {
	return numExpresssions;
}
void Expressions::addExpression(ExpressionInfo newExpression) {
	expressionsVector.push_back(newExpression);
	numExpresssions = numExpresssions + 1;
}


int Expressions::scanExpressions(string inputFile, Variables variableVector) { // scans in expressions from file, returns an integer for error code
	int error = 0;
	int outputerror = 0;
	ifstream inFS;
	inFS.open(inputFile);
	string tempstring;
	int skipline = 0;
	string outputstring;
	string input1string;
	string input2string;
	char operationchar;
	char check = 0;
	int peek = 0;
	vector <string> newVarList;


	while (!inFS.eof()) {
		inFS >> tempstring;
		skipline = 0;
		check = 0;
		if (tempstring == "input" || tempstring == "output" || tempstring == "wire" || tempstring == "register") {
			skipline = 1;
		}

		while (skipline != 1 && peek == 0) { //when now reading in expressions
			outputstring = tempstring;
			newVarList.push_back(tempstring);

			outputerror = checkVariables(tempstring + '\0', 'o', variableVector); //check if variable was declared

			inFS >> tempstring;
			inFS >> tempstring;
			input1string = tempstring;
			newVarList.push_back(tempstring);
			error = checkVariables(tempstring + '\0', 'i', variableVector);
			if (error != 0) {
				return error + outputerror;
			}

			inFS.get(operationchar);
			if (operationchar != '\n') {
				inFS.get(operationchar);
			}

			ExpressionInfo tempExInfo;
			if (operationchar == '\n' || inFS.eof()) { //saves info as register
				tempExInfo.setExType("REG");
				tempExInfo.setInput1(input1string);
				tempExInfo.setOutput(outputstring);
			}
			else if (operationchar == '+') { //reads info and saves strings for add
				tempExInfo.setExType("ADD");
				tempExInfo.setInput1(input1string);
				tempExInfo.setOutput(outputstring);
				inFS >> input2string;
				newVarList.push_back(input2string);
				error = checkVariables(input2string + '\0', 'i', variableVector);
				if (error != 0) {
					return error + outputerror;
				}
				tempExInfo.setInput2(input2string);
			}
			else if (operationchar == '-') { //reads info and saves strings for sub
				tempExInfo.setExType("SUB");
				tempExInfo.setInput1(input1string);
				tempExInfo.setOutput(outputstring);
				inFS >> input2string;
				newVarList.push_back(input2string);
				error = checkVariables(input2string + '\0', 'i', variableVector);
				if (error != 0) {
					return error + outputerror;
				}
				tempExInfo.setInput2(input2string);
			}
			else if (operationchar == '*') { //reads info and saves strings for mult
				tempExInfo.setExType("MUL");
				tempExInfo.setInput1(input1string);
				tempExInfo.setOutput(outputstring);
				inFS >> input2string;
				newVarList.push_back(input2string);
				error = checkVariables(input2string + '\0', 'i', variableVector);
				if (error != 0) {
					return error + outputerror;
				}
				tempExInfo.setInput2(input2string);
			}
			else if (operationchar == '>') { //checks if SFR or comp (gt)
				inFS.get(operationchar);
				inFS >> tempstring;
				newVarList.push_back(tempstring);
				error = checkVariables(tempstring + '\0', 'i', variableVector);
				if (error != 0) {
					return error + outputerror;
				}
				if (operationchar == '>') { //reads info and saves strings for SHR
					tempExInfo.setExType("SHR");
					tempExInfo.setInput1(input1string);
					tempExInfo.setOutput(outputstring);
					tempExInfo.setSftAmount(tempstring);
				}
				else {
					tempExInfo.setExType("COMP"); //reads info and saves string for the comp (gt)
					tempExInfo.setInput1(input1string);
					tempExInfo.setOutput(outputstring);
					tempExInfo.setInput2(tempstring);
					tempExInfo.setComp_Op("gt");
				}

			}
			else if (operationchar == '<') { //checks if SHL or comp (lt)
				inFS.get(operationchar);
				inFS >> tempstring;
				newVarList.push_back(tempstring);
				error = checkVariables(tempstring + '\0', 'i', variableVector);
				if (error != 0) {
					return error + outputerror;
				}
				if (operationchar == '<') { //reads info and save strings for SHL
					tempExInfo.setExType("SHL");
					tempExInfo.setInput1(input1string);
					tempExInfo.setOutput(outputstring);
					tempExInfo.setSftAmount(tempstring);
				}
				else {
					tempExInfo.setExType("COMP"); //reads info and saves string for the comp (lt)
					tempExInfo.setInput1(input1string);
					tempExInfo.setOutput(outputstring);
					tempExInfo.setInput2(tempstring);
					tempExInfo.setComp_Op("lt");
				}

			}
			else if (operationchar == '=') { //reads info and saves string for the comp (eq)
				tempExInfo.setExType("COMP");
				inFS.get(operationchar);
				tempExInfo.setInput1(input1string);
				tempExInfo.setOutput(outputstring);
				inFS >> input2string;
				newVarList.push_back(input2string);
				error = checkVariables(input2string + '\0', 'i', variableVector);
				if (error != 0) {
					return error + outputerror;
				}
				tempExInfo.setInput2(input2string);
				tempExInfo.setComp_Op("eq");
			}
			else if (operationchar == '?') { //reads info and saves strings for the Mux
				tempExInfo.setExType("MUX2x1");
				tempExInfo.setSel(input1string);
				tempExInfo.setOutput(outputstring);
				inFS >> input1string;
				newVarList.push_back(input1string);
				error = checkVariables(input1string + '\0', 'i', variableVector);
				if (error != 0) {
					return error + outputerror;
				}
				inFS >> tempstring;
				inFS >> input2string;
				newVarList.push_back(input2string);
				error = checkVariables(input2string + '\0', 'i', variableVector);
				if (error != 0) {
					return error + outputerror;
				}
				tempExInfo.setInput1(input1string);
				tempExInfo.setInput2(input2string);


			}
			else {
				return 4; //returns error for unknown operation
			}

			expressionsVector.push_back(tempExInfo);
			numExpresssions = numExpresssions + 1;
			check = inFS.peek();
			while (check != '\n' && operationchar != '\n') { // if the next character is not a new line character, then this while loop will find the next
				inFS.get(check);							//'\n' character or end of file
				if (inFS.eof()) {
					check = '\n';
				}
			}
			if (inFS.eof()) {
				break;
			}
			inFS >> tempstring;

		}
		check = inFS.peek();
		while (check != '\n') { // if the next character is a not new line character, while loop will find next '\n' or end of file
			inFS >> check;
			check = inFS.peek();
			if (inFS.eof()) {
				check = '\n';
			}
		}




	}

	return error + outputerror;

}
void Expressions::printExpressions(string outputFIle, Variables list) {  // writes the expressions in verilog format in output file
	ofstream printFile;
	int i;
	int j;
	int input1pos;
	int input2pos;
	int outputpos;
	int sign = 0;
	int input1sign = 0;
	int input2sign = 0;
	int maxbitsize = 0;


	
	printFile.open(outputFIle);
	if (printFile.is_open()) {
		printFile << "`timescale 1ns / 1ps\n";
		printFile << "module Circuit(";
		//printFile << list.getNumVariables
		for (i = 0; i < numExpresssions; ++i) {
			if (expressionsVector.at(i).getExType() == "REG") {
				printFile << "Clk, Rst, ";
				i = numExpresssions;
			}
		}
		for (i = 0; i < numExpresssions; ++i) {
			if (expressionsVector.at(i).getExType() == "COMP") {
				printFile << "GT, LT, EQ, ";
				i = numExpresssions;
			}
		}
		for (i = 0; i < list.getNumVariables(); i++) {
			if ((list.getVarInfo(i).getType() == 'i') | (list.getVarInfo(i).getType() == 'o')) {
				printFile << list.getVarInfo(i).getVarName();
				if ((i + 1) != list.getNumIO()) {
					printFile << ", ";
				}
			}
		}
		printFile << ")\n";

		for (i = 0; i < list.getNumVariables(); i++) {
			if (list.getVarInfo(i).getType() == 'i') {
				printFile << "\tinput ";
				if (list.getVarInfo(i).getSign() == 's') {
					printFile << "signed ";
				}
				if (list.getVarInfo(i).getNumBits() != 1) {
					printFile << "[";
					printFile << (list.getVarInfo(i).getNumBits() - 1);
					printFile << ":0] ";
				}
				printFile << list.getVarInfo(i).getVarName();
				if ((i + 1) != list.getNumVariables()) {
					while ((list.getVarInfo(i).getNumBits() == list.getVarInfo(i + 1).getNumBits()) && (list.getVarInfo(i + 1).getType() == 'i') && (list.getVarInfo(i).getSign() == list.getVarInfo(i + 1).getSign())) {
						if (i + 1== list.getNumVariables()) {
							break;
						}
						else {
							i = i + 1;
						}
						printFile << ", " << list.getVarInfo(i).getVarName();

					}
					
				}
				printFile << ";\n";
				if (i + 1 == list.getNumVariables()) {
					break;
				}
			}
		}


		for (i = 0; i < list.getNumVariables(); i++) {
			if (list.getVarInfo(i).getType() == 'o') {
				printFile << "\toutput reg ";
				if (list.getVarInfo(i).getSign() == 's') {
					printFile << "signed ";
				}
				if (list.getVarInfo(i).getNumBits() != 1) {
					printFile << "[";
					printFile << (list.getVarInfo(i).getNumBits() - 1);
					printFile << ":0] ";
				}
				printFile << list.getVarInfo(i).getVarName();
					if ((i + 1) != list.getNumVariables()) {
						while ((list.getVarInfo(i).getNumBits() == list.getVarInfo(i + 1).getNumBits()) && (list.getVarInfo(i + 1).getType() == 'o') && (list.getVarInfo(i).getSign() == list.getVarInfo(i + 1).getSign())) {
							if (i + 1 == list.getNumVariables()) {
								break;
							}
							else {
								i = i + 1;
							}
							printFile << ", " << list.getVarInfo(i).getVarName();

						}
						
					}
				printFile << ";\n";
				if (i + 1 == list.getNumVariables()) {
					break;
				}
			}
		}

		for (i = 0; i < list.getNumVariables(); i++) {
			if (list.getVarInfo(i).getType() == 'w') {
				printFile << "\twire ";
				if (list.getVarInfo(i).getSign() == 's') {
					printFile << "signed ";
				}
				if (list.getVarInfo(i).getNumBits() != 1) {
					printFile << "[";
					printFile << (list.getVarInfo(i).getNumBits() - 1);
					printFile << ":0] ";
				}
				printFile << list.getVarInfo(i).getVarName();
				
				if ((i + 1) != list.getNumVariables()) {
					while ((list.getVarInfo(i).getNumBits() == list.getVarInfo(i + 1).getNumBits()) && (list.getVarInfo(i + 1).getType() == 'w') && (list.getVarInfo(i).getSign() == list.getVarInfo(i + 1).getSign())) {
						if (i + 1 == list.getNumVariables()) {
							break;
						}
						else {
							i = i + 1;
						}
						printFile << ", " << list.getVarInfo(i).getVarName();
						if (i + 1 == list.getNumVariables()) {
							break;
						}
					}
					
				}
				printFile << ";\n";
			}
		}

		for (i = 0; i < numExpresssions; ++i) {
			if (expressionsVector.at(i).getExType() == "REG") {
				printFile << "\tinput Clk, Rst;\n";
				i = numExpresssions;
			}
		}
		for (i = 0; i < numExpresssions; ++i) {
			if (expressionsVector.at(i).getExType() == "COMP") {
				printFile << "\toutput reg GT, LT, EQ;\n";
				i = numExpresssions;
			}
		}
		for (i = 0; i < list.getNumVariables(); i++) {
			if (list.getVarInfo(i).getNumBits() > maxbitsize) {
				maxbitsize = list.getVarInfo(i).getNumBits();
			}
		 }

		printFile << "\n\talways @(*) begin\n";

		for (i = 0; i < numExpresssions; i++) {
			sign = 0;
			input1sign = 0;
			input2sign = 0;
			for (j = 0; j < list.getNumVariables(); ++j) {
				if (list.getVarInfo(j).getVarName() == expressionsVector.at(i).getInput1()) {
					input1pos = j;
					if (list.getVarInfo(j).getSign() == 's') {
						sign = 1;
						input1sign = 1;
					}
				}
				if (list.getVarInfo(j).getVarName() == expressionsVector.at(i).getInput2()) {
					input2pos = j;
					if (list.getVarInfo(j).getSign() == 's') {
						sign = 1;
						input2sign = 1;
					}
				}
				if (list.getVarInfo(j).getVarName() == expressionsVector.at(i).getOutput()) {
					outputpos = j;
					if (list.getVarInfo(j).getSign() == 's') {
						sign = 1;
					}
				}
			}
			printFile << "\t\t";
			if (sign == 1) {
				printFile << "S";
			} 
			printFile << expressionsVector.at(i).getExType();
			printFile << "#(.DATAWIDTH(";
			//printFile << maxbitsize;
			//printFile << ")) ";
			//printFile << expressionsVector.at(i).getExType();
			//printFile << i;
			//printFile << " (";

			
			if (expressionsVector.at(i).getExType() == "REG") {
				
				printFile << list.getVarInfo(outputpos).getNumBits();
				printFile << ")) ";
				printFile << expressionsVector.at(i).getExType();
				printFile << i;
				printFile << " (";
				if (list.getVarInfo(input1pos).getNumBits() > list.getVarInfo(outputpos).getNumBits()) {
					printFile << expressionsVector.at(i).getInput1();
					if (list.getVarInfo(outputpos).getNumBits() == 1) {
						printFile << "[0]";
					}
					else {
						printFile << "[" << list.getVarInfo(outputpos).getNumBits() - 1 << ":0]";
					}
					
				}
				else if (list.getVarInfo(input1pos).getNumBits() < list.getVarInfo(outputpos).getNumBits()) {
					if (input1sign == 1) {
						printFile << "{{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input1pos).getNumBits() << "{";
						printFile << expressionsVector.at(i).getInput1() << "[" << list.getVarInfo(input1pos).getNumBits() - 1;
						printFile << "]}, " << expressionsVector.at(i).getInput1() << "}";
					}
					else {
						printFile << "{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input1pos).getNumBits() << "'b0, ";
						printFile << expressionsVector.at(i).getInput1() << "}";
					}
				}
				else {
					printFile << expressionsVector.at(i).getInput1();
				}
				printFile << ", Clk, Rst, ";
				printFile << expressionsVector.at(i).getOutput() << ");\n";
			}
			else if (expressionsVector.at(i).getExType() == "SHR") {
				printFile << list.getVarInfo(outputpos).getNumBits();
				printFile << ")) ";
				printFile << expressionsVector.at(i).getExType();
				printFile << i;
				printFile << " (";
				if (list.getVarInfo(input1pos).getNumBits() > list.getVarInfo(outputpos).getNumBits()) {
					printFile << expressionsVector.at(i).getInput1();
					if (list.getVarInfo(outputpos).getNumBits() == 1) {
						printFile << "[0]";
					}
					else {
						printFile << "[" << list.getVarInfo(outputpos).getNumBits() - 1 << ":0]";
					}
				}
				else if (list.getVarInfo(input1pos).getNumBits() < list.getVarInfo(outputpos).getNumBits()) {
					if (input1sign == 1) {
						printFile << "{{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input1pos).getNumBits() << "{";
						printFile << expressionsVector.at(i).getInput1() << "[" << list.getVarInfo(input1pos).getNumBits() - 1;
						printFile << "]}, " << expressionsVector.at(i).getInput1() << "}";
					}
					else {
						printFile << "{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input1pos).getNumBits() << "'b0, ";
						printFile << expressionsVector.at(i).getInput1() << "}";
					}
				}
				else {
					printFile << expressionsVector.at(i).getInput1();
				}
				printFile << ", ";
				printFile << expressionsVector.at(i).getSftAmount();
				printFile << ", ";
				printFile << expressionsVector.at(i).getOutput();
				printFile << ");\n";
			}

			else if (expressionsVector.at(i).getExType() == "SHL") {
				printFile << list.getVarInfo(outputpos).getNumBits();
				printFile << ")) ";
				printFile << expressionsVector.at(i).getExType();
				printFile << i;
				printFile << " (";
				if (list.getVarInfo(input1pos).getNumBits() > list.getVarInfo(outputpos).getNumBits()) {
					printFile << expressionsVector.at(i).getInput1();
					if (list.getVarInfo(outputpos).getNumBits() == 1) {
						printFile << "[0]";
					}
					else {
						printFile << "[" << list.getVarInfo(outputpos).getNumBits() - 1 << ":0]";
					}
				}
				else if (list.getVarInfo(input1pos).getNumBits() < list.getVarInfo(outputpos).getNumBits()) {
					if (input1sign == 1) {
						printFile << "{{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input1pos).getNumBits() << "{";
						printFile << expressionsVector.at(i).getInput1() << "[" << list.getVarInfo(input1pos).getNumBits() - 1;
						printFile << "]}, " << expressionsVector.at(i).getInput1() << "}";
					}
					else {
						printFile << "{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input1pos).getNumBits() << "'b0, ";
						printFile << expressionsVector.at(i).getInput1() << "}";
					}
				}
				else {
					printFile << expressionsVector.at(i).getInput1();
				}
				printFile << ", ";
				printFile << expressionsVector.at(i).getSftAmount();
				printFile << ", ";
				printFile << expressionsVector.at(i).getOutput();
				printFile << ");\n";
			}

			else if (expressionsVector.at(i).getExType() == "COMP") {
				if (list.getVarInfo(input1pos).getNumBits() > list.getVarInfo(input2pos).getNumBits()) {
					printFile << list.getVarInfo(input1pos).getNumBits();
				}
				else {
					printFile << list.getVarInfo(input2pos).getNumBits();
				}
				
				printFile << ")) ";
				printFile << expressionsVector.at(i).getExType();
				printFile << i;
				printFile << " (";
				if (list.getVarInfo(input2pos).getNumBits() > list.getVarInfo(input1pos).getNumBits()) {
					if (input1sign == 1) {
						printFile << "{{" << list.getVarInfo(input2pos).getNumBits() - list.getVarInfo(input1pos).getNumBits() << "{";
						printFile << expressionsVector.at(i).getInput1() << "[" << list.getVarInfo(input1pos).getNumBits() - 1;
						printFile << "]}, " << expressionsVector.at(i).getInput1() << "}";
					}
					else {
						printFile << "{" << list.getVarInfo(input2pos).getNumBits() - list.getVarInfo(input1pos).getNumBits() << "'b0, ";
						printFile << expressionsVector.at(i).getInput1() << "}";
					}
				}
				else {
					printFile << expressionsVector.at(i).getInput1();

				}

				printFile << ", ";

				if (list.getVarInfo(input2pos).getNumBits() < list.getVarInfo(input1pos).getNumBits()) {
					if (input2sign == 1) {
						printFile << "{{" << list.getVarInfo(input1pos).getNumBits() - list.getVarInfo(input2pos).getNumBits() << "{";
						printFile << expressionsVector.at(i).getInput2() << "[" << list.getVarInfo(input2pos).getNumBits() - 1;
						printFile << "]}, " << expressionsVector.at(i).getInput2() << "}";
					}
					else {
						printFile << "{" << list.getVarInfo(input1pos).getNumBits() - list.getVarInfo(input2pos).getNumBits() << "'b0, ";
						printFile << expressionsVector.at(i).getInput2() << "}";
					}
				}
				else {
					printFile << expressionsVector.at(i).getInput2();
					printFile << ", ";
				}
				if ((expressionsVector.at(i).getComp_Op()) == "gt") {

				
				
					printFile << expressionsVector.at(i).getOutput();
					printFile << ", LT, EQ";
					printFile << ");\n";
				}

				if ((expressionsVector.at(i).getComp_Op()) == "lt") {
					printFile << "GT, ";
					printFile << expressionsVector.at(i).getOutput();
					printFile << ", EQ";
					printFile << ");\n";
				}

				if ((expressionsVector.at(i).getComp_Op()) == "eq") {
					printFile << "GT, LT, ";
					printFile << expressionsVector.at(i).getOutput();
					printFile << ");\n";
				}
			}

			else if (expressionsVector.at(i).getExType() == "MUX2x1") {
				printFile << list.getVarInfo(outputpos).getNumBits();
				printFile << ")) ";
				printFile << expressionsVector.at(i).getExType();
				printFile << i;
				printFile << " (";
				if (list.getVarInfo(input1pos).getNumBits() > list.getVarInfo(outputpos).getNumBits()) {
					printFile << expressionsVector.at(i).getInput1();
					if (list.getVarInfo(outputpos).getNumBits() == 1) {
						printFile << "[0]";
					}
					else {
						printFile << "[" << list.getVarInfo(outputpos).getNumBits() - 1 << ":0]";
					}
				}
				else if (list.getVarInfo(input1pos).getNumBits() < list.getVarInfo(outputpos).getNumBits()) {
					if (input1sign == 1) {
						printFile << "{{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input2pos).getNumBits() << "{";
						printFile << expressionsVector.at(i).getInput1() << "[" << list.getVarInfo(input1pos).getNumBits() - 1;
						printFile << "]}, " << expressionsVector.at(i).getInput1() << "}";
					}
					else {
						printFile << "{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input1pos).getNumBits() << "'b0, ";
						printFile << expressionsVector.at(i).getInput1() << "}";
					}
				}
				else {
					printFile << expressionsVector.at(i).getInput1();
				}
				printFile << ", ";
				if (list.getVarInfo(input2pos).getNumBits() > list.getVarInfo(outputpos).getNumBits()) {
					printFile << expressionsVector.at(i).getInput2();
					if (list.getVarInfo(outputpos).getNumBits() == 1) {
						printFile << "[0]";
					}
					else {
						printFile << "[" << list.getVarInfo(outputpos).getNumBits() - 1 << ":0]";
					}
				}
				else if (list.getVarInfo(input2pos).getNumBits() < list.getVarInfo(outputpos).getNumBits()) {
					if (input2sign == 1) {
						printFile << "{{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input2pos).getNumBits() << "{";
						printFile << expressionsVector.at(i).getInput2() << "[" << (list.getVarInfo(input2pos).getNumBits() - 1);
						printFile << "]}, " << expressionsVector.at(i).getInput2() << "}";
					}
					else {
						printFile << "{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input2pos).getNumBits() << "'b0, ";
						printFile << expressionsVector.at(i).getInput2() << "}";
					}
				}
				else {
					printFile << expressionsVector.at(i).getInput2();
				}
				printFile << ", ";
				printFile << expressionsVector.at(i).getOutput();
				printFile << ", ";
				printFile << expressionsVector.at(i).getSel();
				printFile << ");\n";
			}

			else if (expressionsVector.at(i).getExType() == "ADD") {
				printFile << list.getVarInfo(outputpos).getNumBits();
				printFile << ")) ";
				printFile << expressionsVector.at(i).getExType();
				printFile << i;
				printFile << " (";
				if (list.getVarInfo(input1pos).getNumBits() > list.getVarInfo(outputpos).getNumBits()) {
					printFile << expressionsVector.at(i).getInput1();
					if (list.getVarInfo(outputpos).getNumBits() == 1) {
						printFile << "[0]";
					}
					else {
						printFile << "[" << list.getVarInfo(outputpos).getNumBits() - 1 << ":0]";
					}
				}
				else if (list.getVarInfo(input1pos).getNumBits() < list.getVarInfo(outputpos).getNumBits()) {
					if (input1sign == 1) {
						printFile << "{{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input2pos).getNumBits() << "{";
						printFile << expressionsVector.at(i).getInput1() << "[" << list.getVarInfo(input1pos).getNumBits() - 1;
						printFile << "]}, " << expressionsVector.at(i).getInput1() << "}";
					}
					else {
						printFile << "{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input1pos).getNumBits() << "'b0, ";
						printFile << expressionsVector.at(i).getInput1() << "}";
					}
				}
				else {
					printFile << expressionsVector.at(i).getInput1();
				}
				printFile << ", ";
				if (list.getVarInfo(input2pos).getNumBits() > list.getVarInfo(outputpos).getNumBits()) {
					printFile << expressionsVector.at(i).getInput2();
					if (list.getVarInfo(outputpos).getNumBits() == 1) {
						printFile << "[0]";
					}
					else {
						printFile << "[" << list.getVarInfo(outputpos).getNumBits() - 1 << ":0]";
					}
				}
				else if (list.getVarInfo(input2pos).getNumBits() < list.getVarInfo(outputpos).getNumBits()) {
					if (input2sign == 1) {
						printFile << "{{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input2pos).getNumBits() << "{";
						printFile << expressionsVector.at(i).getInput2() << "[" << (list.getVarInfo(input2pos).getNumBits() - 1);
						printFile << "]}, " << expressionsVector.at(i).getInput2() << "}";
					}
					else {
						printFile << "{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input2pos).getNumBits() << "'b0, ";
						printFile << expressionsVector.at(i).getInput2() << "}";
					}
				}
				else {
					printFile << expressionsVector.at(i).getInput2();
				}
				printFile << ", ";
				printFile << expressionsVector.at(i).getOutput();
				printFile << ");\n";
			}

			else if (expressionsVector.at(i).getExType() == "SUB") {
				printFile << list.getVarInfo(outputpos).getNumBits();
				printFile << ")) ";
				printFile << expressionsVector.at(i).getExType();
				printFile << i;
				printFile << " (";
				if (list.getVarInfo(input1pos).getNumBits() > list.getVarInfo(outputpos).getNumBits()) {
					printFile << expressionsVector.at(i).getInput1();
					if (list.getVarInfo(outputpos).getNumBits() == 1) {
						printFile << "[0]";
					}
					else {
						printFile << "[" << list.getVarInfo(outputpos).getNumBits() - 1 << ":0]";
					}
				}
				else if (list.getVarInfo(input1pos).getNumBits() < list.getVarInfo(outputpos).getNumBits()) {
					if (input1sign == 1) {
						printFile << "{{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input1pos).getNumBits() << "{";
						printFile << expressionsVector.at(i).getInput1() << "[" << list.getVarInfo(input1pos).getNumBits() - 1;
						printFile << "]}, " << expressionsVector.at(i).getInput1() << "}";
					}
					else {
						printFile << "{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input1pos).getNumBits() << "'b0, ";
						printFile << expressionsVector.at(i).getInput1() << "}";
					}
				}
				else {
					printFile << expressionsVector.at(i).getInput1();
				}
				printFile << ", ";
				if (list.getVarInfo(input2pos).getNumBits() > list.getVarInfo(outputpos).getNumBits()) {
					printFile << expressionsVector.at(i).getInput2();
					if (list.getVarInfo(outputpos).getNumBits() == 1) {
						printFile << "[0]";
					}
					else {
						printFile << "[" << list.getVarInfo(outputpos).getNumBits() - 1 << ":0]";
					}
				}
				else if (list.getVarInfo(input2pos).getNumBits() < list.getVarInfo(outputpos).getNumBits()) {
					if (input2sign == 1) {
						printFile << "{{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input2pos).getNumBits() << "{";
						printFile << expressionsVector.at(i).getInput2() << "[" << (list.getVarInfo(input2pos).getNumBits() - 1);
						printFile << "]}, " << expressionsVector.at(i).getInput2() << "}";
					}
					else {
						printFile << "{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input2pos).getNumBits() << "'b0, ";
						printFile << expressionsVector.at(i).getInput2() << "}";
					}
				}
				else {
					printFile << expressionsVector.at(i).getInput2();
				}
				printFile << ", ";
				printFile << expressionsVector.at(i).getOutput();
				printFile << ");\n";
			}

			else if (expressionsVector.at(i).getExType() == "MUL") {
				printFile << list.getVarInfo(outputpos).getNumBits();
				printFile << ")) ";
				printFile << expressionsVector.at(i).getExType();
				printFile << i;
				printFile << " (";
				if (list.getVarInfo(input1pos).getNumBits() > list.getVarInfo(outputpos).getNumBits()) {
					printFile << expressionsVector.at(i).getInput1();
					if (list.getVarInfo(outputpos).getNumBits() == 1) {
						printFile << "[0]";
					}
					else {
						printFile << "[" << list.getVarInfo(outputpos).getNumBits() - 1 << ":0]";
					}
				}
				else if (list.getVarInfo(input1pos).getNumBits() < list.getVarInfo(outputpos).getNumBits()) {
					if (input1sign == 1) {
						printFile << "{{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input1pos).getNumBits() << "{";
						printFile << expressionsVector.at(i).getInput1() << "[" << list.getVarInfo(input1pos).getNumBits() - 1;
						printFile << "]}, " << expressionsVector.at(i).getInput1() << "}";
					}
					else {
						printFile << "{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input1pos).getNumBits() << "'b0, ";
						printFile << expressionsVector.at(i).getInput1() << "}";
					}
				}
				else {
					printFile << expressionsVector.at(i).getInput1();
				}
				printFile << ", ";
				if (list.getVarInfo(input2pos).getNumBits() > list.getVarInfo(outputpos).getNumBits()) {
					printFile << expressionsVector.at(i).getInput2();
					if (list.getVarInfo(outputpos).getNumBits() == 1) {
						printFile << "[0]";
					}
					else {
						printFile << "[" << list.getVarInfo(outputpos).getNumBits() - 1 << ":0]";
					}
				}
				else if (list.getVarInfo(input2pos).getNumBits() < list.getVarInfo(outputpos).getNumBits()) {
					if (input2sign == 1) {
						printFile << "{{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input2pos).getNumBits() << "{";
						printFile << expressionsVector.at(i).getInput2() << "[" << (list.getVarInfo(input2pos).getNumBits() - 1);
						printFile << "]}, " << expressionsVector.at(i).getInput2() << "}";
					}
					else {
						printFile << "{" << list.getVarInfo(outputpos).getNumBits() - list.getVarInfo(input2pos).getNumBits() << "'b0, ";
						printFile << expressionsVector.at(i).getInput2() << "}";
					}
				}
				else {
					printFile << expressionsVector.at(i).getInput2();
				}
				printFile << ", ";
				printFile << expressionsVector.at(i).getOutput();
				printFile << ");\n";
			}
		}

	}

	printFile << "\tend\nendmodule\n";


}
int Expressions::checkVariables(string varName, char varType, Variables list) {  //checks if the name and type of variable is saved in list of variables, 
	string name;																	//returns an integer for error code
	int i = 0;
	for (i = 0; i < list.getNumVariables(); i++) {
		name = list.getVarInfo(i).getVarName();
		if (name[name.length() - 1] != '\0') {
			name = name + '\0';
		}
		if (name.compare(varName) == 0) {
			return 0;
		}
	}
	if (varType == 'i') {
		return 1;
	}
	else if (varType == 'o') {
		return 2;
	}
	return 20;

}