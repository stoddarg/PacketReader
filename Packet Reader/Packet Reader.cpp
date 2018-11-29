// Packet Reader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

using namespace std;

int main()
{
	//Variables
	int i = 0;
	int apid = 0;
	int file_length = 0;
	int packet_length = 0;
	int num_packets = 0;
	unsigned int test_value = 0;
	unsigned int sync_marker = 0x352EF853;
	string quit = "";
	string infile = "";
	//string infile = "L2_tt_macro_0_1.bin";	//Change this file to something else to read a different file
	//string infile = "L2_tt_macro_0_2.bin";
	//string infile = "L2_tt_macro_0_3.bin";
	//string infile = "L2_tt_ecal_macro_0_1.bin";
	//string infile = "L2_tt_ecal_macro_0_2.bin";

	/* I want to read in the filename that the user would like to convert from .bin -> .txt
	* Use getline to get user input and append .bin to the end of it
	* then place that string into the fstream function below.
	*/
	cout << "Before entering files to parse, place them in the folder with this executable." << endl;
	//have the user enter a file name to use
	cout << "Enter a binary file name (without .bin extension) to parse: " << endl;
	getline(cin, infile);
	infile += ".bin";
	//test that the file exists before trying to open it:
	if (!experimental::filesystem::exists(infile))
	{
		cout << "The file does not exist in the correct folder." << endl;
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
	//open both streams for use
	ifs.open(infile, ios_base::in | ios_base::binary);
	ofs.open("L2_packets_read.txt", ios::app);
	ofs_vals.open("L2_packets_raw_vals.txt", ios::app);
	//indicate that the output file is a new read and state the file being read
	ofs << "******************************************" << endl;
	ofs << "Reading packets from the file: " + infile << endl;
	ofs_vals << "******************************************" << endl;
	ofs_vals << "Reading packets from the file: " + infile << endl;

	cout << "Begin reading the file: " + infile << endl << endl;
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

	//close out of variables properly
	ifs.close();
	ofs.close();
	ofs_vals.close();
	delete file_buff;

	cout << "Press enter to quit..." << endl;
	getline(cin, quit);
    return 0;
}

