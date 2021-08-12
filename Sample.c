#include<plugins/md5.h>
#include<stdint.h>
#include <time.h>
#include<Windows.h>

int main() {

	// The default path is begin from solutions-dir 相对路径是silutions下
	tm_md5_api->load_dir("s.d");
	// Show the result 
	tm_md5_api->print_result();

	clock_t start = clock();
	tm_md5_api->load_dir("u.rar");
	tm_md5_api->print_result();
	printf("%f seconds\n", (double)(clock()-start)/ CLOCKS_PER_SEC);

	start = clock();
	system("certutil -hashfile u.rar MD5");
	printf("%f seconds\n", (double)(clock() - start) / CLOCKS_PER_SEC);

}