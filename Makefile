all: ps1_s.out ps1_c.out ps2_s.out ps2_c.out 
ps1_s.out:
	gcc -o ps1_s.out ps1_s.c
ps1_c.out:
	gcc -o ps1_c.out ps1_c.c
ps2_s.out:
	gcc -o ps2_s.out ps2_s.c
ps2_c.out:
	gcc -o ps2_c.out ps2_c.c
