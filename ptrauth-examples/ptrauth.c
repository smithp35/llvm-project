#include <ptrauth.h>
#include <stdint.h>
#include <stdio.h>

// Example to show the default signing schema for C function pointers and a
// custom one via the use of the __ptrauth type qualifier.
//
// Function pointers are statically initialized so we need a
// R_AARCH64_AUTH_RELATIVE relocation.

// Function to be called indirectly.
int f(int x) { return x + 10; }

// default signing schema for function pointers
int (*fp)(int) = &f;
// custom signing schema with no address diversity and a discriminator of 0x2639
// modifier = 0x2639
int (*__ptrauth(ptrauth_key_function_pointer, 0, 0x2639) fp2)(int) = &f;
// custom signing schema with address diversity and a discriminator of 0x8bb0
// modifier = 0x2639 | (&f & 0x0000ffffffffffff)
int (*__ptrauth(ptrauth_key_function_pointer, 1, 0x8bb0) fp3)(int) = &f;
// custom signing schema with address diversity and no discriminator
// modifier = &f
int (*__ptrauth(ptrauth_key_function_pointer, 1, 0x0) fp4)(int) = &f;

int main(void) {
  int i = fp(2);
  int j = fp2(4);
  int k = fp3(8);
  int l = fp4(16);
  printf("%d %d %d %d\n", i, j, k, l);
  return 0;
}
