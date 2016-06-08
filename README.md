# vad_for_voip



## sample code

```vad.cpp
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "LTSD.h"
using namespace std;

int main() {
    LTSD ltsd(256, 8000, 7);

    FILE *fp;
	char s[512];

    if ((fp = fopen("data.raw", "r")) == NULL) {
		printf("file open error\n");
		exit(EXIT_FAILURE);
	}

    while (fgets(s, 512, fp) != NULL) {
        bool is_signal = ltsd.process(s);
        cout << is_signal << endl;
        if(is_signal){
            char *hoge = ltsd.getSignal();
            delete[] hoge;
        }
        memset(s, 0, sizeof(s));
    }

    fclose(fp);
    return 0;
}
```
