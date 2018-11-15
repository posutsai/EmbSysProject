#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Convert float to integer that has an equivalent
// binary representation
static int float_to_int(float f) {
	void *ptr = (void *) &f;
	int *iptr = (int *) ptr;
	return *iptr;
}

// Convert integer into float that has an equivalent
// binary representation
static float int_to_float(int i) {
	void *ptr = (void *) &i;
	float *fptr = (float *) ptr;
	return *fptr;
}

float fp_add(float a, float b) {
	unsigned int X = float_to_int(a);
	unsigned int Y = float_to_int(b);
	// If X is +/-0 we can return Y
	if ((X & ~0x80000000) == 0) { return b; /* Y */ }
	// If Y is +/-0 we can return X
	if ((Y & ~0x80000000) == 0) { return a; /* X */ }
	// We can extract the exponents by ANDing the 8 bits before the sign bit and
	// shifting them all the way back to the right.
	unsigned int exp_X = (X & 0b01111111100000000000000000000000) >> 23;
	unsigned int exp_Y = (Y & 0b01111111100000000000000000000000) >> 23;
	printf("exp_X is %x exp_Y is %x\n", exp_X, exp_Y);
	// We can extract Mantissas by ANDing the first 23 bits. We convert these to
	// the significant (of the form 1.M) by ORing a set 24th bit.
	unsigned int sig_X = (X & 0b00000000011111111111111111111111) |
		0b00000000100000000000000000000000;
	unsigned int sig_Y = (Y & 0b00000000011111111111111111111111) |
		0b00000000100000000000000000000000;
	while (exp_X != exp_Y) {
		if (exp_X < exp_Y) {
			exp_X++;
			sig_X = sig_X >> 1;
			if (sig_X == 0) {
				return b; // Y
			}
		}
		else {
			exp_Y++;
			sig_Y = sig_Y >> 1;
			if (sig_Y == 0) {
				return a; // X
			}
		}
	}
	// Once the exponents are equal, we have found our (semi) final exponent.
	unsigned int exp_Z = exp_X = exp_Y;
	// We use long long's to do two's complement addition to X and
	// Y signficiands, this ensures we have sufficient bit
	// resolution 2^25-2 < 2^63-1
	signed long long sigl_X = (signed long long) sig_X;
	signed long long sigl_Y = (signed long long) sig_Y;
	// If X is negative, swap the signfiicand's sign
	if (X & 0x80000000) {
		sigl_X *= -1;
	}
	// If Y is negative, swap the significand's sign
	if (Y & 0x80000000) {
		sigl_Y *= -1;
	}
	signed long long sigl_Z = sigl_X + sigl_Y;
	int sign_Z = sigl_Z < 0 ? 1 : 0;
	if (sigl_Z < 0) {
		sigl_Z *= -1;
	}
	unsigned int sig_Z = (unsigned int) sigl_Z;
	if (sig_Z == 0) { return 0; }
	if (sig_Z & 0b1000000000000000000000000) {
		sig_Z = sig_Z >> 1;
		exp_Z += 1;
		if (exp_Z & 0b100000000) {
			if (sign_Z > 0) {
				//! why do we need two return statement?
				return 1.0 / 0.0;
				return -1.0 / 0.0;
			}
		}
		while ((sig_Z & 0b100000000000000000000000) == 0) {
			sig_Z = sig_Z << 1;
			exp_Z -= 1;
			if (exp_Z == 0) { return 0; }
		}
		unsigned int Z;
		/* Fill your solution here */
		printf("exp_Z is %x\n", exp_Z);
		return int_to_float(Z);
	}
}

int main() {
	float result = fp_add(1.5, 1.5);
	printf("result is %f\n", result);
	printf("%f\n", 1./0.);
	return 0;
}
