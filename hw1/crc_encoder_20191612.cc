
// 컴퓨터 네트워크 hw1 
// crc 인코더 만들기 20191612  
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

 int **firsttxt;//입력으로 받은 것 저장하는 배열 
 int **txtarr4;//4자리씩 연산해야 하는 경우 입력을 저장하는 배열 
  int **crcstring;//비트로 나눠서 저장하는 곳
 int * crcmaker ; //나눗셈 수 저장하는 곳 1101 이나 1111 등
 int *fineint ; //최종배열 
 int cword_num;// s_dividor에 따라서 변하는 총 문자열길이 
int s_maker;// 나누기 수의 자리수 
int s_dividor; // 4 or 8 
int s_crcword; //  crc 연산 후의 결과값크기(원래 비트수 + 나누기 수 -1 )
int s_origin; //총 문자열 길이 
int s_pading;// 패딩크기 
int endsize ; 
void modulo2(int *oneline, int *divisor, int endsize){//모듈러2연산 수행하기
  int div_idx = 0,break_flag = 1;
  int index = 0;//최종 인덱스 
  int endidx = s_crcword-s_maker+1;
  int *remain = (int*)malloc(sizeof(int)*s_maker);

  while(oneline[div_idx]==0){//시작점을 찾아보기 
    div_idx++;
    if(div_idx==endidx) {
      break;
      }
  }

  if(div_idx!=endidx){
    for(int i=0; i<s_maker; i++){
      if(oneline[div_idx]==divisor[i]){ 
        remain[i]=0;
        }
      else {
        remain[i]=1;
        }
      div_idx++;
    };


    while(remain[0]==0) {
      for(int i=0; i<s_maker-1; i++) {
        remain[i] = remain[i+1];
      }
      if(div_idx==s_crcword) {
        remain[s_maker-1] = 0;
        div_idx++;
        break;
      }
      remain[s_maker-1] = oneline[div_idx];
      div_idx++;
    }
  }
  if(div_idx<=s_crcword){
    while(break_flag){
      for(int i=0; i<s_maker; i++){
        if(remain[i]==divisor[i]){ 
          remain[i]=0;
          }
        else remain[i]=1;
      }
      while(remain[0]==0) {
        for(int i=0; i<s_maker-1; i++){
           remain[i] = remain[i+1];
          }
        if(div_idx==s_crcword) {
          remain[s_maker-1] = 0;
          break_flag = 0;
          break;
        }
        remain[s_maker-1] = oneline[div_idx];
        div_idx++;
      }
     
    }
  }
 
  for(int i=s_dividor; i<s_crcword; i++){
    oneline[i] = remain[index];
    index++;
  }
  return ; 
  
}

void freeall(){//메모리할당한 것 다시 해제하기 
  
    //crcstring = (int**)malloc(sizeof(int*)*(cword_num));
   // for(int i=0; i<cword_num; i++) crcstring[i] = (int*)malloc(sizeof(int)*s_crcword);
  
  // txtarr4 = (int**)malloc(sizeof(int*)*(s_origin*2));
  //firsttxt = (int**)malloc(sizeof(int*)*s_origin);
 for(int i = 0; i<s_origin; i++){
   free(firsttxt[i]) ; 
  }
  free(firsttxt) ; 
  if(s_dividor == 4){

    for (int i = 0; i< (s_origin*2); i++){
        free(txtarr4[i]) ; 
    }
        free(txtarr4) ; 
  }
  for(int i = 0  ;i < cword_num ; i++){
   free(crcstring[i]) ; 

   }
  free(crcstring) ; 
  free(fineint) ; 
  free(crcmaker); 
}
int main(int argc, char *argv[]) {
    int lencost = 128; //마지막으로 파일에 쓸 때 사용할 자릿값
    int sum = 0;//마지막으로 파일에 쓸 때 사용할 문자의 int
    char r_onechar[1]; //읽을 때 사용할 char
    int oneascii;
    int idx = 0;
    FILE *infp, *outfp;
    infp = fopen(argv[1], "rb");
    outfp = fopen(argv[2], "wb");
    if(argc != 5) {
	  printf("usage: ./crc_encoder input_file output_file generator dataword_size\n");
	  return 1; 
      }
    if (infp == NULL) {
		printf( "input file open error.\n");
		return 1; 
        }

    if (outfp == NULL) {
		printf( "output file open error.\n");
		return 1; 
        }

    if(strcmp(argv[4], "4")!=0 && strcmp(argv[4], "8")!=0) {
        printf("dataword size must be 4 or 8.\n");
        return 1; 
        }

    fseek(infp, 0, SEEK_END);
    s_origin = ftell(infp);
    fseek(infp, 0,SEEK_SET ) ; 
    ////그냥 파일의 문자열 길이알아내고다시 원상복기   

    firsttxt = (int**)malloc(sizeof(int*)*s_origin);
    for(int i=0; i<s_origin; i++) firsttxt[i] = (int*)malloc(sizeof(int)*8);

    s_maker = (int)strlen(argv[3]);
    s_dividor = atoi(argv[4]);
    s_crcword = s_dividor+s_maker-1; 

    crcmaker = (int*)malloc(sizeof(int)*s_maker);
    for(int i=0; i<s_maker; i++){ 
        crcmaker[i] = ((int)(argv[3][i]))-48;
        }
    // 생성자가 generator 배열에 들어가고 {1,1,0,1}형태이다 


    
 
    while(fread(r_onechar, sizeof(r_onechar), 1, infp)){
     //하나의 글자씩 읽어가지고 아스키 코드로 변환
       
        oneascii = (int)r_onechar[0];
          if(oneascii < 0){
          oneascii = oneascii*(-1)-1;
          for(int i=7; i>=0; i--){ 
            firsttxt[idx][i] = (0 == oneascii%2); 
            oneascii/=2; 
            }
        }else{
            for ( int i = 0 ; i<=7 ; i++){
                firsttxt[idx][7-i] = oneascii%2;
                 oneascii /=2; 
            }
        }
        idx++;
    }

  
    if(s_dividor == 4){// 4 자리씩 나눠서 crc하는 경우는 배열 한 줄에 4자리 수가 들어갸야 한다 (대신 배열의 높이가 늘어나야해)
      int ii = 0; 
      int jj = 0;
      txtarr4 = (int**)malloc(sizeof(int*)*(s_origin*2));
      for(int i=0; i<s_origin*2; i++) {
        txtarr4[i] = (int*)malloc(sizeof(int)*4);  
      }
      for(int i=0; i<s_origin; i++){
        for(int j=0; j<8; j++){
          txtarr4[ii][jj] = firsttxt[i][j];
          jj++;
          if(jj==4){ 
            ii++; 
            jj=0; 
         }
        }
      }
    
    }



    cword_num = s_origin;
    if(s_dividor == 4) {
        cword_num*=2;
    }
    s_pading = ((s_crcword*cword_num)/8+1)*8 - (s_crcword*cword_num);
    if(s_pading==8) {
        s_pading =0;
    }

    crcstring = (int**)malloc(sizeof(int*)*(cword_num));
    for(int i=0; i<cword_num; i++) crcstring[i] = (int*)malloc(sizeof(int)*s_crcword);
   //crc 연산하고 나면 문자열 크기가 s_crcword로 변경된다 (아직 뒤의 자리들은 0으로) 
    int **temp= firsttxt;
    // idx = 0;
    if(s_dividor == 4) {
        temp = txtarr4;
    }
    for(int i=0; i<cword_num; i++){
      for(int j=0; j<s_crcword; j++){
        if(j>=s_dividor){ 
            crcstring[i][j] = 0; 
            continue; 
        }
        crcstring[i][j] = temp[i][j];
      }
    }
    


    endsize = s_crcword*cword_num+s_pading; 
    fineint = (int*)malloc(sizeof(int)*(endsize));
    int lastidx = 0;
    for(int i=0;i<s_pading;i++) {
      fineint[lastidx] = 0;
      lastidx++;
    }
    
    for(int i=0; i<cword_num; i++){
    // 모듈러2 연산으로 실행하기 
     modulo2(crcstring[i], crcmaker, endsize);

    // 최종배열에 다시 저장하기 
    for(int j=0; j<s_crcword; j++) {
        fineint[lastidx] = crcstring[i][j];
        lastidx++;
      }
    }

    char onechar[1]; 
    onechar[0] = (char)(s_pading);
    fwrite(onechar, sizeof(onechar),1,outfp); 
    
    for(int i=0; i<=endsize; i++){
      if(i!=0) {
        if(i%8==0){
          onechar[0] = (char)(sum);
         
          fwrite(onechar,sizeof(onechar),1,outfp); 
          if(i == endsize) {
            break;
            }        
        sum = 0;
        lencost = 128;
        }
      }
      sum += (lencost*fineint[i] );
      lencost/=2;
    }
    freeall() ;
    fclose(infp);
    fclose(outfp);
    return 0;
}