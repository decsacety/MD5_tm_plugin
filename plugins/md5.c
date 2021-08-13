#include<plugins/md5.h>
#include<stdio.h>
#include<Windows.h>
#include <stdint.h>

#define BUFFER_SIZE 64 // BIT

#define LMove(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

uint32_t a = 0x123456;
uint32_t b = 0x89AbCdEf;
uint32_t c = 0xFeDcBa98;
uint32_t d = 0x76543210;
static int A = 0x123456; static int B = 0x89AbCdEf; static int C = 0xFeDcBa98; static int D = 0x76543210;

// To save the result computed
static char tm_value[16];

// The length of the read file
static uint32_t file_length;

int flag = 0;

// The table is computed by abs(sin(i + 1)) ¡Á 2^32
static uint32_t T[64] = {
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee ,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 ,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be ,
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 ,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa ,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 ,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed ,
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a ,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c ,
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 ,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 ,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 ,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 ,
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 ,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 ,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 
};

// The sequence of paramater
static uint32_t s[64] = {
	7,12,17,22,7,12,17,22,7,12,17,22,7,12,17,22,
	5,9,14,20,5,9,14,20,5,9,14,20,5,9,14,20,
	4,11,16,23,4,11,16,23,4,11,16,23,4,11,16,23,
	6,10,15,21,6,10,15,21,6,10,15,21,6,10,15,21
};

// You`d better init before you use the compute or load_dir
// This function used to init/reset the init-seed(a, b, c, d) for md5.
void init()
{
	a = 0x67452301;
	b = 0xefcdab89;
	c = 0x98badcfe;
	d = 0x10325476;

	A = 0x67452301;
	B = 0xefcdab89;
	C = 0x98badcfe;
	D = 0x10325476;

	file_length = 0;
}



void tm_toB(char* value, uint32_t a) {
	value[0] = (uint8_t)a;
	value[1] = (uint8_t)(a >> 8);
	value[2] = (uint8_t)(a >> 16);
	value[3] = (uint8_t)(a >> 24);
}

// Print the result which computed in the command line. You
// can also output in another place as you like
void print_result() {

	tm_toB(tm_value, a);
	tm_toB(tm_value + 4, b);
	tm_toB(tm_value + 8, c);
	tm_toB(tm_value + 12, d);
	for (int i = 0; i < 16; i++)
		printf("%2.2x", (uint8_t)tm_md5_api->tm_md5->value[i]);
	printf("\n");
}

// Compute the a,b,c,d per block(512B)
void compute(int* M) {

		A = a; B = b; C = c; D = d;

		for (int i = 0; i < 64; i++) {
			int f, g;
			if (i < 16) {
				f = (B & C) | ((~B) & D);
				g = i;
			}
			else if (i < 32) {
				f = (D & B) | ((~D) & C);
				g = (5 * i + 1) % 16;
			}
			else if (i < 48) {
				f = B ^ C ^ D;
				g = (3 * i + 5) % 16;
			}
			else if (i < 64) {
				f = C ^ (B | (~D));
				g = (7 * i) % 16;
			}
			int dTemp = D;
			D = C;
			C = B;
			uint32_t tmp1 = A + f + T[i] + M[g];
			uint32_t tmp3 = tmp1 << s[i];
			tmp1 = tmp1 >> (32 - s[i]);
			B = B + (tmp3 | tmp1);
			A = dTemp;
		}

		a += A; b += B; c += C; d += D;

		flag = 1;
}

// When meet the final block, this will fill the rest byte by the rule of MD5
void fill_buffer(char* buffer, int *data, int block_num) {
	int i = 0;
	char* ptr = buffer -1 ;
	for(int i=0;i<16;i++)
		data[i]=0;
	while (*(++ptr) != -52) // The fread fill the rest part with -52;
	{
		data[i>>2] |= (uint8_t)(*ptr) << ((i % 4) * 8);
		i++;
	}

	file_length = i * 8;
	data[i >> 2] |= 0x80 << ((i % 4) * 8);
	i++;

	if (i < 56) {
		// The length is used in x86. To DO: add the 64-support
		data[14] = 512 * block_num + file_length;
		data[15] = 0;
		compute(data);
	}
	else {
		while (i < 56) {
			data[i >> 2] |= 0x0 << ((i % 4) * 8);
		}
		compute(data);
		for (int j = 0; j < 16; j++) data[j] = 0;
		data[14] = 512 * block_num + file_length;
		compute(data);
	}
}

char* tm_load_dir(const char* path) {
	/*LPCSTR file = path;
	const HANDLE h = CreateFileA(file, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);*/
	FILE* file = fopen(path, "rb");
	if (!file)
		printf("Failed to open%s ...", path);

	uint32_t buffer[BUFFER_SIZE / sizeof(uint32_t)];
	for (int i = 0; i < BUFFER_SIZE / sizeof(uint32_t); i++)
		buffer[i] = NULL;
	uint8_t bu[64];
	uint32_t i = 0;
	init();
	while (1) {
		fread(&bu, sizeof(char), BUFFER_SIZE / sizeof(char), file);
		if (feof(file)) {
			fill_buffer(bu,buffer,i);
			break;
		}
		for (int j = 0; j < 64; j += 4) {
			buffer[j >> 2] = (uint32_t)bu[j]
				| ((uint32_t)bu[j + 1] << 8)
				| ((uint32_t)bu[j + 2] << 16)
				| ((uint32_t)bu[j + 3] << 24);
			bu[j] = -52; bu[j + 1] = -52; bu[j + 2] = -52; bu[j + 3] = -52;
		}
		/*DWORD id;
		HANDLE hThr;
		if (flag == 1)
			hThr = CreateThread(NULL, 0, compute, (uint32_t*)buffer, 0, &id);*/
		compute(buffer);
		i++;
	}

	// For test
	//print_result();
}






struct tm_md5_api* tm_md5_api = &(struct tm_md5_api) {
	.load_dir = tm_load_dir,
	.tm_md5 = &(struct tm_md5_value) { .value = &tm_value, },
	.print_result = print_result,
};