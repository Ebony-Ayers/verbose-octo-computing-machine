//r1 = y
//r2 = xIncrement adjusted for SIMD
//r3 = x
//r4 = yIncrement

PS_8 r5;
PS_8 r6;
PS_8 r7;
PS_8 r8;
PS_8 r9;
PS_8 r10;
PS_8 r11;
PS_8 r12;
PS_8 r13;
PS_8 r14;
PS_8 r15;
PS_8 r16;

//x mod 2pi
constSetPS<0.159154943092f>(r6);
r5 = _mm256_mul_ps(r3, r6);
constSetPS<0.00001f>(r6);
r5 = _mm256_add_ps(r5, r6);
r5 = _mm256_round_ps(r5, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
constSetPS<6.28318530718f>(r6);
r5 = _mm256_mul_ps(r5, r6);
r5 = _mm256_sub_ps(r3, r5);
//-sin(n)
constSetPS<3.14159265359f>(r6);		//r6 = pi
r5 = _mm256_sub_ps(r5, r6);			//r5 = n - pi
r9 = _mm256_mul_ps(r5, r5);			//r9 = (n - pi)^2
r7 = _mm256_mul_ps(r5, r9);			//r7 = (n - pi)^3
r10 = _mm256_mul_ps(r7, r9);		//r10 = (n - pi)^5
r11 = _mm256_mul_ps(r10, r9);		//r11 = (n - pi)^7
r12 = _mm256_mul_ps(r11, r9);		//r11 = (n - pi)^9
r13 = _mm256_mul_ps(r12, r9);		//r11 = (n - pi)^11
constSetPS<-0.166666666667f>(r8);	//r8 = -1/3!
r7 = _mm256_mul_ps(r7, r8);			//r7 = -1/3! * (n - pi)^3
r7 = _mm256_add_ps(r7, r5);			//r7 = (n - pi) - 1/3! * (n - pi)^3
constSetPS<0.00833333333333f>(r14);	//r14 = 1/5!
constSetPS<-0.000198412698413f>(r15);//r15 = -1/7!
constSetPS<2.75573192240e-6f>(r16);	//r16 = 1/9!
constSetPS<-2.50521083854e-8f>(r6);	//r6 = -1/11!
r10 = _mm256_mul_ps(r10, r14);		//r10 = 1/5! * (n - pi)^5
r11 = _mm256_mul_ps(r11, r15);		//r11 = -1/7! * (n - pi)^7
r12 = _mm256_mul_ps(r12, r16);		//r12 = 1/9! * (n - pi)^9
r13 = _mm256_mul_ps(r13, r6);		//r13 = -1/11! * (n - pi)^11
r10 = _mm256_add_ps(r10, r11);		//r10 = 1/5! * (n - pi)^5 - 1/7! * (n - pi)^7
r12 = _mm256_add_ps(r12, r13);		//r12 = 1/9! * (n - pi)^9 - 1/11! * (n - pi)^11
r7 = _mm256_add_ps(r7, r10);		//r7 = (n - pi) - 1/3! * (n - pi)^3 + 1/5! * (n - pi)^5 - 1/7! * (n - pi)^7
r7 = _mm256_add_ps(r7, r12);		//r7 = (n - pi) - 1/3! * (n - pi)^3 + 1/5! * (n - pi)^5 - 1/7! * (n - pi)^7 + 1/9! * (n - pi)^9 - 1/11! * (n - pi)^11




r5 = _mm256_add_ps(r1, r7);		//r5 = y-sin(x)
/*
r9 = FPABS_SIGN_BIT_MASK;
r6 = _mm256_add_ps(r3, r1);		//r6 = x+y
r6 = _mm256_andnot_ps(r9, r6);	//r6 = abs(x+y)

r7 = _mm256_sub_ps(r3, r1);		//r7 = x-y
r7 = _mm256_andnot_ps(r9, r7);	//r6 = abs(x-y)

constSetPS<1.0f>(r8);
r6 = _mm256_add_ps(r6, r7);		//r6 = abs(x+y) + abs(x-y)
r6 = _mm256_sub_ps(r6, r8);		//r6 = abs(x+y) + abs(x-y) - 1.0
constSetPS<0.0f>(r8);
r6 = _mm256_max_ps(r6, r8);		//r6 = max( abs(x+y) + abs(x-y) - 1.0, 0.0 )

r5 = _mm256_mul_ps(r5, r6);		//r5 = (y-sin(x)) * max(abs(x+y) + abs(x-y) - 1.0, 0.0)

constSetPS<0.25f>(r6);
r7 = _mm256_mul_ps(r3, r3);		//r7 = x*x
r6 = _mm256_sub_ps(r6, r7);		//r6 = 0.25 - (x*x)
r7 = _mm256_sub_ps(r1, r1);		//r7 = y*y
r6 = _mm256_sub_ps(r6, r7);		//r6 = 0.25 - (x*x) - (y*y)
r6 = _mm256_max_ps(r6, r8);		//r6 = max(0.25 - (x*x) - (y*y), 0.0)

r5 = _mm256_add_ps(r5, r6);		//r5 = (y-sin(x)) * max(abs(x+y) + abs(x-y) - 1.0, 0.0) + max(0.25 - (x*x) - (y*y), 0.0)

r5 = _mm256_andnot_ps(r9, r5);	//r5 = abs((y-sin(x)) * max(abs(x+y) + abs(x-y) - 1.0, 0.0) + max(0.25 - (x*x) - (y*y), 0.0))
*/
