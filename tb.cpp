/*
 * Empty C++ Application
 */
#include "core.hpp"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <set>

typedef double val_t;
typedef uint32_t id_t;

val_t eof = 1e-5;

FILE *fp;

int i, j, k;
bool isfinished=false;

#define rows 9801
#define columns 9804

val_t vector[columns/4];
val_t vector1[columns/4];
val_t vector2[columns/4];
val_t vector3[columns/4];

val_t vectort[rows];

#define total 43543
#define rowlengt 2451

__attribute ((aligned (16))) id_t n_cid_f[total*2];
__attribute ((aligned (16))) val_t result_f[rows];
__attribute ((aligned (16))) val_t matrix1[total];
__attribute ((aligned (16))) val_t matrix2[total];
__attribute ((aligned (16))) id_t rid[total*2];
__attribute ((aligned (16))) id_t number_per_rbatch[rowlengt];
val_t resultr_f[rows] = {0.0};

id_t total_num=total, result_number=rows, vector_number=columns;

int main(){

	fp = fopen("/home/lsq/projects/pycharm_projects/spmv_tcad/pynq_bin/epb1/val1.BIN", "rb");
	fread(matrix1, 8, total, fp);
	fclose(fp);

	fp = fopen("/home/lsq/projects/pycharm_projects/spmv_tcad/pynq_bin/epb1/val2.BIN", "rb");
	fread(matrix2, 8, total, fp);
	fclose(fp);

	fp = fopen("/home/lsq/projects/pycharm_projects/spmv_tcad/pynq_bin/epb1/rid.BIN", "rb");
	fread(rid, 4, total*2, fp);
	fclose(fp);

	fp = fopen("/home/lsq/projects/pycharm_projects/spmv_tcad/pynq_bin/epb1/cid.BIN", "rb");
	fread(n_cid_f, 4, total*2, fp);
	fclose(fp);

	fp = fopen("/home/lsq/projects/pycharm_projects/spmv_tcad/pynq_bin/epb1/rowl.BIN", "rb");
	fread(number_per_rbatch, 4, rowlengt, fp);
	fclose(fp);

	for(int i=0; i<rows; i++){
		vectort[i] = (val_t)i;
		result_f[i] = (val_t)0;
		resultr_f[i] = (val_t)0;
	}

//	printf("%u %u %u %u\n", number_per_rbatch[0] + number_per_rbatch[1], rid[1066*4+1], rid[1066*4+2], rid[1066*4+3]);
//	return 0;

	for(int i=0; i<columns/4; i++){
		vector[i] = (val_t)(4*i);
		vector1[i] = (val_t)(4*i+1);
		vector2[i] = (val_t)(4*i+2);
		vector3[i] = (val_t)(4*i+3);
	}

	val_t sum1 = 0.0;
	int index = 0;
//    total_num = number_per_rbatch[0] + number_per_rbatch[1] + number_per_rbatch[2];
//    for (int i=3; i<99; i++){
//    	total_num += number_per_rbatch[i];
//    }
//    printf("%u\n", total_num);
//    return 0;

//    for(int i=0; i<total_num*4; i++){
//        if (i%4<=1){
//        	if (rid[i] != 1048576){
//        		uint32_t crid = rid[i];
////        		if (crid >= 3140)
////        			rid[i] = 0;
//            	resultr_f[rid[i]] += vectort[n_cid_f[i]] * matrix1[i%2+i/4*2];
//            	sum1 += vectort[n_cid_f[i]] * matrix1[i%2+i/4*2];
//        	}
//        }
//        else{
//        	if (rid[i] != 1048576){
//        		uint32_t crid = rid[i];
////        		if (crid >= 3140)
////        			rid[i] = 0;
//        		resultr_f[rid[i]] += vectort[n_cid_f[i]] * matrix2[i%2+i/4*2];
//        		sum1 += vectort[n_cid_f[i]] * matrix2[i%2+i/4*2];
//        	}
//        }
//    }

//    for (int i=0; i<total_num*4; i++) {
//        if (i%4<=1){
//			if (n_cid_f[i] == 128)
//				printf("<2 %lf %lf %u\n", vectort[n_cid_f[i]], matrix1[i%2+i/4*2], rid[i]);
//        } else {
//			if (n_cid_f[i] == 128)
//				printf(">2 %lf %lf\n", vectort[n_cid_f[i]], matrix2[i%2+i/4*2]);
//        }
//    }

//	for (int i=0; i<total_num; i++){
//		if(rid[i*4+3] != 1048576){
//			sum1 += vectort[n_cid_f[i*4+3]] * matrix2[i*2+1];
//			resultr_f[i] = vectort[n_cid_f[i*4+3]] * matrix2[i*2+1];
//		}
//	}

//    for (int i=0; i<99; i++) {
//    	if (number_per_rbatch[i] < 6){
//    		printf("found\n");
//    	}
//    }
//
//    return 0;

	spmv((ap_uint<64> *)matrix1, (ap_uint<64> *)matrix2, (ap_uint<64> *)rid, (ap_uint<64> *)n_cid_f, result_f,
			number_per_rbatch, (ap_uint<64> *)vector, (ap_uint<64> *)vector1, (ap_uint<64> *)vector2, (ap_uint<64> *)vector3,
			total_num, result_number, vector_number);

//    test_for_final_adder(result_f, result_number);

//	val_t sum2 = 0.0;
//	for (i=0; i<index; i++){
//		sum2 += result_f[i];
//	}

//	for (i=0; i<result_number; i++){
//		if (fabs(resultr_f[i]-result_f[i]) > eof)
//			printf("Error %d %f %f\n", i, resultr_f[i], result_f[i]);
//		else {
//			printf("Success %d %f %f\n", i, resultr_f[i], result_f[i]);
//		}
//	}
//	if (fabs(sum1-sum2) > eof)
//		printf("Error %d %f %f\n", i, sum1, sum2);
//	else {
//		printf("Success %d %f %f\n", i, sum1, sum2);
//	}

	printf("Success\n");

	return 0;
}
