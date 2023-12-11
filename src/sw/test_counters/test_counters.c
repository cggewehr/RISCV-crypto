

void func() {
	int a,b = 0;
	register int a_reg asm("a0") = a;
	register int b_reg asm("a1") = b;
	asm volatile (
		
		"add a0, a1, a0 \n"
		"add a0, a1, a0 \n"
		"add a0, a1, a0 \n"
		"mul a1, a0, a1 \n"
			

	 ::"r" (a_reg), "r" (b_reg):);
}

int main() {
	func();
}
