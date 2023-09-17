#include <test_utils.h>

#include <sys/asm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "simple_system_common.h"

int main(void)
{
  //int a = 3;
  //int b = 4;      
  //int c = 0;
  //c = a + b;


  asm volatile (
    "addi x28, x0, 3 \n"
    "addi x29, x0, 4 \n"
    "rol x30, x28, x29 \n"
  ::);

  //puthex(c);

  return 0;
}
