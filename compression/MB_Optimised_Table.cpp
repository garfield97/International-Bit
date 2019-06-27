#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <map>
#include <fstream>
#include <sstream>
#include <cmath>
#include <stdint.h>
using namespace std;

void check_freq_n(int n);

int freq[16777216];									  //need to change array size depending on the length measured
uint8_t ordered_freq[256];

int main()
{
	int length = 1;
	check_freq_n (length);							   //generate array freq[]

	int non_zero_count = 0;							   //counting the number of existing discrete values
	double total_freq = 0;							   //counting the total number of Bytes for the data sequence

	for (int i = 0; i < (sizeof(freq)/4); i++){
		if (freq[i] >= 1){
			non_zero_count ++;
			//cout << i << ":" << freq[i] << "\t";	   //checking frequency of discrete values
			total_freq += freq[i];
		}
	}

//	Measuring Entropy
	float entropy = 0;
	float total_prob = 0;
	for (int i =0; i<16777216; i++){
		if (freq[i] !=0)
			entropy += -(freq[i]/total_freq*log2(freq[i]/total_freq));
			total_prob += (freq[i]/total_freq);
	}
	cout << endl;
	cout << "Number of Values that exist = " << non_zero_count << endl;
	cout << "Total Bytes = " << total_freq * length << endl;
	cout << "Entropy = " << entropy << endl;
	cout << "Total Probability = " << total_prob << endl;
}

void check_freq_n(int n){
	ifstream map;
	string line;
	uint32_t map_val;

	bool detect = false;
	int bit_count = 0;
	int stop_count = 0;
	stringstream temp_bit;
	string x = "x";
	string comma = ",", a = "a", b = "b", c = "c", d = "d", e = "e", f = "f";

	cout << "Checking Frequency of sequence with length of " << n << endl;
	map.open("aug_bitmap_full.txt");
	if (map.is_open()){
		cout << "map opened" << endl;
		//reading in the each line from the mapping library
		while (getline(map,line)){
			//read in a byte of data whenever 0x is detected
			//store n bytes string data into temp_bit, where n is the specified length of bytes to be measured,
			//and convert temp_bit from string to integer value
			//the frequency of the corresponding integer value will increment by 1
			for (int k = 0; k < line.size(); k++){
				if (bit_count == n * 2){
					bit_count = 0;
					map_val = 0;
					for (int i = 0; i < (n * 2); i = i+2){
						if (temp_bit.str()[i+1] == comma[0]){
							if (temp_bit.str()[i] == a[0] || temp_bit.str()[i] == b[0] || temp_bit.str()[i] == c[0] ||
								temp_bit.str()[i] == e[0] || temp_bit.str()[i] == d[0] || temp_bit.str()[i] == f[0]){
								map_val += (temp_bit.str()[i] - 87) * pow(16, i);
							}
							else{
								map_val += (temp_bit.str()[i] - 48) * pow(16, i);
							}
						}
						else{
							if (temp_bit.str()[i] == a[0] || temp_bit.str()[i] == b[0] || temp_bit.str()[i] == c[0] ||
								temp_bit.str()[i] == e[0] || temp_bit.str()[i] == d[0] || temp_bit.str()[i] == f[0]){
								map_val += (temp_bit.str()[i] - 87) * pow(16, i+1);
							}
							else{
								map_val += (temp_bit.str()[i] - 48) * pow(16, i+1);
							}
							if (temp_bit.str()[i+1] == a[0] || temp_bit.str()[i+1] == b[0] || temp_bit.str()[i+1] == c[0] ||
								temp_bit.str()[i+1] == e[0] || temp_bit.str()[i+1] == d[0] || temp_bit.str()[i+1] == f[0]){
								map_val += (temp_bit.str()[i+1] - 87) * pow(16, i);
							}
							else{
								map_val += (temp_bit.str()[i+1] - 48) * pow(16 , i);
							}
						}
					}
					//if (map_val < 400000000)		//prevent overflowing in array when map_val too large
					freq[map_val] += 1;
					temp_bit.str(string());
					detect = false;
				}
				if (detect == true){
					temp_bit << line[k];
					bit_count ++;
					stop_count++;
					if (stop_count == 2){
						detect = false;
					}
				}
				if (line[k] == x[0]){
					detect = true;
					stop_count = 0;
				}
			}
		}
	map.close();
	}
}
