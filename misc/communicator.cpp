#include <string.h>
#include "parameters.h"
#include </usr/include/python2.7/Python.h> // python2.7 Needed on minnowboard
//#include </usr/include/python2.7/pythonrun.h>

using namespace std;

//Somewhere there.
/*struct motor_command{
	int action;  //Either READ or DRIVE
	int board_address;
	int motor_num;
	int speed;
	int PID;
	double result;
};*/



double run_python(motor_command *command){

//Motor_command has 
/*
struct motor_command{
	int action;  //Either READ or DRIVE
	int board_address;
	int motor_num;
	int speed;
	int PID;
	double result;
};
*/
//A routine that will run a python function that is in the same directory, with arguments. This routine is specialized to the files 
// drive_motor.py with a function run_drive(board_num, motor, speed) and read_encoders.py with a function run_enc(board_num, motor).
	
	Py_Initialize();  //Initialize the python interpreter
	
			/***Vars set up*****/
	PyObject *pName, *pModule, *pDict, *pFunc;

	string pyName, pyFunc; //Declaration of the string and int
	int speed;

	if (command->action == READ){  //READ or DRIVE
		pyName = READ_ENC_FILE; //Determining which python module to call
		pyFunc = READ_FUNC;  //These strings all defined in parameters.h
	}else{
		pyName = MOTOR_FILE;
		pyFunc = MOTOR_FUNC;
		speed = command->speed;  //Additionally gets the speed. 
	}
	
	int board_address = command->board_address;
	int motor = command->motor_num;
	
	
			/***Set path and import module and function*****/
	PyRun_SimpleString("import sys");  //Set the path for the python libraries
	PyRun_SimpleString("sys.path.append(\".\")");
	
	pName = PyString_FromString(pyName.c_str()); //Convert to python object string (name of file)
	pModule = PyImport_Import(pName); // Import the file into the interpreter
	Py_INCREF(pModule); //Save on python stack
	
	if (pModule == NULL){ //Error checking
		printf("pModule is Null\n");
		return ERRORNO;
	}

	pDict = PyModule_GetDict(pModule); //Get list of functions
	Py_INCREF(pDict); //Save on python stack
	
	if (pDict == NULL){
		printf("pDict is Null\n");
		return ERRORNO;
	}
	pFunc = PyDict_GetItemString(pDict, pyFunc.c_str());  // Load the function from the file
	if (pFunc == NULL){
		printf("pFunc is Null\n");
		return ERRORNO;
	}
	Py_INCREF(pFunc); //Save on python stack

	PyObject* args;
	
			/***Create the arg string*****/

	if(command->action == READ){
		args = PyTuple_Pack(2,PyInt_FromLong(board_address),PyInt_FromLong(motor)); //Appropriate args for the read_encoders
	}else{ //Action is DRIVE
		args = PyTuple_Pack(3,PyInt_FromLong(board_address),PyInt_FromLong(motor), PyInt_FromLong(speed)); //Appropriate args for the drive_motor
	} 

	if (args == NULL){
		printf("Problem with args.\n");
		return ERRORNO;
	}
	Py_INCREF(args); //Save on python stack
	PyObject* myResult;

			/***Call function and get result*****/
	if (PyCallable_Check(pFunc)){
		myResult = PyObject_CallObject(pFunc, args);  //Call function
		if (myResult != NULL){  //If no error, then save myResult on stack
			Py_INCREF(myResult);
		}
	}else{
		PyErr_Print();
	}

			/***Report the result*****/
	double result = ERRORNO;
	if (myResult != NULL){
		result = PyFloat_AsDouble(myResult);  //If no error, get value from the encoders. 
		Py_DECREF(myResult);
		command->result = result;
	}

			/***Dereference python vars*****/
	Py_DECREF(pModule); //Dereference all python variables.
	Py_DECREF(pDict);
	Py_DECREF(pFunc);
	Py_DECREF(args);
	
	Py_Finalize(); //Close python interpreter -- this could be a very bad idea (Actually, it seems to work.
	return result;
}
