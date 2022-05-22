#include <iostream>

uint8_t carry;
int length;
int pos;

uint8_t getYoungerPart(uint8_t a0);
uint8_t getOlderPart(uint8_t a0);

uint8_t addDigitsBCD(uint8_t a0, uint8_t a1);

uint8_t addDigitsBCDWithCarry(uint8_t a0, uint8_t a1);

int findLengthOfNumber(uint8_t *l);

void addBCDNumbers(uint8_t* l1, uint8_t* l2, uint8_t* l3);

void setLength(uint8_t* l1,
	uint8_t* l2);

int getNextDigit(uint8_t* l1, int pos);

int getNextAddress(int length);

void addEndOfNumber(uint8_t* l, int length);

void addFinalCarry(uint8_t* l);

void rightShift(uint8_t* l, int length);

int main() {

	//uint8_t l1[] = { 0x23, 0x43, 0x53, 0xf};
	//uint8_t l2[] = { 0x77, 0x65, 0x70, 0xf};
	//uint8_t l1[] = { 0x23, 0x43, 0x22, 0x59, 0x1f};
	//uint8_t l2[] = { 0x77, 0x65, 0x70, 0xf };
	uint8_t l1[] = { 0x99, 0x99, 0x99, 0xf };
	uint8_t l2[] = { 0x99, 0x99, 0x9f, 0x00 };
	uint8_t l3[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	//printf("Byte: %d\n", l2[2]);

	/*
	uint8_t a = getYoungerPart(l1[0]);
	//std::cout << "Younger part: " << a << "\n";
	printf("Younger part: %d\n", a);

	a = getOlderPart(l1[0]);
	//std::cout << "Older part: " << a << "\n";
	printf("Older part: %d\n", a);

	a = addDigitsBCD(getYoungerPart(l1[0]), getYoungerPart(l2[0]));
	printf("Added digits BCD: %d\n", a);

	length = findLengthOfNumber(l1);
	printf("Length of the number: %d\n", length);

	pos = findLengthOfNumber(l2);

	// We don't count position from 1
	while (pos > 0) {
		printf("Next digit: %d\n", getNextDigit(l2, pos));
		pos--;
	}*/

	addBCDNumbers(l1, l2, l3);

	for (int n : l3) printf("Result: 0x%02x\n", n);

	//for (int n : l1) printf("Result: %d\n", n);

}

void addFinalCarry(uint8_t* l) {
	l[0] = l[0] | (10000);
}

void addEndOfNumber(uint8_t* l, int length) {
	int pos = length / 2;
	l[pos] = l[pos] | 0xf;
}

// Shifts by nibble to the right
void rightShift(uint8_t* l, int length) {

	// Position in the array of the last digit
	int t0 = (length - 1) / 2;
	int t1 = t0 + 1;
	while (t0 >= 0) {
	    // Extract younger nibble from the previous position and place it in the
		// next slot on the older nibble position
		l[t1] = l[t1] | ((l[t0] & 0xf) << 4);
		// Right shift to make place for the next byte
		l[t0] = l[t0] >> 4;
		t0--;
		t1--;
	}
}

// In assembler, this method will shift to the next address if needed
int getNextAddress(int length) {
	if (length - 1 <= 0) return 0;
	if (length % 2 == 1) return 0; // in assembler, shift by one address to the left
	else return 0;
}

void addBCDNumbers(uint8_t* l1, uint8_t* l2, uint8_t* l3) {
	
	int t0 = findLengthOfNumber(l1);
	printf("Length 1: %d\n", t0);
	int t1 = findLengthOfNumber(l2);
	printf("Length 2: %d\n", t1);
	int length = t0 > t1 ? t0 : t1;

	int pos = (length - 1) / 2;
	int originalLength = length;
	int v0 = 0;

	while (length > 0) {

		// place digits in registers a0 and a1 
		v0 = addDigitsBCDWithCarry(getNextDigit(l1, t0),
			getNextDigit(l2, t1));
		// add digits bcd
		// check if the carry occured
		// if it occured, than add 6 to the result

		// On even positions we add younger bits
		if (length % 2 == 0)
			l3[pos] = v0;
		else {
             l3[pos] = l3[pos] | (v0 << 4);
			 pos--;
		}
			
		printf("Addition : %d\n", l3[pos]);
		t0--;
		t1--;
		length--;

	}

	addEndOfNumber(l3, originalLength);

	if (carry == 1) {
		if ((l1[0] >> 4) != 0 && (l2[0] >> 4) != 0) rightShift(l3, originalLength);
		addFinalCarry(l3);
	}
}



// Alternative solution is to right-shift by one position, and
// check the least significant bit to figure out whether the length is even or not
int getNextDigit(uint8_t* l1, int length) {
	uint8_t v0;

	// Pass in length to return 0 in case the number was shorther

	if (length <= 0) v0 = 0;

	else if (length % 2 == 0) {
		v0 = l1[length / 2 - 1]; // Here will be passed an address, but we still need
		// to check whether it is younger or older byte
		printf("Extract younger byte: ");
	}
	else {
		v0 = l1[length / 2] >> 4;
		// We could left-shift an address here 
		// Decrement length here and if it is non-zero, then shift an address
		printf("Extract older byte: ");
	}
	v0 = v0 & 0xf;
	printf("%d\n", v0);
	return v0;
}

// We iterate at most as many times as the length of the smaller number
void setLength(uint8_t* l1,
	uint8_t* l2) {
	length = findLengthOfNumber(l1);
	int t1 = findLengthOfNumber(l2);
	if (length > t1) {
		length = t1;
	}
}

uint8_t getYoungerPart(uint8_t a0) {
	return a0 & (0xf);
}

uint8_t getOlderPart(uint8_t a0) {
	return (a0 >> 4) & 0xf;
}
// Assumption: we add first four bits of each 
// number and save the carry in the global variable
uint8_t addDigitsBCD(uint8_t a0, uint8_t a1) {

	uint8_t t0 = 1;
	uint8_t t1 = 1;
	uint8_t v0 = 0;

	printf("Was carry?: %c\n", (carry == 1) ? 'y' : 'n');

	for (int i = 0; i < 4; i++) {
		// Extract the youngest bit from each number 
		t0 = t0 & a0;
		t1 = t1 & a1;
		printf("Extracted bits: %d, %d\n", t0, t1);
		// Add bits and carry from previous additions
		v0 = v0 | (((t1 ^ t0) ^ carry) << i);
		printf("Addition: %d\n", v0);
		// Update carry
		printf("Updating carry: (%d) | ((%d)&%d)\n", (t0 & t1), (t0 | t1), carry);
		carry = ((t0 | t1) & carry) | (t0 & t1);
		printf("Carry: %d\n", carry);
		t0 = 1;
		t1 = 1;
		// Right shift to add next pair of bits
		a0 = a0 >> 1;
		a1 = a1 >> 1;
	}

	return v0;
}

// Place return address to here at the top of the stack?? 
// Or save return address in some other register to the 
// main loop
uint8_t addDigitsBCDWithCarry(uint8_t a0, uint8_t a1) {
	a0 = addDigitsBCD(a0, a1);
	// sum >= 10, add 6 and set final carry to 0
	if ((a0 >= 10) || (carry == 1)) {
		printf("Addition with overflow: %d\n", a0);
		// Add correction and extract first four bits
		// carry can't be added with correction
		carry = 0;
		a0 = addDigitsBCD(a0, 0x06) & 0xf;
		carry = 1;
		printf("Extract fout bits: %d\n", a0);
	}
	// sum < 9, add 6 and set carry to 1
	// if sum < 9 and carry is 0, then the number is fine
	return a0;
}

int findLengthOfNumber(uint8_t* l) {

	int v0 = 0;
	int t0 = 0;    // current position in the array

	while (true) {
	
		// 0xf means the end of the number
		if (l[t0] == 0xf) 
			break;
		v0++;
		if ((l[t0] & 0xf) == 0xf) break;
		t0++;
		v0++;
	}

	return v0;

}