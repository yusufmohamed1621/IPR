#include <iostream>
# include <cmath>
using namespace std;

string reservoirType(float averageReservoirPressure, float bubblePointPressure) {
	if (averageReservoirPressure <= bubblePointPressure) {
		return "Saturated Oil Reservoir";
	}
	else {
		return "Undersaturated Oil Reservoir";
	}
}
// Well PI IPR Method (Straight line Method);

float productivityIndex(float stabilizedFlowRate, float stabilizedBhfp, float averageReservoirPressure) {
	return stabilizedFlowRate / (averageReservoirPressure - stabilizedBhfp);
}
float wellPI(float Bhfp, float productivityIndex, float averageReservoirPressure) {
	return productivityIndex * (averageReservoirPressure - Bhfp);
}
float BHFP_PI(float averageReservoirPressure, float flowRate, float productivityIndex) {
	return averageReservoirPressure - (flowRate / productivityIndex);
}
float drawDown(float averageReservoirPressure, float Bhfp) {
	return averageReservoirPressure - Bhfp; 
}

// Vogel's IPR Method

float vogelQmax(float stabilizedFlowRate, float stabilizedBhfp, float averageReservoirPressure) {
	float pressureRatio = stabilizedBhfp / averageReservoirPressure;
	return stabilizedFlowRate / (1 - (0.2 * pressureRatio) - (0.8 * pow(pressureRatio, 2))); 
}
float vogelIPR(float Bhfp, float averageReservoirPressure, float Qmax) {
	float pressureRatio = Bhfp / averageReservoirPressure;
	return Qmax * (1 - (0.2 * pressureRatio) - (0.8 * pow(pressureRatio , 2)));
}
float BHFP_Vogel(float averageReservoirPressure, float flowRate, float vogel_Qmax) {
	return (averageReservoirPressure / 1.6) * (-0.2 + (pow( 0.04 - (3.2 * ((flowRate / vogel_Qmax) - 1)), 0.5)));
}

// Composite IPR Method (Well PI + Vogel) for undersaturated oil reservoirs.
//Case 1 : stabilized Pwf >= Pbubblepoint

float Q_bubblepoint(float PI_compositeabove, float averageReservoirPressure, float bubblePointPressure) {
	return PI_compositeabove * (averageReservoirPressure - bubblePointPressure);
}
float Vogel_Below(float Q_bubblepoint, float PI_compositeabove, float bubblePointPressure, float Bhfp) {
	float pressureRatio = Bhfp / bubblePointPressure; 
	return   Q_bubblepoint + (((PI_compositeabove*bubblePointPressure) / 1.8)*(1- 0.2*pressureRatio - 0.8 * (pow(pressureRatio , 2 ))));
}
float Composite_Qmax(float Q_bubblepoint, float PI_compositeabove, float bubblePointPressure) {
	return Q_bubblepoint + ((PI_compositeabove * bubblePointPressure)/1.8);
}

//Case 2 : stabilized Pwf < Pbubblepoint

float PI_compositebelow(float stabilizedFlowRate, float stabilizedBhfp, float averageReservoirPressure , float bubblePointPressure) {
	float pressureRatio = stabilizedBhfp / bubblePointPressure; 
	return stabilizedBhfp / ((averageReservoirPressure - bubblePointPressure) + ((bubblePointPressure / 1.8)*(1 - 0.2 * pressureRatio - 0.8 * pow(pressureRatio , 2))));
}
float BHFP_VogelBelow(float Q_bubblepoint, float PI_composite, float bubblePointPressure, float flowrate) {
	return ((bubblePointPressure / 1.6) * (-0.2 + (pow(0.04 - 3.2 * (((1.8 / (PI_composite * bubblePointPressure)) * (Q_bubblepoint - flowrate)) - 1), 0.5))));
}

// Future IPR Prediction (Fetkovich Approximation Method).
float QmaxPredicted(float presentQmax , float presentAverageReservoirpressure , float futureAverageReservoirPressure) {
	return  presentQmax * (pow((futureAverageReservoirPressure / presentAverageReservoirpressure), 3));
}

float stepSize(int pointsCount , float maximumPressure , float minimumPressure){
	return (maximumPressure - minimumPressure) / pointsCount; 
}

void horizontalLine() {
	cout << endl; 
	cout << "=========================================================================";
	cout << endl; 
}

int main()
{
	float averageReservoirPressure;
	float bubblePointPressure;
	float stabilizedFlowRate;
	float stabilizedBhfp;
	string startStop;
	int selection;
	int pointsCount;
	float inputBhfp;
	float inputFlowRate;
	float futureReservoirPressure;

	cout << "IPR Using Well Test Data" << endl << endl;
	cout << "Type any button to Proceed or 'x' to Exit" << endl;
	cin >> startStop;

	if (cin.fail() || startStop == "x" || startStop == "X") {
		return 0;
	}

	while (true){
		cout << "Please Enter the Required following Data : " << endl;

		while (true) {
			cout << "Average Reservoir pressure (psi) : ";
			cin >> averageReservoirPressure;
			if (cin.fail() || averageReservoirPressure < 0) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "Bad Input. Please enter a valid number." << endl;
				continue;
			}
			break;
		}

		while (true) {
			cout << "Bubble Point Pressure (psi) :";
			cin >> bubblePointPressure;
			if (cin.fail() || bubblePointPressure < 0) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "Bad Input. Please enter a valid number." << endl;
				continue;
			}
			break;
		}

		while (true) {
			cout << "Well Test Stabilized Flow rate (STB/Day) : ";
			cin >> stabilizedFlowRate;
			if (cin.fail() || stabilizedFlowRate < 0) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "Bad Input. Please enter a valid number." << endl;
				continue;
			}
			else if (stabilizedFlowRate == 0) {
				cout << "The Stabilized Q must be more than 0 as the well is flowing not S/I." << endl;
				continue;
			}
			break;
		}

		while (true) {
			cout << "Well Test Stabilized Bottomhole Flowing Pressure (psi) : ";
			cin >> stabilizedBhfp;
			if (cin.fail() || stabilizedBhfp < 0) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "Bad Input. Please enter a valid number." << endl;
				continue;
			}
			else if (stabilizedBhfp >= averageReservoirPressure) {
				cout << "The stabilized BHFP must be less than the Average Reservoir Pressure." << endl;
				continue;
			}
			break;
		}

		while (true) {
			cout << "Number of points for IPR calculation : ";
			cin >> pointsCount;
			if (cin.fail() || pointsCount < 0) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "Bad Input. Please enter a valid number." << endl;
				continue;
			}
			else if (pointsCount < 3) {
				cout << "You must enter at least 3 points in order to calculate a meaningful IPR." << endl;
				continue;
			}
			break;
		}
		// Reservoir Type

		string reservoir_type = reservoirType(averageReservoirPressure, bubblePointPressure);

		//PI , Qmax for Well PI method (Straight line method)

		float productivity_index = productivityIndex(stabilizedFlowRate, stabilizedBhfp, averageReservoirPressure);
		float PI_Qmax = productivity_index * averageReservoirPressure;

		//Points and spacing for IPR table Calculation.

		float unit = stepSize(pointsCount, averageReservoirPressure, 0);

		// Qmax for Vogel's IPR method

		float Vogel_Qmax = vogelQmax(stabilizedFlowRate, stabilizedBhfp, averageReservoirPressure);

		// PI calculation for Composite IPR method 

		float PI_composite = productivity_index;
		float Q_bubblePoint = Q_bubblepoint(PI_composite, averageReservoirPressure, bubblePointPressure);
		float Qmax_Composite = Composite_Qmax(Q_bubblePoint, PI_composite, bubblePointPressure);

		if (stabilizedBhfp < bubblePointPressure) {
			PI_composite = PI_compositebelow(stabilizedFlowRate, stabilizedBhfp, averageReservoirPressure, bubblePointPressure);
			Q_bubblePoint = Q_bubblepoint(PI_composite, averageReservoirPressure, bubblePointPressure);
			Qmax_Composite = Composite_Qmax(Q_bubblePoint, PI_composite, bubblePointPressure);
		}




		while (true) {
			cout << endl << "Main Menu : " << endl << endl;
			cout << "1. Reservoir Type & Recommended IPR method." << endl;
			cout << "2. Well PI IPR (Straight Line IPR)." << endl;
			cout << "3. Vogel's IPR." << endl;
			cout << "4. Composite IPR method for UnderSaturated Oil Reservoirs (PI + Vogel)." << endl;
			cout << "5. Recommended IPR method." << endl;
			cout << "6. Future IPR Prediction due to Reservoir Pressure Decline (Fetkovich Approximation Method)." << endl;
			cout << "7. Re-Input Data" << endl;
			cout << "8. Terminate the program." << endl << endl;

			cout << "Enter your Selection : ";
			cin >> selection;


			switch (selection) {
			case 1:
				horizontalLine();
				cout <<"The Reservoir type is " << reservoir_type << " ." << endl;
				if (averageReservoirPressure < bubblePointPressure) {
					cout << "The Recommended IPR method is Vogel's IPR method.\nHowever, you are free to choose any suitable method for your case.\nFor example in some very high water cut wells 'Well PI IPR method' (straight line method) is considered a good approximation even for saturated oil reservoirs." << endl;
				}
				else {
					cout << "The Recommended IPR method is Composite IPR method (PI + Vogel).\nHowever, you are free to choose any suitable method for your case.\nFor example in some very high water cut wells 'Well PI IPR method' (straight line method) is considered a good approximation even for saturated oil reservoirs." << endl;
				}
				horizontalLine();
				break;
			case 2:
				while (true) {
					cout << endl << "Well PI IPR (Straight Line IPR) :" << endl << endl;
					cout << "1. Return to Main Menu." << endl;
					cout << "2. Calculate Qmax / AOF." << endl;
					cout << "3. Calculate IPR." << endl;
					cout << "4. Calculate Q for a given Pwf." << endl;
					cout << "5. Calculate Pwf / Drawdown required to achieve deired Q." << endl;
					cout << "6. Calculate Productivity Index (PI)." << endl << endl;

					while (true) {
						cout << "Enter your Selection : ";
						cin >> selection;
						if (cin.fail()) {
							cin.clear();
							cin.ignore(numeric_limits<streamsize>::max(), '\n');
							cout << "Bad Input. Please enter a valid number." << endl;
							continue;
						}
						break;
					}
					switch (selection) {
					case 1:
						break;
					case 2:
						horizontalLine();
						cout << "Qmax (AOF) = " << PI_Qmax << " STB/day.";
						horizontalLine();
						break;
					case 3:
						horizontalLine();
						cout << "Pwf (psi) | Q (STB/day)" << endl;
						for (float i = 0; i <= averageReservoirPressure; i = i + unit) {
							cout << i << " psi" << " | " << wellPI(i, productivity_index, averageReservoirPressure) << " STB/Day." << endl;
						}
						horizontalLine();
						break;
					case 4:
						horizontalLine();
						while (true) {
							cout << "Pwf (psi): ";
							cin >> inputBhfp;
							if (cin.fail() || inputBhfp < 0) {
								cin.clear();
								cin.ignore(numeric_limits<streamsize>::max(), '\n');
								cout << "Bad Input. Please enter a valid number." << endl;
								continue;
							}
							else if (inputBhfp > averageReservoirPressure) {
								cout << "The BHFP can't be more than the Average Reservoir Pressure." << endl;
								continue;
							}
							break;
						}
						cout << "Q : " << wellPI(inputBhfp, productivity_index, averageReservoirPressure) << " STB/Day.";
						horizontalLine();
						break;
					case 5:
						horizontalLine();
						while (true) {
							cout << "Q (STB/Day): ";
							cin >> inputFlowRate;
							if (cin.fail() || inputFlowRate < 0) {
								cin.clear();
								cin.ignore(numeric_limits<streamsize>::max(), '\n');
								cout << "Bad Input. Please enter a valid number." << endl;
								continue;
							}
							else if (inputFlowRate > PI_Qmax) {
								cout << "The Flow rate can't be more than " << PI_Qmax << " psi beacuse that is AOF (Qmax)." << endl;
								continue;
							}
							break;
						}
						cout << "Pwf : " << BHFP_PI(averageReservoirPressure, inputFlowRate, productivity_index) << " psi." << endl;
						cout << "Required Drawdown : " << drawDown(averageReservoirPressure, BHFP_PI(averageReservoirPressure, inputFlowRate, productivity_index)) << " psi." << endl;
						horizontalLine();
						break;
					case 6:
						horizontalLine();
						cout << "PI : " << productivity_index << " STB/Day/psi.";
						horizontalLine();
						break;
					default:
						cout << "Please Enter your Selection From (1 : 6) !!";
						break;
					}
					if (selection == 1) {
						break;
					}
				}
				
				break;

			case 3:
				while (true) {
					cout << endl << "Vogel's IPR :" << endl << endl;
					cout << "1. Calculate Qmax / AOF." << endl;
					cout << "2. Calculate IPR." << endl;
					cout << "3. Calculate Q for a given Pwf." << endl;
					cout << "4. Calculate Pwf / Drawdown required to achieve deired Q." << endl;
					cout << "5. Return to Main Menu." << endl << endl;

					while (true) {
						cout << "Enter your Selection : ";
						cin >> selection;
						if (cin.fail()) {
							cin.clear();
							cin.ignore(numeric_limits<streamsize>::max(), '\n');
							cout << "Bad Input. Please enter a valid number." << endl;
							continue;
						}
						break;
					}
					switch (selection) {
					case 1:
						horizontalLine();
						cout << "Qmax (AOF) = " << Vogel_Qmax << " STB/day.";
						horizontalLine();
						break;
					case 2:
						horizontalLine();
						cout << "Pwf (psi) | Q (STB/day)" << endl;
						for (float i = 0; i <= averageReservoirPressure; i = i + unit) {
							cout << i << " psi" << " | " << vogelIPR(i , averageReservoirPressure , Vogel_Qmax) << " STB/Day." << endl;
						}
						horizontalLine();
						break;
					case 3:
						horizontalLine();
						while (true) {
							cout << "Pwf (psi): ";
							cin >> inputBhfp;
							if (cin.fail() || inputBhfp < 0) {
								cin.clear();
								cin.ignore(numeric_limits<streamsize>::max(), '\n');
								cout << "Bad Input. Please enter a valid number." << endl;
								continue;
							}
							else if (inputBhfp > averageReservoirPressure) {
								cout << "The BHFP can't be more than the Average Reservoir Pressure." << endl;
								continue;
							}
							break;
						}
						cout << "Q : " << vogelIPR(inputBhfp, averageReservoirPressure, Vogel_Qmax) << " STB/Day.";
						horizontalLine();
						break;
					case 4:
						horizontalLine();
						while (true) {
							cout << "Q (STB/Day): ";
							cin >> inputFlowRate;
							if (cin.fail() || inputFlowRate < 0) {
								cin.clear();
								cin.ignore(numeric_limits<streamsize>::max(), '\n');
								cout << "Bad Input. Please enter a valid number." << endl;
								continue;
							}
							else if (inputFlowRate > PI_Qmax) {
								cout << "The Flow rate can't be more than " << PI_Qmax << " psi beacuse that is AOF (Qmax)." << endl;
								continue;
							}
							break;
						}
						cout << "Pwf : " << BHFP_Vogel(averageReservoirPressure , inputFlowRate , Vogel_Qmax) << " psi." << endl;
						cout << "Required Drawdown : " << drawDown(averageReservoirPressure, BHFP_Vogel(averageReservoirPressure, inputFlowRate, Vogel_Qmax)) << " psi." << endl;
						horizontalLine();
						break;
					case 5:
						break;
					default:
						cout << "Please Enter your Selection From (1 : 6) !!";
						break;
					}
					if (selection == 5) {
						break;
					}
				}

				break;
			case 4 :
				if (averageReservoirPressure <= bubblePointPressure) {
					horizontalLine();
					cout << "The Reservoir is Saturated Oil Reservoir.\nYou can choose Well PI IPR (straight line Method) or Vogel's Method (Recommended & most accurate for saturated oil Reservoirs)." << endl;
					horizontalLine();
					break;
				}
				horizontalLine();
				cout << "The Reservoir is Undersaturated Oil Reservoir.\nSo We can use the Composite IPR method (PI + Vogel) for more accurate results than using each method solely." << endl;
				horizontalLine();
				while (true) {
					cout << endl << "Composite IPR method for UnderSaturated Oil Reservoirs (PI + Vogel)." << endl << endl;
					cout << "1. Return to Main Menu." << endl;
					cout << "2. Calculate Qmax / AOF." << endl;
					cout << "3. Calculate IPR." << endl;
					cout << "4. Calculate Q for a given Pwf." << endl;
					cout << "5. Calculate Pwf / Drawdown required to achieve deired Q." << endl;
					cout << "6. Calculate Q @ Bubble Point Pressure." << endl;
					cout << "7. Calculate PI above Bubble Point Pressure." << endl << endl;
					

					while (true) {
						cout << "Enter your Selection : ";
						cin >> selection;
						if (cin.fail()) {
							cin.clear();
							cin.ignore(numeric_limits<streamsize>::max(), '\n');
							cout << "Bad Input. Please enter a valid number." << endl;
							continue;
						}
						break;
					}
					switch (selection) {
					case 1:
						break;
					case 2:
						horizontalLine();
						cout << "Qmax (AOF) = " << Qmax_Composite << " STB/day.";
						horizontalLine();
						break;
					case 3:
						horizontalLine();
						cout << "Pwf (psi) | Q (STB/day)" << endl;
						for (float i = 0; i <= averageReservoirPressure; i = i + unit) {
							if (i < bubblePointPressure) {
								cout << i << " psi" << " | " << Vogel_Below(Q_bubblePoint , PI_composite , bubblePointPressure , i) << " STB/Day." << endl;
							}
							else {
								cout << i << " psi" << " | " << wellPI(i, PI_composite, averageReservoirPressure) << " STB/Day." << endl;
							}
						}
						horizontalLine();
						break;
					case 4:
						horizontalLine();
						while (true) {
							cout << "Pwf (psi): ";
							cin >> inputBhfp;
							if (cin.fail() || inputBhfp < 0) {
								cin.clear();
								cin.ignore(numeric_limits<streamsize>::max(), '\n');
								cout << "Bad Input. Please enter a valid number." << endl;
								continue;
							}
							else if (inputBhfp > averageReservoirPressure) {
								cout << "The BHFP can't be more than the Average Reservoir Pressure." << endl;
								continue;
							}
							break;
						}
						horizontalLine();
						if (inputBhfp < bubblePointPressure) {
							cout << "Q : " << Vogel_Below(Q_bubblePoint, PI_composite, bubblePointPressure, inputBhfp) << " STB/Day.";
						}
						else {
							cout << "Q : " << wellPI(inputBhfp, PI_composite, averageReservoirPressure) << " STB/Day.";
						}
						horizontalLine();
						break;
					case 5:
						horizontalLine();
						while (true) {
							cout << "Q (STB/Day): ";
							cin >> inputFlowRate;
							if (cin.fail() || inputFlowRate < 0) {
								cin.clear();
								cin.ignore(numeric_limits<streamsize>::max(), '\n');
								cout << "Bad Input. Please enter a valid number." << endl;
								continue;
							}
							else if (inputFlowRate > Qmax_Composite) {
								cout << "The Flow rate can't be more than " << Qmax_Composite << " psi beacuse that is AOF (Qmax)." << endl;
								continue;
							}
							break;
						}
						horizontalLine();
						if (inputFlowRate > Q_bubblePoint) {
							cout << "Pwf : " << BHFP_VogelBelow( Q_bubblePoint, PI_composite , bubblePointPressure,inputFlowRate) << " psi." << endl;
							cout << "Required Drawdown : " << drawDown(averageReservoirPressure, BHFP_VogelBelow(Q_bubblePoint, PI_composite, bubblePointPressure, inputFlowRate)) << " psi." << endl;
						}
						else {
							cout << "Pwf : " << BHFP_PI(averageReservoirPressure, inputFlowRate, PI_composite) << " psi." << endl;
							cout << "Required Drawdown : " << drawDown(averageReservoirPressure, BHFP_PI(averageReservoirPressure, inputFlowRate, PI_composite)) << " psi." << endl;
						}
						horizontalLine();
						break;
					case 6:
						horizontalLine();
						cout << "Q @ (Pb = " << bubblePointPressure <<" psi ) = " << Q_bubblePoint << " STB/Day.";
						horizontalLine();
						break;
					case 7:
						horizontalLine();
						cout << "PI above (Pb = " << bubblePointPressure << " psi ) = " << PI_composite << " STB/Day/psi.";
						horizontalLine();
						break;
					default:
						cout << "Please Enter your Selection From (1 : 5) !!";
						break;
					}
					if (selection == 1) {
						break;
					}
				}
				break;
			case 5:
				if (averageReservoirPressure < bubblePointPressure) {
					while (true) {
						cout << endl << "Vogel's IPR :" << endl << endl;
						cout << "1. Calculate Qmax / AOF." << endl;
						cout << "2. Calculate IPR." << endl;
						cout << "3. Calculate Q for a given Pwf." << endl;
						cout << "4. Calculate Pwf / Drawdown required to achieve deired Q." << endl;
						cout << "5. Return to Main Menu." << endl << endl;

						while (true) {
							cout << "Enter your Selection : ";
							cin >> selection;
							if (cin.fail()) {
								cin.clear();
								cin.ignore(numeric_limits<streamsize>::max(), '\n');
								cout << "Bad Input. Please enter a valid number." << endl;
								continue;
							}
							break;
						}
						switch (selection) {
						case 1:
							horizontalLine();
							cout << "Qmax (AOF) = " << Vogel_Qmax << " STB/day.";
							horizontalLine();
							break;
						case 2:
							horizontalLine();
							cout << "Pwf (psi) | Q (STB/day)" << endl;
							for (float i = 0; i <= averageReservoirPressure; i = i + unit) {
								cout << i << " psi" << " | " << vogelIPR(i, averageReservoirPressure, Vogel_Qmax) << " STB/Day." << endl;
							}
							horizontalLine();
							break;
						case 3:
							horizontalLine();
							while (true) {
								cout << "Pwf (psi): ";
								cin >> inputBhfp;
								if (cin.fail() || inputBhfp < 0) {
									cin.clear();
									cin.ignore(numeric_limits<streamsize>::max(), '\n');
									cout << "Bad Input. Please enter a valid number." << endl;
									continue;
								}
								else if (inputBhfp > averageReservoirPressure) {
									cout << "The BHFP can't be more than the Average Reservoir Pressure." << endl;
									continue;
								}
								break;
							}
							cout << "Q : " << vogelIPR(inputBhfp, averageReservoirPressure, Vogel_Qmax) << " STB/Day.";
							horizontalLine();
							break;
						case 4:
							horizontalLine();
							while (true) {
								cout << "Q (STB/Day): ";
								cin >> inputFlowRate;
								if (cin.fail() || inputFlowRate < 0) {
									cin.clear();
									cin.ignore(numeric_limits<streamsize>::max(), '\n');
									cout << "Bad Input. Please enter a valid number." << endl;
									continue;
								}
								else if (inputFlowRate > PI_Qmax) {
									cout << "The Flow rate can't be more than " << PI_Qmax << " psi beacuse that is AOF (Qmax)." << endl;
									continue;
								}
								break;
							}
							cout << "Pwf : " << BHFP_Vogel(averageReservoirPressure, inputFlowRate, Vogel_Qmax) << " psi." << endl;
							cout << "Required Drawdown : " << drawDown(averageReservoirPressure, BHFP_Vogel(averageReservoirPressure, inputFlowRate, Vogel_Qmax)) << " psi." << endl;
							horizontalLine();
							break;
						case 5:
							break;
						default:
							cout << "Please Enter your Selection From (1 : 6) !!";
							break;
						}
						if (selection == 5) {
							break;
						}
					}
				}
				else {
					while (true) {
						cout << endl << "Composite IPR method for UnderSaturated Oil Reservoirs (PI + Vogel)." << endl << endl;
						cout << "1. Return to Main Menu." << endl;
						cout << "2. Calculate Qmax / AOF." << endl;
						cout << "3. Calculate IPR." << endl;
						cout << "4. Calculate Q for a given Pwf." << endl;
						cout << "5. Calculate Pwf / Drawdown required to achieve deired Q." << endl;
						cout << "6. Calculate Q @ Bubble Point Pressure." << endl;
						cout << "7. Calculate PI above Bubble Point Pressure." << endl << endl;


						while (true) {
							cout << "Enter your Selection : ";
							cin >> selection;
							if (cin.fail()) {
								cin.clear();
								cin.ignore(numeric_limits<streamsize>::max(), '\n');
								cout << "Bad Input. Please enter a valid number." << endl;
								continue;
							}
							break;
						}
						switch (selection) {
						case 1:
							break;
						case 2:
							horizontalLine();
							cout << "Qmax (AOF) = " << Qmax_Composite << " STB/day.";
							horizontalLine();
							break;
						case 3:
							horizontalLine();
							cout << "Pwf (psi) | Q (STB/day)" << endl;
							for (float i = 0; i <= averageReservoirPressure; i = i + unit) {
								if (i < bubblePointPressure) {
									cout << i << " psi" << " | " << Vogel_Below(Q_bubblePoint, PI_composite, bubblePointPressure, i) << " STB/Day." << endl;
								}
								else {
									cout << i << " psi" << " | " << wellPI(i, PI_composite, averageReservoirPressure) << " STB/Day." << endl;
								}
							}
							horizontalLine();
							break;
						case 4:
							horizontalLine();
							while (true) {
								cout << "Pwf (psi): ";
								cin >> inputBhfp;
								if (cin.fail() || inputBhfp < 0) {
									cin.clear();
									cin.ignore(numeric_limits<streamsize>::max(), '\n');
									cout << "Bad Input. Please enter a valid number." << endl;
									continue;
								}
								else if (inputBhfp > averageReservoirPressure) {
									cout << "The BHFP can't be more than the Average Reservoir Pressure." << endl;
									continue;
								}
								break;
							}
							horizontalLine();
							if (inputBhfp < bubblePointPressure) {
								cout << "Q : " << Vogel_Below(Q_bubblePoint, PI_composite, bubblePointPressure, inputBhfp) << " STB/Day.";
							}
							else {
								cout << "Q : " << wellPI(inputBhfp, PI_composite, averageReservoirPressure) << " STB/Day.";
							}
							horizontalLine();
							break;
						case 5:
							horizontalLine();
							while (true) {
								cout << "Q (STB/Day): ";
								cin >> inputFlowRate;
								if (cin.fail() || inputFlowRate < 0) {
									cin.clear();
									cin.ignore(numeric_limits<streamsize>::max(), '\n');
									cout << "Bad Input. Please enter a valid number." << endl;
									continue;
								}
								else if (inputFlowRate > Qmax_Composite) {
									cout << "The Flow rate can't be more than " << Qmax_Composite << " psi beacuse that is AOF (Qmax)." << endl;
									continue;
								}
								break;
							}
							horizontalLine();
							if (inputFlowRate > Q_bubblePoint) {
								cout << "Pwf : " << BHFP_VogelBelow(Q_bubblePoint, PI_composite, bubblePointPressure, inputFlowRate) << " psi." << endl;
								cout << "Required Drawdown : " << drawDown(averageReservoirPressure, BHFP_VogelBelow(Q_bubblePoint, PI_composite, bubblePointPressure, inputFlowRate)) << " psi." << endl;
							}
							else {
								cout << "Pwf : " << BHFP_PI(averageReservoirPressure, inputFlowRate, PI_composite) << " psi." << endl;
								cout << "Required Drawdown : " << drawDown(averageReservoirPressure, BHFP_PI(averageReservoirPressure, inputFlowRate, PI_composite)) << " psi." << endl;
							}
							horizontalLine();
							break;
						case 6:
							horizontalLine();
							cout << "Q @ (Pb = " << bubblePointPressure << " psi ) = " << Q_bubblePoint << " STB/Day.";
							horizontalLine();
							break;
						case 7:
							horizontalLine();
							cout << "PI above (Pb = " << bubblePointPressure << " psi ) = " << PI_composite << " STB/Day/psi.";
							horizontalLine();
							break;
						default:
							cout << "Please Enter your Selection From (1 : 5) !!";
							break;
						}
						if (selection == 1) {
							break;
						}
					}
				}
				break;
			case 6:
				while (true) {
					cout << "Enter Future (Predicted) Reservoir Pressure (psi): ";
					cin >> futureReservoirPressure;
					if (cin.fail() || futureReservoirPressure < 0) {
						cin.clear();
						cin.ignore(numeric_limits<streamsize>::max(), '\n');
						cout << "Bad Input. Please enter a valid number." << endl;
						continue;
					}
					else if (futureReservoirPressure > averageReservoirPressure) {
						cout << "Due to Decline in Reservoir Pressure , The Future Reservoir Pressure should be less than the Present one." << endl;
						continue;
					}
					break;
				}

				while (true) {
					cout << endl << "Future IPR Prediction (Fetkovich Approximation Method) :" << endl << endl;
					cout << "1. Calculate Present Qmax / AOF." << endl;
					cout << "2. Calculate Future Qmax / AOF." << endl;
					cout << "3. Calculate Present IPR." << endl;
					cout << "4. Calculate Future IPR." << endl;
					cout << "5. Compare Present IPR W/ Future IPR." << endl;
					cout << "6. Return to Main Menu." << endl << endl;

					while (true) {
						cout << "Enter your Selection : ";
						cin >> selection;
						if (cin.fail()) {
							cin.clear();
							cin.ignore(numeric_limits<streamsize>::max(), '\n');
							cout << "Bad Input. Please enter a valid number." << endl;
							continue;
						}
						break;
					}
					switch (selection) {
					case 1:
						horizontalLine();
						cout << "Present Qmax (AOF) = " << Vogel_Qmax << " STB/Day.";
						horizontalLine();
						break;
					case 2:
						horizontalLine();
						cout << "Future Qmax (AOF) = " << QmaxPredicted(Vogel_Qmax , averageReservoirPressure ,futureReservoirPressure) << " STB/Day.";
						horizontalLine();
						break;
					case 3:
						horizontalLine();
						cout << "Present IPR" << endl << endl;
						cout << "Pwf (psi) | Q (STB/day)" << endl;
						for (float i = 0; i <= averageReservoirPressure; i = i + unit) {
							cout << i << " psi" << " | " << vogelIPR(i, averageReservoirPressure, Vogel_Qmax) << " STB/Day." << endl;
						}
						horizontalLine();
						break;
					case 4:
						horizontalLine();
						cout << "Future IPR" << endl << endl;
						cout << "Pwf (psi) | Q (STB/day)" << endl;
						for (float i = 0; i <= futureReservoirPressure; i = i + (futureReservoirPressure / pointsCount)) {
							cout << i << " psi" << " | " << vogelIPR(i, futureReservoirPressure, QmaxPredicted(Vogel_Qmax, averageReservoirPressure, futureReservoirPressure)) << " STB/Day." << endl;
						}
						horizontalLine();
						break;
					case 5:
						horizontalLine();
						cout << "Comparison Between Present IPR & Future IPR" << endl << endl;
						cout << "Pwf (psi) | Present Q (STB/day) | Future Q (STB/Day) | Difference (Present - Future) (STB/Day)" << endl;
						for (float i = 0; i <= averageReservoirPressure; i = i + unit) {
							if (i > futureReservoirPressure) {
								cout << i << " psi" << "    | " << vogelIPR(i, averageReservoirPressure, Vogel_Qmax) << " STB/Day." << "    | " << "--- STB/Day." << "    | " << vogelIPR(i, averageReservoirPressure, Vogel_Qmax) << "STB/Day." << endl;
							}
							else {
								cout << i << " psi" << "    | " << vogelIPR(i, averageReservoirPressure, Vogel_Qmax) << " STB/Day." << "    | " << vogelIPR(i, futureReservoirPressure, QmaxPredicted(Vogel_Qmax, averageReservoirPressure, futureReservoirPressure)) << " STB/Day." << "    | " << vogelIPR(i, averageReservoirPressure, Vogel_Qmax) - vogelIPR(i, futureReservoirPressure, QmaxPredicted(Vogel_Qmax, averageReservoirPressure, futureReservoirPressure)) << "STB/Day." << endl;
							}
						}
						horizontalLine();
						break;
					case 6:
						break;
					default:
						cout << "Please Enter your Selection From (1 : 6) !!";
						break;
					}
					if (selection == 6) {
						break;
					}
				}
				break;
			case 7:
				break;
			case 8:
				return 0;
			default:
				cout << "Please Enter your Selection From (1 - 8) !!";
				break;
			}
			if (selection == 7) {
				break;
			}



			
		}
		
	}
	
}

