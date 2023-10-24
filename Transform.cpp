#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef unsigned char BYTE;

#define WIDTH	256
#define HEIGHT	256
#define BLOCK_SIZE	8 //DCT size

unsigned char QP = 20;

// Memory management
unsigned char** MemAlloc_UC_2D(const int width, const int height);
double** MemAlloc_D_2D(const int width, const int height);
void MemFree_UC_2D(unsigned char** arr, const int height);
void MemFree_D_2D(double** arr, const int height);

// Image file management
void ImgRead(const char* filename, unsigned char** img_in, const int width, const int height);
//void ImgWrite(const char* filename, unsigned char** img_in, const int width, const int height);

// Discrete consin Transform
// separable block-based forward DCT
//void SeparableBlockFDCT_2D(double** input, double** coeff, const int width, const int height, const int block_size);
// separable block-based inverse DCT
//void SeparableBlockIDCT_2D(double** coeff, double** output, const int width, const int height, const int block_size);

void FDCT_1D(const double* input, double* coeff, const int N);
void IDCT_1D(const double* coeff, double* output, const int N);

// Matrix operation
void MatTranspose(double** mat, const int size);

// block-based 2D forward DCT
void BlockFDCT_2D(const double** input, double* coeff, const int width, const int height, const int block_size);
// block-based 2D Inverse DCT
void BlockIDCT_2D(const double** coeff, double* output, const int width, const int height, const int block_size);

// Quantization 
void Quantization(const double** coeff, double** qp_coeff, const int width, const int height, const int CU_block_size);
void InvQuant(const double** qp_coeff, double** coeff, const int width, const int height, const int CU_block_size);

// 2-D memory allocation for unsigned char type
unsigned char** MemAlloc_UC_2D(const int width, const int height)
{
	unsigned char** arr;
	int i;
	arr = (unsigned char**)malloc(height * sizeof(unsigned char*));
	for (i = 0; i < height; i++) arr[i] = (unsigned char*)malloc(width * sizeof(unsigned char));
	return arr;
}

// 2-D memory allocation for double type
double** MemAlloc_D_2D(const int width, const int height) 
{
	double** arr;
	int i;
	arr = (double**)malloc(height * sizeof(double*));
	for (i = 0; i < height; i++) arr[i] = (double*)malloc(width * sizeof(double));
	return arr;
}

// 2-D memory free for unsigned char type
void MemFree_UC_2D(unsigned char** arr, const int height) 
{
	int i;
	for (i = 0; i < height; i++) free(arr[i]);
	free(arr);
}

// 2-D memory free for double type
void MemFree_D_2D(double** arr, const int height) 
{
	int i;
	for (i = 0; i < height; i++) free(arr[i]);
	free(arr);
}

// image file read
void ImgRead(const char* filename, unsigned char** img_in, const int width, const int height) 
{
	FILE* fp_in;
	int i;
	fopen_s(&fp_in, filename, "rb");
	for (i = 0; i < height; i++) fread(img_in[i], sizeof(unsigned char), width, fp_in);
	fclose(fp_in);
}

 // image file write
void ImgWrite(const char* filename, unsigned char** img_out, const int width, const int height) 
{
	FILE* fp_out;
	int i;
	fopen_s(&fp_out, filename, "wb");
	for (i = 0; i < height; i++) fwrite(img_out[i], sizeof(unsigned char), width, fp_out);
	fclose(fp_out);
}

void FDCT_1D(const double* input, double* coeff, const int N) 
{
	double PI = 3.1415926535;
	int n, k;

	for (k = 0; k < N; k++) 
	{
		double beta = (k == 0) ? 1 / sqrt(2.0) : 1;
		double temp = 0;
		for (n = 0; n < N; n++) 
		{
			double basis = cos(((2 * n + 1) * PI * k) / (2.0 * N));
			temp += input[n] * basis;
		}

		temp *= sqrt(2 / (double)N) * beta;

		coeff[k] = temp;
	}
}

void IDCT_1D(const double* coeff, double* output, const int N) 
{
	const double PI = 3.1415926535;
	int n, k;

	for (n = 0; n < N; n++) 
	{
		double temp = 0;
		for(k=0; k < N; k++)
		{
			double beta = (k == 0) ? 1 / sqrt(2.0) : 1;
			double basis = cos(((2 * n * 1) * PI * k) / (2.0 * N));
			temp += beta * coeff[k] * basis;
		}
		temp *= sqrt(2 / (double)N);

		output[n] = temp;
	}
}

void SeparableBlockFDCT_2D(double** input, double** coeff, const int width, const int height, const int block_size) 
{
	int i, j;
	double** temp_hor = MemAlloc_D_2D(block_size, block_size);
	double** temp_ver = MemAlloc_D_2D(block_size, block_size);
	double** temp = MemAlloc_D_2D(block_size, block_size);

	int m, n;

	for (i = 0; i < height; i += block_size) 
	{
		for (j = 0; j < width; j += block_size) 
		{
			for (m = 0; m < block_size; m++) 
			{
				for (n = 0; n < block_size; n++)	temp[m][n] = input[i + m][j + n];
			}
			
			for (m = 0; m < block_size; m++)
				FDCT_1D(temp[m], temp_hor[m], block_size);
			MatTranspose(temp_hor, block_size);

			for (n = 0; n < block_size; n++)
				FDCT_1D(temp_hor[n], temp_ver[n], block_size);
			MatTranspose(temp_ver, block_size);

			for (m = 0; m < block_size; m++) 
			{
				for (n = 0; n < block_size; n++)	coeff[i + m][j + n] = temp_ver[m][n];
			}
		}
	}
	MemFree_D_2D(temp_hor, block_size);
	MemFree_D_2D(temp_ver, block_size);
	MemFree_D_2D(temp, block_size);
}

void SeparableBlockIDCT_2D(double** coeff, double** output, const int width, const int height, const int block_size) 
{
	int i, j;
	double** temp_hor = MemAlloc_D_2D(block_size, block_size);
	double** temp_ver = MemAlloc_D_2D(block_size, block_size);
	double** temp = MemAlloc_D_2D(block_size, block_size);

	int m, n;

	for (i = 0; i < height; i += block_size) 
	{
		for (j = 0; j < width; j += block_size) 
		{
			for (m = 0; m < block_size; m++) 
			{
				for (n = 0; n < block_size; n++)
					temp[m][n] = coeff[i + m][j + n];
			}

			for (m = 0; m < block_size; m++)
				IDCT_1D(temp[m], temp_hor[m], block_size);
			MatTranspose(temp_hor, block_size);

			for (n = 0; n < block_size; n++)
				IDCT_1D(temp_hor[n], temp_ver[n], block_size);
			MatTranspose(temp_ver, block_size);

			for (m = 0; m < block_size; m++) 
			{
				for (n = 0; n < block_size; n++) 
					output[i + m][j + n] = temp_ver[m][n];
				
			}
		}
	}
	MemFree_D_2D(temp_hor, block_size);
	MemFree_D_2D(temp_ver, block_size);
	MemFree_D_2D(temp, block_size);
}

void MatTranspose(double** mat, const int size) 
{
	int i, j;

	for (i = 0; i < size; i++) 
	{
		for (j = i + 1; j < size; j++) 
		{
			double temp = mat[i][j];
			mat[i][j] = mat[j][i];
			mat[j][i] = temp;
		}
	}
}

void BlockFDCT_2D(const double** input, double** coeff, const int width, const int height, const int block_size) 
{
	const double PI = 3.1415926535;
	int m, n, i, j, x, y;
	for (m = 0; m < width; m += block_size) 
	{
		for (n = 0; n < height; n += block_size) 
		{
			for (x = 0; x < block_size; x++) 
			{
				double cx = (x == 0) ? 1 / sqrt(2.0) : 1;
				for (y = 0; y < block_size; y++) 
				{
					double cy = (y == 0) ? 1 / sqrt(2.0) : 1;
					double temp = 0;
					for (i = 0; i < block_size; i++) 
					{
						for (j = 0; j < block_size; j++) 
						{
							double basis = cos((2 * i * 1) * PI * x / (2.0 * block_size)) * cos((2 * j + 1) * PI * y / (2.0 * block_size));
							temp += input[i][j] * basis;
						}
					}
					temp *= (cx * cy) / (block_size / 2.0);
					coeff[x][y] = temp;
				}
			}
		}
	}
}

void BlockIDCT_2D(const double** coeff, double** output, const int width, const int height, const int block_size) 
{
	const double PI = 3.1415926535;
	int m, n, x, y, i, j;

	for (m = 0; m < width; m += block_size) 
	{
		for (n = 0; n < height; n += block_size) 
		{
			for (x = 0; x < block_size; x++) 
			{
				double temp = 0;
				double cx = (x == 0) ? 1 / sqrt(2.0) : 1;
				for (y = 0; y < block_size; y++) 
				{
					double cy = (y == 0) ? 1 / sqrt(2.0) : 1;

					for (i = 0; i < block_size; i++) 
					{
						for (j = 0; j < block_size; j++) 
						{
							double basis = cos((2 * i + 1) * PI * x / (2.0 * block_size)) * cos((2 * j + 1) * PI * y / (2.0 * block_size));
							temp += cx * cy * coeff[i][j] * basis;
						}
					}
					output[x][y] = temp;
				}
			}
		}
	}
}

void Quantization(const double** coeff, double** qp_coeff, const int width, const int height, const int CU_block_size) 
{
	int QuantScale[6] = { 26214, 23302, 20560, 18394, 16384, 14564 };  // 2^14 Up-Scale

	int i, j, m, n;

	for (i = 0; i < height; i += CU_block_size) 
	{
		for (j = 0; j < width; j += CU_block_size) 
		{
			for (m = 0; m < CU_block_size; m++) 
			{
				for (n = 0; n < CU_block_size; n++) 
				{
					qp_coeff[i + m][j + n] = (int)(coeff[i + m][j + n] * QuantScale[QP % 6] + pow(2., 14 + (QP / 6) + 5) / 3) >> (14 + QP / 6 + 5); // f => intra prediction
				}
			}
		}
	}
}

void InvQuant(const double** qp_coeff, double** coeff, const int width, const int height, const int CU_block_size) 
{
	int LevelScale[6] = { 40, 45, 51, 57, 64, 72 };  // 2^6 Up-Scale

	int i, j, m, n;

	int shift = 5;

	for (i = 0; i < height; i += CU_block_size) 
	{
		for (j = 0; j < width; j += CU_block_size) 
		{
			for (m = 0; m < CU_block_size; m++) 
			{
				for (n = 0; n < CU_block_size; n++) 
				{
					coeff[i + m][j + n] = ((int)(qp_coeff[i + m][j + n] * (LevelScale[QP % 6] << QP / 6) * 16  + (1 << (shift - 1))) >> shift); // f => intra prediction
				}
			}
		}
	}
}

int main()
{
	int i, j, cnt;
	clock_t start;

	BYTE** img_in = MemAlloc_UC_2D(WIDTH, HEIGHT);
	BYTE** img_out_sep = MemAlloc_UC_2D(WIDTH, HEIGHT);
	BYTE** img_out_2D = MemAlloc_UC_2D(WIDTH, HEIGHT);
	double** input = MemAlloc_D_2D(WIDTH, HEIGHT);
	double** output = MemAlloc_D_2D(WIDTH, HEIGHT);
	double** coeff_sep = MemAlloc_D_2D(WIDTH, HEIGHT);
	double** coeff_2D = MemAlloc_D_2D(WIDTH, HEIGHT);

	double** qp_coeff = MemAlloc_D_2D(WIDTH, HEIGHT);

	// iamge read
	ImgRead("Lena(256x256).raw", img_in, WIDTH, HEIGHT);
	// type casting
	for (i = 0; i < HEIGHT; i++)
	{
		for (j = 0; j < WIDTH; j++)
		{
			input[i][j] = (double)img_in[i][j];
		}
	}

	start = clock();
	SeparableBlockFDCT_2D(input, coeff_sep, WIDTH, HEIGHT, BLOCK_SIZE);
	printf("%dx%d separable block-based FDCT to %dx%d image: %.2f ms\n", BLOCK_SIZE, BLOCK_SIZE, WIDTH, HEIGHT, (double)1000 * (clock() - start / CLOCKS_PER_SEC));

	//type casting and clipping
	for (i = 0; i < HEIGHT; i++)
	{
		for (j = 0; j < WIDTH; j++)
		{
			double temp = coeff_sep[i][j];
			temp = (temp > 255) ? 255 : (temp < 0) ? 0 : temp;
			img_out_sep[i][j] = (BYTE)floor(temp + 0.5);
		}
	}

	// iamge write
	ImgWrite("[SeparableFDCT]Lena(256x256).raw", img_out_sep, WIDTH, HEIGHT);

	// separable block-based inverse DCT
	start = clock();
	SeparableBlockIDCT_2D(coeff_sep, output, WIDTH, HEIGHT, BLOCK_SIZE);
	printf("%dx%d separable block-based IDCT to %dx%d image: %.2f ms\n", BLOCK_SIZE, BLOCK_SIZE, WIDTH, HEIGHT, (double)1000 * (clock() - start / CLOCKS_PER_SEC));

	//type casting and clipping
	for (i = 0; i < HEIGHT; i++)
	{
		for (j = 0; j < WIDTH; j++)
		{
			double temp = output[i][j];
			temp = temp > 255 ? 255 : temp < 0 ? 0 : temp;
			img_out_sep[i][j] = (BYTE)floor(temp + 0.5);
		}
	}

	// image write
	ImgWrite("[SeparableIDCT]Lena(256x256).raw", img_out_sep, WIDTH, HEIGHT);

	MemFree_UC_2D(img_in, HEIGHT);
	MemFree_UC_2D(img_out_sep, HEIGHT);
	MemFree_UC_2D(img_out_2D, HEIGHT);
	MemFree_D_2D(input, HEIGHT);
	MemFree_D_2D(output, HEIGHT);
	MemFree_D_2D(coeff_sep, HEIGHT);
	MemFree_D_2D(coeff_2D, HEIGHT);
	MemFree_D_2D(qp_coeff, HEIGHT);
}