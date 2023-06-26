
//컴퓨터네트워크 hw1
//ccrc 디코더만들기 20191612 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *maker ; //나누기 할 수 
char *codeword_all ; //padding을 포함한 전체 word를 담는 공간
char *dword_arr ; //dataword를 담을 공간
char *backagain ;// 원상복구 문자열 
int s_dividor ; //dividor총길이
char divnum ; //4 or 8 
int s_origin ; //총 문자열 길이 
int s_crcword; //crc최종 결과값 자리수
int input_all; //입력데이터 전체 
char eightbits[8];//비트하나씩 담을 곳
char padding ;//입력의 패딩 
char oneword ; //1글자씩 읽을것
int s_codeword ;//총 길이에서 codeword의 개수를 센다
int s_fine; //원상복구 문자열 길이 
unsigned int temp;//char로 변형될 int
unsigned char datamiss;//오타 개수 저장 

void modulo2(char *codeword, char *maker,int s_crcword, int s_dividor)
{   int i, j;
	for ( i = 0; i <= s_crcword - s_dividor; i++) {
		//0인경우 아닌경우 구분해서 연산 시행 
		if (codeword[i]==0){
			continue;
		}
		for (j = 0; j < s_dividor; j++){
			//값이 같은 경우 xor로 인해 0이되고 다른 경우는 1이 되도록
            if ((codeword[i + j]-maker[j])==0){
                codeword[i + j] = 0;
			}
            else{
                codeword[i + j] = 1;
			}
        }
	}
}

void freeall(){
	free(maker) ; 
	free(dword_arr);
	free(backagain);
	free(codeword_all);
}
int main(int argc, char *argv[]) {
    FILE *infp, *outfp, *resfp;
	infp = fopen(argv[1], "rb"); 
	outfp = fopen(argv[2], "wb");
	resfp = fopen(argv[3], "w");
	if(argc!=6){
		printf( "usage: ./crc_decoder input_file output_file result_file maker dataword_size\n");
		return 1;
	}	if (infp == NULL){
		printf("input file open error.\n");
		return 1;
	}	if (outfp == NULL){
		printf("output file open error.\n");
		return 1;
	}	if (resfp == NULL){
		printf("result file open error.\n");
		return 1;
	}	if(strcmp(argv[5], "4")!=0 && strcmp(argv[5], "8")!=0){
		printf("dataword size must be 4 or 8.\n");
        return 1; 
	}
	divnum= atoi(argv[5]) ; 
    s_dividor = strlen(argv[4]);
    fseek(infp  ,0, SEEK_END) ; 
    s_origin = ftell(infp) ; 
    fseek(infp, 0,SEEK_SET) ;
    maker = (char*)malloc(sizeof(char)*s_dividor) ; 

	for(int i=0;i<s_dividor;i++){
		maker[i]= argv[4][i]-'0';
	}
    s_crcword = divnum+s_dividor-1;

    input_all = (s_origin-1)*8;
	codeword_all = (char*)malloc(sizeof(char)*(input_all));
	
	fread(&padding, 1, 1, infp);
	for(int j=0;j<s_origin-1;j++){
		fread(&oneword, 1, 1, infp);
        char tmp ; //임시 저장소 
        for (int i = 7 ; i>-1 ; i--){
            tmp = oneword &1 ; 
            eightbits[i] = tmp ; 
            oneword = oneword >>1 ; 
        }    
		for(int i=0;i<8;i++){
			codeword_all[i+(j*8)]=eightbits[i]; 
		}
	}
	
	
	s_codeword= (input_all-padding)/s_crcword ;
	int current= padding;//패딩을 제외하고 dataword로 변환한다
	dword_arr = (char*)malloc(sizeof(char)*divnum*s_codeword); //dataword를 담을 공간
	

	int count,j;
	
	for(int i=0;i<s_codeword;i++){
		count=0;
		char *codeword= (char*)malloc(sizeof(char)*s_crcword); 
		for(j=0;j<divnum;j++){
			codeword[j] =codeword_all[current];
			dword_arr[(divnum*i)+j] = codeword_all[current++];
			}
		for(;j<s_crcword;j++){
			codeword[j] =codeword_all[current++];
		}
		modulo2(codeword,maker, s_crcword, s_dividor);
		for(int i=0;i<s_crcword;i++){
			count+=codeword[i];
		}
		if(count!=0){
			datamiss++;
		}
		free(codeword) ; 
	}

	s_fine = ((s_codeword*divnum)+padding)/8;
	backagain =(char *) malloc(sizeof(char) * s_fine);
	
	current=0;
	for(int i=0;i<s_fine;i++){
		temp = 0; 
        for ( int j = 0 ; j< 7 ; j++){//8번째 바로 직전까지만 실행
            temp += dword_arr[current++] ; 
            temp *= 2 ; 
        }
        temp += dword_arr[current] ; 
		current++ ; 
		backagain[i] = temp;
	}
	fwrite(backagain, s_fine, 1, outfp);
	fprintf(resfp,"%d %d",s_codeword,datamiss);

	freeall(); 
	fclose(infp);
	fclose(outfp);
	fclose(resfp);
    return 0;
}