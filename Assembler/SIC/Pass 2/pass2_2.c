#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

char label[50];
char opcode[50];
char operand[50];
char symbol[50];
char value[10];
char mnemonic[50];
char operand_address[5];
char opcode_address[10];
char locctr[10];
char text_record[100];
char object_code[20];
char integer[20];
int length = 3027;
int cur_length = 0;
int is_last = 1;
int starting_address;
int STARTING_ADDR = 0;
int i = 0;


int check_indexed(){
    int flag = 0;
    char *p = strtok (operand, " ,");
    char *array[3];
    int len = 0;

    while (p != NULL)
    {
        array[len] = p;
        p = strtok (NULL, " ,");
        len++;
    }

    if((len == 2) ){
            if(((strcmp(array[1], "X") == 0)||(strcmp(array[1], "x") == 0)))
            {
                strcpy(operand, array[0]);
                flag = 1;
            }
            else
            {
                printf("Invalid addressing mode\n");
            }
    }

    return flag;
}

int search_symtab()
{
	FILE *fp5=fopen("SYMBOL_TABLE.txt","r");
    if(fp5== NULL)
        printf("Error Opening SYMBOL1.txt\n");

    int flag2 = 0;
    strcpy(operand_address, "0000");
    while(!feof(fp5))
    {
    	fscanf(fp5,"%s\t%s",symbol,value);
        if(strcmp(symbol,operand)==0)
        {
        	strcpy(operand_address, value);
            flag2 = 1;
        	break;
        }


    }

    fclose(fp5);
    if(!flag2)
       {
            printf("%s Undefined symbol\n", operand);
       }

    return flag2;
}

int search_optab()
{
	FILE *fp6=fopen("OPTAB1.txt","r");
    if(fp6== NULL)
        printf("Error Opening Optab.txt\n");

 	int flag3 = 0;
    strcpy(opcode_address, "0");
    while(!feof(fp6))
    {
    	fscanf(fp6,"%s\t%s",mnemonic,value);
        if(strcmp(mnemonic, opcode)==0)
        {
        	strcpy(opcode_address, value);
        	flag3 = 1;
        	break;
        }
    }

    fclose(fp6);
    return flag3;
}

void pass(){
    FILE *fp1 = fopen("Intermediate.txt", "r");
    FILE *fp2 = fopen("Output8.txt", "w");
    FILE *fp3 = fopen("ObjectProgram.txt", "w");

    if(fp1== NULL)
        printf("Error Opening Intermediate.txt\n");
    if(fp2== NULL)
        printf("Error Opening Output8.txt\n");
    if(fp3== NULL)
        printf("Error Opening ObjectProgram.txt\n");

    char space[]=" \t\r\n";
    int start_location;
    char line[100];
    int len;
    len = 100 * sizeof(char);

    while ((fgets(&line, &len, fp1)) != NULL)
    {
        int i = 0;
        strcpy(label, " ");
        strcpy(opcode, " ");
        strcpy(operand, " ");
        char *p = strtok (line, space);
        char *array[5];
        strcpy(object_code, "");

        while (p != NULL)
        {
            array[i] = p;
            p = strtok (NULL, space);
            i++;
        }
        if(i == 2){
            strcpy(locctr, array[0]);
            strcpy(opcode, array[1]);
        }
        else if(i == 3){
            strcpy(locctr, array[0]);
            strcpy(opcode, array[1]);
            strcpy(operand, array[2]);
        }
        else if(i == 4){
            strcpy(locctr, array[0]);
            strcpy(label, array[1]);
            strcpy(opcode, array[2]);
            strcpy(operand, array[3]);
        }

        if(strcmp(opcode, "END")==0)
            break;

        if(strcmp(opcode, "START")==0){
            fprintf(fp2, "%s\t%s\t%s\t%s\n", locctr, label, opcode, operand);
            STARTING_ADDR=starting_address = (int)strtol(operand, NULL, 16);
            fprintf(fp3,"H^%6s^%06x^%06d\n", label, starting_address, length);
            fprintf(fp3, "T^");
            fprintf(fp3, "%06x^", starting_address);
            continue;
        }

        if((!strcmp(label, " ")==0) || (!strcmp(opcode, " ")==0) || (!strcmp(operand, " ")==0)){
            if(search_optab()){
                if(!(strcmp(operand, " ") == 0)){
                    int is_indexed = check_indexed();

                    search_symtab();

                    if(is_indexed){
                        strcat(operand, ", X");
                        int num = (int)strtol(operand_address, NULL, 16);
                        num = num | (1 << 15);
                        sprintf(operand_address, "%04x", num);
                    }
                }
                else
                    strcpy(operand_address, "0000");

                strcpy(object_code, strcat(opcode_address, operand_address));
                fprintf(fp2, "%s\t%s\t%s\t%s\t%s\n", locctr, label, opcode, operand, object_code);

                cur_length = (int)strtol(locctr, NULL, 16) - starting_address;
            }
            else if((strcmp(opcode, "BYTE") == 0) || (strcmp(opcode, "byte") == 0) || (strcmp(opcode, "WORD") == 0) || (strcmp(opcode, "word") == 0)){

                if((strcmp(opcode, "WORD") == 0)||(strcmp(opcode, "word") == 0)){
                    strcpy(object_code, "");
                    sprintf(integer, "%06x", atoi(operand));
                    strcpy(object_code, integer);
                    fprintf(fp2, "%s\t%s\t%s\t%s\t%s\n", locctr, label, opcode, operand, object_code);


                }

                else{
                    fprintf(fp2, "%s\t%s\t%s\t%s\t", locctr, label, opcode, operand);


                    strcpy(object_code, "");
                    if(operand[0] == 'C' || operand[0] == 'c'){
                        for(i = 2; i < strlen(operand) - 1; i++){
                            sprintf(integer, "%x", operand[i]);
                            strcat(object_code, integer);
                        }
                        fprintf(fp2, "%s\n", object_code);


                    }

                    else if(operand[0] == 'X' || operand[0] == 'x') {
                        for(i = 2; i < strlen(operand) - 1; i++){
                            sprintf(integer, "0000%c", operand[i]);
                            strcat(object_code, integer);
                        }
                        fprintf(fp2, "%s\n", object_code);
                    }
                    else
                    {
                        printf("Invalid Byte Type\n");
                    }
                }
            }
           else{
                fprintf(fp2, "%s\t%s\t%s\t%s\n", locctr, label, opcode, operand);

            }
            if((strcmp(opcode, "RESW") != 0)||(strcmp(opcode, "RESB") != 0)||(strcmp(opcode, "resw") != 0)||(strcmp(opcode, "resb") != 0)){
            if(((int)strtol(locctr, NULL, 16) - starting_address) < 30){
             if(cur_length<=30){
                if(!(strcmp(object_code, "")) == 0){
                    strcat(text_record, "^");
                    strcat(text_record, object_code);
                }else if(is_last){
                    cur_length = (int)strtol(locctr, NULL, 16) - starting_address;
                    is_last = 0;
                }
             }
            }

            else{
                cur_length = (int)strtol(locctr, NULL, 16) - starting_address;
                fprintf(fp3, "%02x%s\n", cur_length, text_record);
                strcpy(text_record, "^");
                strcat(text_record, object_code);
                starting_address = (int)strtol(locctr, NULL, 16);
                fprintf(fp3, "T^");
                fprintf(fp3, "%06x^", starting_address);
                starting_address=(int)strtol(locctr, NULL, 16)-3;
                is_last = 1;
            }
        }

        }

    }

    fprintf(fp3, "%02x%s\n", cur_length, text_record);
    starting_address = (int)strtol(locctr, NULL, 16);

    // End record
    fprintf(fp3, "E^%06x\n", STARTING_ADDR);


    fclose(fp1);
    fclose(fp2);
    fclose(fp3);

}


void show_output(){
    FILE *fp8 = fopen("Output8.txt", "r");
    char locctr[50];

    if(fp8== NULL)
        printf("Error Opening output8.txt\n");

    char line[100];
    size_t len = 100 * sizeof(char);

    while ((fgets(&line, &len, fp8)) != NULL)
        printf("%s", line);

    fclose(fp8);

}

int main(){

	pass();
    show_output();
    FILE *fpf = fopen("ObjectProgram.txt","r");
    if(fpf == NULL)
        printf("Error Opening Object Program.txt\n");
    char line1[100];
    size_t len1 = 100*sizeof(char);

    while((fgets(&line1, &len1, fpf))!=NULL)
        printf("%s", line1);

    fclose(fpf);


	return 0;
}
