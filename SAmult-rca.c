/*
		Robert A. Maxwell		11/5/2013
		Computer Arithmetic		Project 2
		University of Texas at San Antonio

 n-bit shift and add multiplier with Ripple-Carry 
	Full Adders

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX_BITS 32
#define MAX_RUNS 1000
#define ARRAY_SIZE MAX_BITS
#define PROD_ARRAY_SIZE ARRAY_SIZE*2 +1
//********************
// Helper Functions
//********************
unsigned int evalRCA(int *in1, int *in2, int *out, int numBits){

	int m;
	int cin = 0;
	unsigned int cout = 0;

	for(m = numBits-1; m >= 0; m--){

		//Ripple-carry logic
		out[m] = in1[m] ^ in2[m] ^ cin;		
		cout = (in1[m] & in2[m]) | (in1[m] & cin) | (in2[m] & cin);
		cin = cout;				// Ripple cout to next Cin

		//printf("\t(2d) RCA Carry Delay\n");
		//printf("\tRCA Intermediate Sum = ");
		//printOperand(out, numBits);

	}

return cout;
}

int *evalMUX( int *in1, int *in2, int mux_ctrl ){

	return mux_ctrl? in2: in1;

}

void printOperand( int *oper, int max_bits ){

	int c;

	for ( c = 0; c < max_bits; c++ ){

		printf("%u", oper[c]);

	}
	printf("\n");
}
/*
*	Generate random binary numbers
*/
void generateOperand( unsigned int *op , int num_bits ){

	int pos;
	unsigned int val;

	for(pos = 0; pos < num_bits; pos++){
		val = rand() % 2;
		
		if(val){
			op[pos] = val;			
		}
	}
}
/*
*	Insert product into upper half of product array
*/
void insertUpperProd(int *p_reg, int num_bits, int * op , int c){

	int m = 0;

	for(m = 0; m < num_bits; m++){

		p_reg[m] = op[m];

	}
}
/*
*	Insert multiplier into lower n-bits of product array
*/
void insertLowerProd(int *p_reg, int num_bits, int * op){

	int m = 0;

	for(m = 0; m < num_bits; m++){

		p_reg[m+num_bits] = op[m];

	}
}
/*
*	retrieve upper portion of product for input into adder
*/
void getUpperProd(int *p_reg, int *pProdReg, int num_bits){

	int m;

	for(m = 0; m < num_bits; m++){

		pProdReg[m] = p_reg[m];

	}
}
/*
*	For copying to mux_out
*/
void arrayCopy( int * arr1, int * arr2, int size){
	
	int m;

	for(m = 0; m < size; m++){

		arr2[m] = arr1[m];

	}
}
/*
*	Register shift
*/
void shiftREG( int *prodReg, int shamt ){

	int m;
	unsigned int temp = prodReg[0];

	for(m = PROD_ARRAY_SIZE-1; m >= 0; m--){

		prodReg[m] = prodReg[m-1];

	} 
	
	prodReg[0] = prodReg[1];
}
/*
*	compliment all bits and add 1 at the end
*/
void comp2(int * val, int num_bits){

	int m;

	for(m = 0; m < num_bits; m++){			//loop for complimenting all bits

		if(val[m])
			val[m] = 0;
		else val[m] = 1;

	}

	for(m = num_bits-1; m >= 0; m--){		//loop for ripple-adding 1 at the end
		
		if(val[m]){							// if 1 in bit position
			val[m] = 0;						// set it to zero
		}else{								// if 0 in bit position
			val[m] = 1;						// set it to one
			break;							// can end the addition at this time
		}	
	}
}
/*
*	for debug - checking product of multiplication
*	 -- returns integer value
*/
double bin2dec(int *bin, int num_bits){

	int m;
	int cnt = 0;
	double dec = 0;	

	for(m = num_bits-1; m >=0; m--){
		
		if(bin[m] == 1)
			dec += pow(2,cnt);

		cnt++;	
	}

return dec;
}

// ****************************************
// Main Function
// ****************************************
int main(void){

	int i, j, k;
	int delay;
	int lastDelay = 0;

	double avgDelay[MAX_BITS];
	int worstDelay[MAX_BITS];
	float delayPerCycle;
	float avgDelayPerCycle[MAX_BITS];
	//float avgCycles[MAX_BITS];

	int A[ARRAY_SIZE];				
	int B[ARRAY_SIZE];				
	int S[ARRAY_SIZE];
	int MUX_IN1[ARRAY_SIZE];	//for holding a zero variable
	int pProd[ARRAY_SIZE];

	srand(time(NULL));			//seed the random number generator

	for ( i = 2; i <= MAX_BITS; i++ ) {

		avgDelay[i-2] = 0;
		avgDelayPerCycle[i-2] = 0;
		worstDelay[i-2] = 0;

		for(j = 0; j < MAX_RUNS; j++){

			/* Initialize variables
			*  for each multiplication
			*/
			unsigned int carry = 0;
			unsigned int mux_ctrl = 0x0;					

			int l;
			int cycles;
			int c_complete = 0;
			int *mux_out;		
			int prod[PROD_ARRAY_SIZE];
			
			for(k = 0; k < ARRAY_SIZE; k++){
				A[k] = 0;					//initialize storage arrays
				B[k] = 0;
				S[k] = 0;
				MUX_IN1[k] = 0;
				pProd[k] = 0;
			}

			for(k = 0; k < PROD_ARRAY_SIZE; k++){
				prod[k] = 0;
			}

			delay = 0;
			
			generateOperand(A, i);			//generate operands A and B
			generateOperand(B, i);
			                                //Initialize product register
			insertLowerProd(prod, i, B);	//B goes in lower half of product

			for(l = 0; l < i; l++){
				//printf("-----------------CYCLE %i-----------------------\n", cycles);
											//1.) evaluate mux based on LSB of product (+2d)
				mux_ctrl = prod[i*2];			//Get last bit (just shifted out)
				//printf("(2d) Mux Delay\n");			
				mux_out = evalMUX(MUX_IN1, A, mux_ctrl);
				//printf("MUX out = ");
				//printOperand(mux_out, i);
				delay += 2;					//MUX Delay

											//2.) evaluate RCA (+2nd)
			
				getUpperProd(prod, pProd, i);
				//printf("-----------------RCA Process---------------\n");
				//printf("RCA in1 = ");
				//printOperand(mux_out, i);
				//printf("RCA in2 = ");
				//printOperand(pProd, i);
				carry = evalRCA(pProd, mux_out, S, i);
				//printf("-------------------------------------------\n");
				delay += 2*i;				//RCA delay
				//printf("(%id) RCA Delay\n", 2*i);

											//3.) storeSum
				insertUpperProd(prod, i, S, carry);		//sum goes in upper half of prod
				//printf("Intermediate Product = ");
				//printOperand(prod, 2*i+1);

											//4.) shift Prod Reg (+2d)
				shiftREG( prod, 1 );
				//printf("(2d) Shift Delay\n");
				delay += 2;						//shift delay
				cycles++;						//new cycle only after
												//mux,RCA,shift delays

				//printf("Shifted Product      = ");
				//printOperand(prod, 2*i+1);
				
			}

			avgDelay[i-2] += delay;
			if(delay > lastDelay)
				worstDelay[i-2] = delay;
				lastDelay = delay;

			delayPerCycle = delay/cycles;
			avgDelayPerCycle[i-2] += delayPerCycle;
			cycles = 0;
			delay = 0;
		}//END FOR LOOP (Each sim run)

		avgDelay[i-2] = avgDelay[i-2]/MAX_RUNS;	
		//avgCycles[i-2] = avgCycles[i-2]/MAX_RUNS;
		avgDelayPerCycle[i-2] = avgDelayPerCycle[i-2]/MAX_RUNS;	
		printf("-------------------------------------\n");
		printf("Number of bits: %i\n", i);
		printf("Average Delay = %.3f\n", avgDelay[i-2]);
		printf("Worst Delay = %i\n", worstDelay[i-2]);
		//printf("Average Num Cycles = %.3f\n", avgCycles[i-2]);	
		printf("Average Delay Per Cycle = %.3f\n", avgDelayPerCycle[i-2]);

	}//END FOR LOOP (Each Set of number of bits)


return 0;
}
