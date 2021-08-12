#pragma once

#define __TM_EXPORTS__
#ifdef __TM_EXPORTS__
#define TM_EXPORT  __declspec(dllexport)
#else
#define TM_EXPORT __declspec(dllimport)
#endif


// The struct to save the value computed 
typedef struct tm_md5_value {
	// The value of the MD5 compute
	char* value;
}tm_md5_value;

// The api of 
struct tm_md5_api {

	char* (*load_dir)(const char* file_name);

	tm_md5_value(*compute_md5)(char* data);

	tm_md5_value *tm_md5;

	// For some test or command line apply
	void (*print_result)();
};

TM_EXPORT extern struct tm_md5_api* tm_md5_api;




