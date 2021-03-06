// Packet Reader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

using namespace std;

typedef struct {
	float ECalSlope;
	float EcalIntercept;
	int TriggerThreshold;
	int IntegrationBaseline;
	int IntegrationShort;
	int IntegrationLong;
	int IntegrationFull;
	int HighVoltageValue[4];
	float ScaleFactorEnergy_1_1;
	float ScaleFactorEnergy_1_2;
	float ScaleFactorEnergy_2_1;
	float ScaleFactorEnergy_2_2;
	float ScaleFactorEnergy_3_1;
	float ScaleFactorEnergy_3_2;
	float ScaleFactorEnergy_4_1;
	float ScaleFactorEnergy_4_2;
	float ScaleFactorPSD_1_1;
	float ScaleFactorPSD_1_2;
	float ScaleFactorPSD_2_1;
	float ScaleFactorPSD_2_2;
	float ScaleFactorPSD_3_1;
	float ScaleFactorPSD_3_2;
	float ScaleFactorPSD_4_1;
	float ScaleFactorPSD_4_2;
	float OffsetEnergy_1_1;
	float OffsetEnergy_1_2;
	float OffsetEnergy_2_1;
	float OffsetEnergy_2_2;
	float OffsetEnergy_3_1;
	float OffsetEnergy_3_2;
	float OffsetEnergy_4_1;
	float OffsetEnergy_4_2;
	float OffsetPSD_1_1;
	float OffsetPSD_1_2;
	float OffsetPSD_2_1;
	float OffsetPSD_2_2;
	float OffsetPSD_3_1;
	float OffsetPSD_3_2;
	float OffsetPSD_4_1;
	float OffsetPSD_4_2;
} CONFIG_STRUCT_TYPE;

int main()
{
	//Variables
	int i = 0;
	int jj = 0;
	int apid = 0;
	int file_length = 0;
	int packet_length = 0;
	int num_packets = 0;
	int apid_packet_vals[200] = {};
	int apid_packet_true[] = { 0x44,		0x00,		0x00,		0x00,		0x11,
		0x11,		0x11,		0x11,		0x00,		0x00,		0x11,		0x11,
		0x00,		0x00,		0x11,		0x11,		0x00,		0x11,		0x11,
		0x00,		0x00,		0x00,		0x00,		0x00,		0x00,		0x00,
		0x00,		0x00,		0x00,		0x00,		0x00,		0x00,		0x11,
		0x11,		0x11 };
	unsigned int test_value = 0;
	unsigned int sync_marker = 0x352EF853;
	string quit = "";
	string infile = "L2_MACRO_OUT.bin";

	cout << "This utility will read the Run Macro output file and the configuration file from SD 0." << endl;
	cout << "There will be 4 files produced in this folder which describe the test run." << endl;
	cout << endl << "Begin reading processing the Run Macro output file..." << endl;

	/* I want to read in the filename that the user would like to convert from .bin -> .txt
	* Use getline to get user input and append .bin to the end of it
	* then place that string into the fstream function below.
	*/
	//the code below this was when this took user input, it is now automated, but relies on specific files being present
/*	cout << "Before entering files to parse, place them in the folder with this executable." << endl;
	//have the user enter a file name to use
	cout << "Enter a binary file name (without .bin extension) to parse: " << endl;
	getline(cin, infile);
	infile += ".bin"; */
	//test that the file exists before trying to open it:
	if (!experimental::filesystem::exists(infile))
	{
		cout << "The L2 Run Macro output file does not exist in this folder." << endl;
		cout << "Press enter to quit..." << endl;
		getline(cin, quit);
		return 100;
	}

	//create the pointer for us to declare a buffer to save the file to
	unsigned char * file_buff = NULL;
	//declare the streams for input and output
	ifstream ifs;
	ofstream ofs;
	ofstream ofs_vals;
	ofstream ofs_noSOH_vals;
	//open both streams for use
	ifs.open(infile, ios_base::in | ios_base::binary);
	ofs.open("L2_packets_read.txt", ios::app);
	ofs_vals.open("L2_packets_raw_vals.txt", ios::app);
	ofs_noSOH_vals.open("L2_packets_noSOH_vals.txt", ios::app);
	//indicate that the output file is a new read and state the file being read
	ofs << "******************************************" << endl;
	ofs << "Reading packets from the file: " + infile << endl;
	ofs_vals << "******************************************" << endl;
	ofs_vals << "Reading packets from the file: " + infile << endl;
	ofs_noSOH_vals << "******************************************" << endl;
	ofs_noSOH_vals << "Reading packets from the file: " + infile << endl;

	if (ifs.is_open())
	{
		//just use the fact that this is a short file for now
		// we can modify this later
		//get the length of the file:
		ifs.seekg(0, ifs.end);
		file_length = (int)ifs.tellg();
		ifs.seekg(0, ifs.beg);
		//declare a buffer the size of the file
		file_buff = new unsigned char[file_length];
		//read in the file to our buffer
		ifs.read(reinterpret_cast<char *>(file_buff), file_length);
		//We have the entire file in memory
		//Parse file_buff byte-by-byte to find successive instances of the sync marker (0x35,2E,F8,53)
		while (file_length > 0)
		{
			//create an int from the next 4 bytes
			test_value = (file_buff[i + 0] << 24) | (file_buff[i + 1] << 16) | (file_buff[i + 2] << 8) | (file_buff[i + 3]);
			if (test_value == sync_marker)
			{
				if (file_buff[i + 4] == 0x0A)
				{
					if (file_buff[i + 6] == 0xC0)
					{
						if (file_buff[i + 7] == 0x01)
						{
							//if we pass all of these requirements on the packet (there are so many because the packets
							// are not constrained...we only have 4 and not full functionality
							//Then we know that this is a real packet
							//Print out the APID and the packet size
							apid = file_buff[i + 5];
							if (apid == 0)
								ofs_vals << "0x" << std::hex << apid << "0" << endl;	//write an extra 0 for 0x00
							else
								ofs_vals << "0x" << std::hex << apid << endl;
							ofs << "The APID is: 0x" << std::hex << apid << endl;
							packet_length = ((file_buff[i + 8] << 8) | file_buff[i + 9]);
							ofs << "The packet length is: " << std::dec << packet_length << endl;
							//write to another separate file the non-SOH packet APIDs
							if (apid != 0x22)
							{
								apid_packet_vals[jj] = apid;
								jj++;
								if(apid == 0)
									ofs_noSOH_vals << "0x" << std::hex << apid << "0" << endl;	//write an extra 0 for 0x00
								else
									ofs_noSOH_vals << "0x" << std::hex << apid << endl;
							}

							//add the packet length + 4 (sync marker bytes) to the iterator, i
							i += packet_length + 4;
							file_length -= packet_length + 4;
							num_packets++;
						}
						else
						{
							i++;
							file_length -= 1;
						}
					}
					else
					{
						i++;
						file_length -= 1;
					}
				}
				else
				{
					i++;
					file_length -= 1;
				}
			}
			else
			{
				i++;
				file_length -= 1;
			}

			//test to see if we are at the end of the file
			//if (file_length <= 0)
				//break;
		}
	}

	cout << "Read " << num_packets << " number of packets from the file." << endl << endl;

	//compare the true and read in values
	int myval = (sizeof(apid_packet_true) / sizeof(int));
	if (myval == jj)
		cout << "Number of packets is equal." << endl;
	else
		cout << "Number of packets is not equal." << endl;

	cout << "APIDs saved from packets is: " << jj << " compared to expected: " << (sizeof(apid_packet_true) / sizeof(int)) << endl;
	
	jj = 0;	//reset
	cout << "Checking APIDs against expected values: " << endl;
	while (1)
	{
		if (apid_packet_vals[jj] == apid_packet_true[jj])
		{
			cout << jj << "...";
			jj++;
			if (jj == (sizeof(apid_packet_true)/sizeof(int)))
			{
				cout << "Pass" << endl;
				break;
			}
		}
		else
		{
			cout << "Error on APID " << jj << endl;
			break;
		}
	}

	//close out of variables properly
	ifs.close();
	ofs.close();
	ofs_vals.close();
	ofs_noSOH_vals.close();
	delete file_buff;

	/**** This part of the code will take the configuration file read it and compare it to what it should be after a test run ****/
	//the following structure is what the config buff should be after a test run
	CONFIG_STRUCT_TYPE cfgBuffRunVals = { 3.0, 9.0, 12, -51, 87, 399, 5999, {220, 220, 220, 220}, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.1, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.3, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.6, 0.0, 0.0, 0.0, 0.0, 0.0 };

	string cfgfile = "MNSCONF.bin";
	if (!experimental::filesystem::exists(cfgfile))
	{
		cout << "The MNSCONF.bin SD card file does not exist in this folder." << endl;
		cout << "Press enter to quit..." << endl;
		getline(cin, quit);
		return 100;
	}
	//declare the streams for input and output
	ifstream ifs_cfg;
	ofstream ofs_cfg;
	//open both streams for use
	ifs_cfg.open(cfgfile, ios_base::in | ios_base::binary);
	ofs_cfg.open("L2_config_analyze.txt", ios::app);
	//indicate that the output file is a new read and state the file being read
	ofs_cfg << "******************************************" << endl;
	ofs_cfg << "Analyzing the Configuration File from the file: " + cfgfile << endl;

	int iter = 0;
	int missed = 0;
	unsigned char temp[sizeof(float)] = "";
	unsigned char holder[sizeof(float)] = "";
	CONFIG_STRUCT_TYPE cfgBuff = {};

	cout << endl << "Begin comparing the configuration file: " + infile << endl << endl;
	file_length = 0;
	//unsigned char * cfg_file_buff = NULL;
	if (ifs_cfg.is_open())
	{
		ifs_cfg.seekg(0, ifs_cfg.end);
		file_length = (int)ifs_cfg.tellg();
		ifs_cfg.seekg(0, ifs_cfg.beg);

		//have to read in properly
		//this is reading in the floats in BIG ENDIAN
		while (file_length > 0)
		{
			ifs_cfg.read(reinterpret_cast<char *>(&temp), sizeof(float));
			holder[0] = temp[3];
			holder[1] = temp[2];
			holder[2] = temp[1];
			holder[3] = temp[0];

			switch (iter)
			{
			case 0:
				cfgBuff.ECalSlope = reinterpret_cast<float&>(temp);
				if (cfgBuff.ECalSlope == cfgBuffRunVals.ECalSlope)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ECAL Slope Fail" << endl;
				missed++;
				}
				break;			
			case 1:
				cfgBuff.EcalIntercept = reinterpret_cast<float&>(temp);
				if (cfgBuff.EcalIntercept == cfgBuffRunVals.EcalIntercept)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ECAL Intercept Fail" << endl;
					missed++;
				}
				break;
			case 2:
				cfgBuff.TriggerThreshold = reinterpret_cast<int&>(temp);
				if (cfgBuff.TriggerThreshold == cfgBuffRunVals.TriggerThreshold)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "TriggerThreshold Fail" << endl;
					missed++;
				}
				break;
			case 3:
				cfgBuff.IntegrationBaseline = reinterpret_cast<int&>(temp);
				if (cfgBuff.IntegrationBaseline == cfgBuffRunVals.IntegrationBaseline)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "IntegrationBaseline Fail" << endl;
					missed++;
				}
				break;
			case 4:
				cfgBuff.IntegrationShort = reinterpret_cast<int&>(temp);
				if (cfgBuff.IntegrationShort == cfgBuffRunVals.IntegrationShort)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "IntegrationShort Fail" << endl;
					missed++;
				}
				break;
			case 5:
				cfgBuff.IntegrationLong = reinterpret_cast<int&>(temp);
				if (cfgBuff.IntegrationLong == cfgBuffRunVals.IntegrationLong)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "IntegrationLong Fail" << endl;
					missed++;
				}
				break;
			case 6:
				cfgBuff.IntegrationFull = reinterpret_cast<int&>(temp);
				if (cfgBuff.IntegrationFull == cfgBuffRunVals.IntegrationFull)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "IntegrationFull Fail" << endl;
					missed++;
				}
				break;
			case 7:
				cfgBuff.HighVoltageValue[0] = reinterpret_cast<int&>(temp);
				if (cfgBuff.HighVoltageValue[0] == cfgBuffRunVals.HighVoltageValue[0])
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "HighVoltageValue[0] Fail" << endl;
					missed++;
				}
				break;
			case 8:
				cfgBuff.HighVoltageValue[1] = reinterpret_cast<int&>(temp);
				if (cfgBuff.HighVoltageValue[1] == cfgBuffRunVals.HighVoltageValue[1])
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "HighVoltageValue[1] Fail" << endl;
					missed++;
				}
				break;
			case 9:
				cfgBuff.HighVoltageValue[2] = reinterpret_cast<int&>(temp);
				if (cfgBuff.HighVoltageValue[2] == cfgBuffRunVals.HighVoltageValue[2])
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "HighVoltageValue[2] Fail" << endl;
					missed++;
				}
				break;
			case 10:
				cfgBuff.HighVoltageValue[3] = reinterpret_cast<int&>(temp);
				if (cfgBuff.HighVoltageValue[3] == cfgBuffRunVals.HighVoltageValue[3])
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "HighVoltageValue[3] Fail" << endl;
					missed++;
				}
				break;
			case 11:
				cfgBuff.ScaleFactorEnergy_1_1 = reinterpret_cast<float&>(temp);
				if (cfgBuff.ScaleFactorEnergy_1_1 == cfgBuffRunVals.ScaleFactorEnergy_1_1)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ScaleFactorEnergy_1_1 Fail" << endl;
					missed++;
				}
				break;
			case 12:
				cfgBuff.ScaleFactorEnergy_1_2 = reinterpret_cast<float&>(temp);
				if (cfgBuff.ScaleFactorEnergy_1_2 == cfgBuffRunVals.ScaleFactorEnergy_1_2)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ScaleFactorEnergy_1_2 Fail" << endl;
					missed++;
				}
				break;
			case 13:
				cfgBuff.ScaleFactorEnergy_2_1 = reinterpret_cast<float&>(temp);
				if (cfgBuff.ScaleFactorEnergy_2_1 == cfgBuffRunVals.ScaleFactorEnergy_2_1)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ScaleFactorEnergy_2_1 Fail" << endl;
					missed++;
				}
				break;
			case 14:
				cfgBuff.ScaleFactorEnergy_2_2 = reinterpret_cast<float&>(temp);
				if (cfgBuff.ScaleFactorEnergy_2_2 == cfgBuffRunVals.ScaleFactorEnergy_2_2)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ScaleFactorEnergy_2_2 Fail" << endl;
					missed++;
				}
				break;
			case 15:
				cfgBuff.ScaleFactorEnergy_3_1 = reinterpret_cast<float&>(temp);
				if (cfgBuff.ScaleFactorEnergy_3_1 == cfgBuffRunVals.ScaleFactorEnergy_3_1)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ScaleFactorEnergy_3_1 Fail" << endl;
					missed++;
				}
				break;
			case 16:
				cfgBuff.ScaleFactorEnergy_3_2 = reinterpret_cast<float&>(temp);
				if (cfgBuff.ScaleFactorEnergy_3_2 == cfgBuffRunVals.ScaleFactorEnergy_3_2)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ScaleFactorEnergy_3_2 Fail" << endl;
					missed++;
				}
				break;
			case 17:
				cfgBuff.ScaleFactorEnergy_4_1 = reinterpret_cast<float&>(temp);
				if (cfgBuff.ScaleFactorEnergy_4_1 == cfgBuffRunVals.ScaleFactorEnergy_4_1)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ScaleFactorEnergy_4_1 Fail" << endl;
					missed++;
				}
				break;
			case 18:
				cfgBuff.ScaleFactorEnergy_4_2 = reinterpret_cast<float&>(temp);
				if (cfgBuff.ScaleFactorEnergy_4_2 == cfgBuffRunVals.ScaleFactorEnergy_4_2)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ScaleFactorEnergy_4_2 Fail" << endl;
					missed++;
				}
				break;
			case 19:
				cfgBuff.ScaleFactorPSD_1_1 = reinterpret_cast<float&>(temp);
				if (cfgBuff.ScaleFactorPSD_1_1 == cfgBuffRunVals.ScaleFactorPSD_1_1)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ScaleFactorPSD_1_1 Fail" << endl;
					missed++;
				}
				break;
			case 20:
				cfgBuff.ScaleFactorPSD_1_2 = reinterpret_cast<float&>(temp);
				if (cfgBuff.ScaleFactorPSD_1_2 == cfgBuffRunVals.ScaleFactorPSD_1_2)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ScaleFactorPSD_1_2 Fail" << endl;
					missed++;
				}
				break;
			case 21:
				cfgBuff.ScaleFactorPSD_2_1 = reinterpret_cast<float&>(temp);
				if (cfgBuff.ScaleFactorPSD_2_1 == cfgBuffRunVals.ScaleFactorPSD_2_1)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ScaleFactorPSD_2_1 Fail" << endl;
					missed++;
				}
				break;
			case 22:
				cfgBuff.ScaleFactorPSD_2_2 = reinterpret_cast<float&>(temp);
				if (cfgBuff.ScaleFactorPSD_2_2 == cfgBuffRunVals.ScaleFactorPSD_2_2)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ScaleFactorPSD_2_2 Fail" << endl;
					missed++;
				}
				break;
			case 23:
				cfgBuff.ScaleFactorPSD_3_1 = reinterpret_cast<float&>(temp);
				if (cfgBuff.ScaleFactorPSD_3_1 == cfgBuffRunVals.ScaleFactorPSD_3_1)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ScaleFactorPSD_3_1 Fail" << endl;
					missed++;
				}
				break;
			case 24:
				cfgBuff.ScaleFactorPSD_3_2 = reinterpret_cast<float&>(temp);
				if (cfgBuff.ScaleFactorPSD_3_2 == cfgBuffRunVals.ScaleFactorPSD_3_2)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ScaleFactorPSD_3_2 Fail" << endl;
					missed++;
				}
				break;
			case 25:
				cfgBuff.ScaleFactorPSD_4_1 = reinterpret_cast<float&>(temp);
				if (cfgBuff.ScaleFactorPSD_4_1 == cfgBuffRunVals.ScaleFactorPSD_4_1)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ScaleFactorPSD_4_1 Fail" << endl;
					missed++;
				}
				break;
			case 26:
				cfgBuff.ScaleFactorPSD_4_2 = reinterpret_cast<float&>(temp);
				if (cfgBuff.ScaleFactorPSD_4_2 == cfgBuffRunVals.ScaleFactorPSD_4_2)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "ScaleFactorPSD_4_2 Fail" << endl;
					missed++;
				}
				break;
			case 27:
				cfgBuff.OffsetEnergy_1_1 = reinterpret_cast<float&>(temp);
				if (cfgBuff.OffsetEnergy_1_1 == cfgBuffRunVals.OffsetEnergy_1_1)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "OffsetEnergy_1_1 Fail" << endl;
					missed++;
				}
				break;
			case 28:
				cfgBuff.OffsetEnergy_1_2 = reinterpret_cast<float&>(temp);
				if (cfgBuff.OffsetEnergy_1_2 == cfgBuffRunVals.OffsetEnergy_1_2)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "OffsetEnergy_1_2 Fail" << endl;
					missed++;
				}
				break;
			case 29:
				cfgBuff.OffsetEnergy_2_1 = reinterpret_cast<float&>(temp);
				if (cfgBuff.OffsetEnergy_2_1 == cfgBuffRunVals.OffsetEnergy_2_1)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "OffsetEnergy_2_1 Fail" << endl;
					missed++;
				}
				break;
			case 30:
				cfgBuff.OffsetEnergy_2_2 = reinterpret_cast<float&>(temp);
				if (cfgBuff.OffsetEnergy_2_2 == cfgBuffRunVals.OffsetEnergy_2_2)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "OffsetEnergy_2_2 Fail" << endl;
					missed++;
				}
				break;
			case 31:
				cfgBuff.OffsetEnergy_3_1 = reinterpret_cast<float&>(temp);
				if (cfgBuff.OffsetEnergy_3_1 == cfgBuffRunVals.OffsetEnergy_3_1)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "OffsetEnergy_3_1 Fail" << endl;
					missed++;
				}
				break;
			case 32:
				cfgBuff.OffsetEnergy_3_2 = reinterpret_cast<float&>(temp);
				if (cfgBuff.OffsetEnergy_3_2 == cfgBuffRunVals.OffsetEnergy_3_2)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "OffsetEnergy_3_2 Fail" << endl;
					missed++;
				}
				break;
			case 33:
				cfgBuff.OffsetEnergy_4_1 = reinterpret_cast<float&>(temp);
				if (cfgBuff.OffsetEnergy_4_1 == cfgBuffRunVals.OffsetEnergy_4_1)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "OffsetEnergy_4_1 Fail" << endl;
					missed++;
				}
				break;
			case 34:
				cfgBuff.OffsetEnergy_4_2 = reinterpret_cast<float&>(temp);
				if (cfgBuff.OffsetEnergy_4_2 == cfgBuffRunVals.OffsetEnergy_4_2)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "OffsetEnergy_4_2 Fail" << endl;
					missed++;
				}
				break;
			case 35:
				cfgBuff.OffsetPSD_1_1 = reinterpret_cast<float&>(temp);
				if (cfgBuff.OffsetPSD_1_1 == cfgBuffRunVals.OffsetPSD_1_1)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "OffsetPSD_1_1 Fail" << endl;
					missed++;
				}
				break;
			case 36:
				cfgBuff.OffsetPSD_1_2 = reinterpret_cast<float&>(temp);
				if (cfgBuff.OffsetPSD_1_2 == cfgBuffRunVals.OffsetPSD_1_2)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "OffsetPSD_1_2 Fail" << endl;
					missed++;
				}
				break;
			case 37:
				cfgBuff.OffsetPSD_2_1 = reinterpret_cast<float&>(temp);
				if (cfgBuff.OffsetPSD_2_1 == cfgBuffRunVals.OffsetPSD_2_1)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "OffsetPSD_2_1 Fail" << endl;
					missed++;
				}
				break;
			case 38:
				cfgBuff.OffsetPSD_2_2 = reinterpret_cast<float&>(temp);
				if (cfgBuff.OffsetPSD_2_2 == cfgBuffRunVals.OffsetPSD_2_2)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "OffsetPSD_2_2 Fail" << endl;
					missed++;
				}
				break;
			case 39:
				cfgBuff.OffsetPSD_3_1 = reinterpret_cast<float&>(temp);
				if (cfgBuff.OffsetPSD_3_1 == cfgBuffRunVals.OffsetPSD_3_1)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "OffsetPSD_3_1 Fail" << endl;
					missed++;
				}
				break;
			case 40:
				cfgBuff.OffsetPSD_3_2 = reinterpret_cast<float&>(temp);
				if (cfgBuff.OffsetPSD_3_2 == cfgBuffRunVals.OffsetPSD_3_2)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "OffsetPSD_3_2 Fail" << endl;
					missed++;
				}
				break;
			case 41:
				cfgBuff.OffsetPSD_4_1 = reinterpret_cast<float&>(temp);
				if (cfgBuff.OffsetPSD_4_1 == cfgBuffRunVals.OffsetPSD_4_1)
					cout << "Pass " << iter << endl;
				else{
					ofs_cfg << "OffsetPSD_4_1 Fail" << endl;
					missed++;
				}
				break;
			case 42:
				cfgBuff.OffsetPSD_4_2 = reinterpret_cast<float&>(temp);
				if (cfgBuff.OffsetPSD_4_2 == cfgBuffRunVals.OffsetPSD_4_2)
					cout << "Pass " << iter << endl;
				else {
					ofs_cfg << "OffsetPSD_4_2 Fail" << endl;
					missed++;
				}
				break;
			default:
				cout << "Missed congif buff member: " << iter << endl;
				missed++;
				break;
			}
			iter++;
			file_length -= 4;
		}
	}

	cout << endl << "...done." << endl << endl;

	if (missed > 0)
		cout << "Check the file: " << "L2_config_analyze.txt" << " for which configuration parameters did not pass." << endl;
	else {
		cout << "The configuration file has passed." << endl;
		ofs_cfg << endl << "The configuration file has passed." << endl;
	}

	ifs_cfg.close();
	ofs_cfg.close();

	cout << "Press enter to quit..." << endl;
	getline(cin, quit);
    return 0;
}

