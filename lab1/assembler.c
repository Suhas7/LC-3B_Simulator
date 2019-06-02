/*
   Name 1: Simon Hoque
   Name 2: Suhas Raja
   UTEID 1: th29344
   UTEID 2: scr2469

 */

#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */
#define MAX_LINE_LENGTH 255
#define maxLabels 400
enum{DONE, OK, EMPTY_LINE};
char* dLabel[maxLabels];
int dAddress[maxLabels];
int dIndex=0;
int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4);
char* cmpOpcode[]={"add", "and", "not", "xor", "lshf", "rshfl", "rshfa", //7 items (complete)
                   "jsrr", "jmp", "jsr", // 3 items (complete)
                   "br", "brn", "brz", "brp", "brnz", "brzn", "brnp", "brpn", "brzp", "brpz", "brnzp", // 11 items (complete)
                   "lea", "ldb", "ldw", "stb", "stw", //5 items (complete)
                   "trap","halt", "ret","rti", "nop"}; //5 items (complete)
char* cmpPseudoOps[]={".orig",".fill",".end"};
char* cmpShorthandOps[]={"in","out","getc","puts"};
int toNum( char * pStr );

// returns an id based on opcode, -1 if invalid
int isOpcode(char *lPtr) ;

int populateDRSR(char* instruction, int d, int s);

int populateOp3(char* instruction, char* operand);

char* b2x(char* bin);

void populateShiftImm(char code[16], char *arg3);

void populateImmediate(char code[16], char *arg3, int bits, int isPointer);

int isPseudoOpcode(char *lPtr) ;

int isLabel(char *lPtr) ;

int isShorthandOp(char* lPtr);

int main(int argc, char* argv[]) {
    char *prgName   = NULL;
    char *iFileName = NULL; //#todo file input interface and file writing
    char *oFileName = NULL;
    prgName   = argv[0];
    iFileName = argv[1];
    oFileName = argv[2];
    printf("program name = '%s'\n", prgName);
    printf("input file name = '%s'\n", iFileName);
    printf("output file name = '%s'\n", oFileName);
    char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
    int lRet;
    FILE * lInfile;
    lInfile = fopen(argv[1], "r" );	/* open the input file */
    FILE * pOutfile;
    int addy, currAddy, begun;
    pOutfile = fopen(argv[2], "w" );
    //SYMBOL TABLE GENERATION
    do{
        lRet = readAndParse( lInfile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
        if( lRet != DONE && lRet != EMPTY_LINE ){
            if(begun) {
                if (strcmp(lLabel, "") != 0) {
                    if (isOpcode(lLabel) != -1){
                        printf("Error: Invalid Label, %s\n",lLabel);
                        exit(4);
                    }
                    if(isShorthandOp(lLabel)!=-1){
                        printf("Error: Invalid Label, %s\n",lLabel);
                        exit(4);
                    }
                    for(int i = 0; i<strlen(lLabel);i++){
                        if(!isalnum(lLabel[i])){
                            printf("Error: Invalid Label, %s\n",lLabel);
                            exit(4);
                        }
                    }
                    dLabel[dIndex] = malloc(sizeof(char)*strlen(lLabel));
                    strcpy(dLabel[dIndex],lLabel);
                    dAddress[dIndex] = currAddy;
                    dIndex++;
                }
                currAddy+=2;
            }
            if(strcmp(lOpcode,".orig")==0){
                begun = 1;
                addy=currAddy=toNum(lArg1);
                if(((addy%2)==1) || (addy>0x0FFFF)){
                    printf("Invalid Orig Address, must be even");
                    exit(3);
                }
            }
        }
    } while( lRet != DONE );
    //reset file
    begun = 0;
    currAddy=addy;
    lInfile = fopen(argv[1], "r" );	/* open the input file */
    //bitwise assembly
    do{
        lRet = readAndParse( lInfile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
        if(0!=strcmp(lArg4,"")){
            printf("4 is too many args");
            exit(4);
        }
        if( lRet != DONE && lRet != EMPTY_LINE ) {
            char currOpCode[16]; //binary housing
            int bitwise=0;
            //check if the opcode is broke
            if ((isOpcode(lOpcode) == -1) && (isPseudoOpcode(lOpcode)==-1)){
                printf("Error: Invalid Opcode, %s\n",lOpcode);
                exit(2);
            }
            //pseudoOps
            if(begun && (currAddy>0x0FFFF)){
                exit(4);
            }

            if(!begun){}
            else if (strcmp(lOpcode, cmpPseudoOps[1]) == 0) { fprintf(pOutfile, "0x%.4X\n", toNum(lArg1)); }
            else if (strcmp(lOpcode, cmpPseudoOps[2]) == 0) {
                return 0;
            }
            //control ops
            else if (isOpcode(lOpcode) > 25) {
                if (0 == strcmp(lOpcode, cmpOpcode[26])) {
                    if(*lArg1!=120){
                        printf("Invalid TRAP Vector Operand");
                        exit(4);
                    }
                    if(0!=strcmp(lArg2,"")){
                        printf("too many operands");
                        exit(4);
                    }
                    int d=toNum(lArg1);
                    if((d>0x0FF) || (d<0)){
                        printf("Invalid TRAP Vector");
                        exit(3);
                    }
                    fprintf(pOutfile,"0xF0");
                    fprintf(pOutfile,lArg1 + 1);
                    fprintf(pOutfile,"\n");
                }else {
                    if(0!=strcmp(lArg1,"")){
                        printf("too many operands");
                        exit(4);
                    }
                    if (0 == strcmp(lOpcode, cmpOpcode[27])) fprintf(pOutfile,"0xF025\n");
                    else if (0 == strcmp(lOpcode, cmpOpcode[28])) fprintf(pOutfile,"0xC1C0\n");
                    else if (0 == strcmp(lOpcode, cmpOpcode[29])) fprintf(pOutfile,"0x8000\n");
                    else if (0 == strcmp(lOpcode, cmpOpcode[30])) fprintf(pOutfile,"0x0000\n");
                }
            } else if ((isOpcode(lOpcode) < 7) && (isOpcode(lOpcode) != -1)) {
                int shf = 0;
                if (0 == strcmp(lOpcode, cmpOpcode[0])) sprintf(currOpCode, "0001");
                else if (0 == strcmp(lOpcode, cmpOpcode[1])) { sprintf(currOpCode, "0101"); }
                else if (0 == strcmp(lOpcode, cmpOpcode[2])) { sprintf(currOpCode, "1001"); }
                else if (0 == strcmp(lOpcode, cmpOpcode[3])) { sprintf(currOpCode, "1001"); }
                else if (0 == strcmp(lOpcode, cmpOpcode[4])) {
                    sprintf(currOpCode, "1101");
                    currOpCode[10] = currOpCode[11] = 48;
                    shf = 1;
                } else if (0 == strcmp(lOpcode, cmpOpcode[5])) {
                    sprintf(currOpCode, "1101");
                    currOpCode[10] = 48;
                    currOpCode[11] = 49;
                    shf = 1;
                } else if (0 == strcmp(lOpcode, cmpOpcode[6])) {
                    sprintf(currOpCode, "1101");
                    currOpCode[10] = currOpCode[11] = 49;
                    shf = 1;
                }
                //check if DR/SR are valid registers
                if(*lArg1!=114){
                    printf("Error: unexpected operand, %s\n",lArg1);
                    exit(4);
                }
                if(*lArg2!=114){
                    printf("Error: unexpected operand, %s\n",lArg2);
                    exit(4);
                }
                //populate DR/SR1 for all arithmetic ops
                int drNum = toNum(lArg1);
                int srNum = toNum(lArg2);
                populateDRSR(currOpCode, drNum, srNum);
                if (isOpcode(lOpcode) == 2) { //not command
                    if(strcmp("",lArg3)!=0){
                        exit(4);
                    }
                    for (int i = 10; i < 16; i++)currOpCode[i] = 49;
                }
                    //check whether SR2 or imm operand, only if not NOT command
                else if ((isOpcode(lOpcode) < 4) && (isOpcode(lOpcode) != -1)) {
                    populateOp3(currOpCode, lArg3);
                } else if (shf) {
                    populateShiftImm(currOpCode, lArg3);
                }
                bitwise=1;
            } else if (isOpcode(lOpcode) > 21) { //this means offset of 6 and is a load/store
                //"ldb", "ldw", "stb", "stw", //5 items
                bitwise=1;
                switch (isOpcode(lOpcode)) { //write the opcode in
                    default:
                        break;
                    case 22:
                        sprintf(currOpCode, "0010");
                        break;
                    case 23:
                        sprintf(currOpCode, "0110");
                        break;
                    case 24:
                        sprintf(currOpCode, "0011");
                        break;
                    case 25:
                        sprintf(currOpCode, "0111");
                        break;
                }
                populateDRSR(currOpCode,toNum(lArg1),toNum(lArg2));
                populateImmediate(currOpCode, (char *) toNum(lArg3), 6, 0);
            //operation ops
            } else if (isOpcode(lOpcode) > 9) {
                char* tmpPtr=lArg2;
                bitwise=1;
                if(isOpcode(lOpcode)==21){
                    sprintf(currOpCode, "1110");
                    populateDRSR(currOpCode,toNum(lArg1), 0);
                    if(strcmp(lArg3,"")!=0) exit(4);
                }
                else{
                    sprintf(currOpCode, "0000");
                    currOpCode[4]=currOpCode[5]=currOpCode[6]=48;
                    for(int i = 1;i<strlen(lOpcode);i++){ //clarify type of branch
                        if(lOpcode[i]==110){currOpCode[4]=49;}
                        if(lOpcode[i]==122){currOpCode[5]=49;}
                        if(lOpcode[i]==112){currOpCode[6]=49;}
                    }
                    if((currOpCode[4]==48) && (currOpCode[5]==48) && (currOpCode[6]==48)){
                        currOpCode[4]=currOpCode[5]=currOpCode[6]=49;
                    }
                    tmpPtr=lArg1;
                    if(strcmp(lArg2,"")!=0) exit(4);
                }
                //calculate offset
                if((*tmpPtr==35)||(*tmpPtr==120)) populateImmediate(currOpCode, tmpPtr, 9, 1);
                else{
                    //find label in symbol table
                    if(isLabel(tmpPtr)==-1){ //#error
                        printf("Error: Invalid label, %s\n",tmpPtr);
                        exit(1);
                    }
                    //write offset into command
                    populateImmediate(currOpCode, (char*) ((dAddress[isLabel(tmpPtr)]-currAddy-2)/2), 9, 0);
                }
            //control ops
            }else if((isOpcode(lOpcode)>6) && (isOpcode(lOpcode)<10)){
                bitwise=1;
                if(strcmp("",lArg2)!=0) exit(4);
                switch(isOpcode(lOpcode)){
                    default:break;
                    case 7://"jsrr", "jmp", "jsr"
                        sprintf(currOpCode,"0100");
                        break;
                    case 8:
                        sprintf(currOpCode,"1100");
                        break;
                    case 9:
                        sprintf(currOpCode,"0100");
                        break;
                }
                if(isOpcode(lOpcode)!=9){
                    if((*lArg1!=114)||(*lArg1<0)){
                        printf("Error: Invalid Operand, %s\n",lArg1);
                        exit(4);
                    }
                    int d = toNum(lArg1);
                    populateDRSR(currOpCode,0,d);
                    populateImmediate(currOpCode,0,6,0);
                }else{
                    if(isLabel(lArg1)==-1){ //#error
                        printf("Error: Invalid label, %s\n",lArg1);
                        exit(1);
                    }
                    int d = dAddress[isLabel(lArg1)];
                    populateImmediate(currOpCode, (char *) ((d - currAddy - 2) / 2), 11, 0);
                    currOpCode[4]=49;
                }
            }
            if(bitwise){
                char *command = b2x(currOpCode);
                char tmp[8];
                tmp[0] = 48;
                tmp[1] = 120;
                tmp[6] = '\n';
                tmp[7]=0;
                for (int i = 0; i < 4; i++)tmp[2 + i] = command[i];
                fprintf(pOutfile,tmp);
                free(command);
            }
            if (strcmp(lOpcode, cmpPseudoOps[0]) == 0) {
                fprintf(pOutfile,"0x%.4X\n", toNum(lArg1));
                begun = 1;
            }
            //pseudoOps (only .fill implemented)
            if(0 == strcmp(cmpPseudoOps[2], lOpcode)){
                return 0;
            }
            if(begun && (isOpcode(lOpcode) != -1)) currAddy+=2;
        }
    } while( lRet != DONE );
    printf("no .end in sight");
    exit(4);
}

void populateImmediate(char code[16], char *arg3, int bits, int isPointer){
    int d = arg3;
    if(isPointer) d=toNum(arg3);
     //#todo fix the case for neg nums out of range
    if((d>((1<<(bits-1))-1)) || (d<(-(1<<(bits-1))))){
        if(isPointer){
            printf("Error: Address out of range");
            exit(4);
        }else{
            printf("Error: Invalid immediate value out of range");
            exit(4);
        }

    }
    for(int i = 0; i<bits;i++){code[15-i]=(char)(48+(1&(d>>i)));}
}

void populateShiftImm(char code[16], char *arg3) {
    int d=toNum(arg3);
    if((d>7)||(d<0)){
        printf("Error: Immediate value out of range %s\n",arg3);
        exit(3);
    }
    populateImmediate(code,d,4,0);
}

int populateOp3(char* instruction, char* operand){
    int d;
    if((*operand)==114){
        d =atoi(operand+1);
        if((d>7)||(d<0)){
            printf("Error: Invalid Operand, %s\n",operand);
            exit(4);
        }
        instruction[10]=instruction[11]=instruction[12]=48; //set bit 5
        instruction[13]=(char) (48+((d&4)>>2));
        instruction[14]=(char) (48+((d&2)>>1));
        instruction[15]=(char) (48+(d&1));
        return 0;
    }else if((*operand)==35){ //check for invalid nums #todo
        d = toNum(operand); //check for neg numbers #todo
    }else if((*operand)==120){ //#todo check range
        d = (int) strtol(operand + 1, NULL, 16);
    }else{//#todo error throwing for invalid number
        printf("Error: Invalid Operand, %s\n",operand);
        exit(4);
    }
    instruction[10]=49; //write to the instruction
    if((d>15) || d<-16){
    //    exit(3);
    }
    populateImmediate(instruction,d,5,0);
    for(int i = 0; i<5;i++){
        instruction[15-i]=(char)(48+(1&(d>>i)));
    }
}

int populateDRSR(char* instruction, int d, int s){
    if((d>7) || (d<0) || (s>7) || (s<0)){
        printf("Error: Invalid Register Number");
        exit(4);
    }
    instruction[4]=(char) (48+((d&4)>>2));
    instruction[5]=(char) (48+((d&2)>>1));
    instruction[6]=(char) (48+(d&1));
    instruction[7]=(char) (48+((s&4)>>2));
    instruction[8]=(char) (48+((s&2)>>1));
    instruction[9]=(char) (48+(s&1));
}

int b2xUnit(char* bin);

char* b2x(char* bin){
    char hex[] = "0123456789ABCDEF";
    char tmp[4];
    char* out = malloc(sizeof(char)*4);
    for(int i = 0;i<16;i+=4){
        for(int j = 0;j<4;j++){
            tmp[j]=bin[i+j];
        }
        out[i/4]=hex[b2xUnit(tmp)];
    }
    return out;
}

int b2xUnit(char* bin){
    int out = 0;
    for(int i = 3;i>=0;i--){
        out|=(bin[i]-48)<<(3-i);
    }
    return out;
}

int toNum( char * pStr ){
    char * t_ptr;
    char * orig_pStr;
    int t_length,k;
    int lNum, lNeg = 0;
    long int lNumLong;

    orig_pStr = pStr;
    if( *pStr == '#' )				/* decimal */
    {
        pStr++;
        if( *pStr == '-' )				/* dec is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
            if (!isdigit(*t_ptr))
            {
                printf("Error: invalid decimal operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNum = atoi(pStr);
        if (lNeg)
            lNum = -lNum;

        return lNum;
    }
    else if( *pStr == 'x' )	/* hex     */
    {
        pStr++;
        if( *pStr == '-' )				/* hex is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++){
            if (!isxdigit(*t_ptr))
            {
                printf("Error: invalid hex operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
        lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
        if( lNeg )
            lNum = -lNum;
        return lNum;
    }
    else if( *pStr == 'r' ){
        int d=atoi(pStr+1);
        if((d>7) || (d<0)){
            printf("invalid reg");
            exit(4);
        }
        return atoi(pStr+1);
    }
    else
    {
        printf( "Error: invalid operand, %s\n", orig_pStr);
        exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
    }
}

int isOpcode(char *lPtr);

int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4){
    char* lRet, * lPtr;
    int i;
    if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
        return( DONE );
    for( i = 0; i < strlen( pLine ); i++ )
        pLine[i] = tolower( pLine[i] );
    /* convert entire line to lowercase */
    *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);
    /* ignore the comments */
    lPtr = pLine;
    while( *lPtr != ';' && *lPtr != '\0' && *lPtr != '\n' ) lPtr++;
    *lPtr = '\0';
    if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) return( EMPTY_LINE );
    if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) { /* found a label */
        *pLabel = lPtr;
        if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    }
    *pOpcode = lPtr;
    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    *pArg1 = lPtr;
    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    *pArg2 = lPtr;
    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    *pArg3 = lPtr;
    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    *pArg4 = lPtr;
    return OK;
}

// returns an id based on opcode, -1 if invalid
int isOpcode(char *lPtr) {
    for(int i = 0; i<31;i++){
        if(strcmp(lPtr,cmpOpcode[i])==0) return i;
    }
    return -1;
}

int isPseudoOpcode(char *lPtr) {
    for(int i = 0; i<3;i++){
        if(strcmp(lPtr,cmpPseudoOps[i])==0) return i;
    }
    return -1;
}

int isLabel(char *lPtr) {
    for(int i = 0; i<dIndex;i++){
        if(strcmp(lPtr,dLabel[i])==0) return i;
    }
    return -1;
}

int isShorthandOp(char* lPtr){
    for(int i = 0; i<4;i++){
        if(strcmp(lPtr,cmpShorthandOps[i])==0) return i;
    }
    return -1;
}
