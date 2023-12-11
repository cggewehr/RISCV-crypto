int main(void) {
int a,b,c = 0;
asm volatile("asconsigma0h %0, %1, %2\n"
	     "asconsigma1h %0, %1, %2\n"
	     "asconsigma2h %0, %1, %2\n"
	     "asconsigma3h %0, %1, %2\n"
	     "asconsigma4h %0, %1, %2\n"
	     "asconsigma0l %0, %1, %2\n"
	     "asconsigma1l %0, %1, %2\n"
	     "asconsigma2l %0, %1, %2\n"
	     "asconsigma3l %0, %1, %2\n"
	     "asconsigma4l %0, %1, %2\n"
	     :"=r"(a):"r"(b),"r"(c):
);

}
