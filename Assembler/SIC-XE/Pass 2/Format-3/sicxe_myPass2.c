#include<stdio.h>
#include<Stdlib.h>
#include<string.h>

struct Instruction {
	char label[20];
	char opcode[20];
	char operand[50];
	char address[20];
} typedef INST;

INST tokenise(char line[]) {
	int count=0;
	INST inst;
	char *token1="";
	char *token2="";
	char *token3=NULL;
	char *token4=NULL;

	token1=strtok(line," ");
	token2=strtok(NULL," ");
	token3=strtok(NULL," ");
	token4=strtok(NULL," ");

	if(token2==NULL)
		count=1;

	if(token3==NULL && token2!=NULL)
		count=2;

	if(token3!=NULL && token4==NULL)
		count=3;

	if(token4!=NULL)
		count=4;

	if(count==1) {
		strcpy(inst.address, "");
		strcpy(inst.label, "");
		strcpy(inst.opcode, "");
		strcpy(inst.operand, "");
	}

	if(count==2) {
		strcpy(inst.address, token1);
		strcpy(inst.label, "");
		strcpy(inst.opcode, token2);
		strcpy(inst.operand, "");
	}

	if(count==3) {
		strcpy(inst.address, token1);
		strcpy(inst.label, "");
		strcpy(inst.opcode, token2);
		strcpy(inst.operand, token3);
	}

	if(count==4) {
		strcpy(inst.address, token1);
		strcpy(inst.label, token2);
		strcpy(inst.opcode, token3);
		strcpy(inst.operand, token4);
	}
	return inst;
}

char* search_opcode(char *a) {
	INST ins;
	char line[100];
	int address = 0, flag=0;
	char *ans=(char*)malloc(10*sizeof(char));
	FILE* ptr = fopen("optab.txt", "r");
	while(fgets(line, sizeof(line), ptr)) {
		line[strcspn(line, "\n")] = 0;
		ins = tokenise(line);
	//printf("opcode: %s, opnum : %s\n", ins.address, ins.opcode);
		if(strcmp(ins.address, a) == 0) {
			fclose(ptr);
			strcpy(ans, ins.opcode);
			return ans;
		}

	}
	fclose(ptr);
	return NULL;
}

char* search_symtab(char *a) {
	INST ins;
	char line[100];
	int address = 0, flag=0;
	char *ans=(char*)malloc(10*sizeof(char));
	FILE* ptr = fopen("symtab.txt", "r");
	while(fgets(line, sizeof(line), ptr)) {
		line[strcspn(line, "\n")] = 0;
		ins = tokenise(line);
		//printf("symbol: %s, value : %s\n", ins.address, ins.opcode);
		if(strcmp(ins.address, a) == 0)
        {
			fclose(ptr);
			strcpy(ans, ins.opcode);
			return ans;
		}

	}
	fclose(ptr);
	return NULL;
}

int decToBin(int n) {
  long long bin = 0;
  int rem, i = 1;

  while (n!=0) {
    rem = n % 2;
    n /= 2;
    bin += rem * i;
    i *= 10;
  }

  return bin;
}

char* pcRelative(char* opc,int type)
{

    char buffer[5];
    buffer[0] = opc[1];
    buffer[1] = 0;
    int x = strtol(buffer, NULL, 16);
    x = x|type;
    sprintf(buffer,"%x",x);
    opc[1] = buffer[0];

    return opc;

}

int main()
{

	FILE* f1 = fopen("intermediate.txt", "r");
	FILE* f2 = fopen("objectprog2.txt", "w+");
	FILE* f3 = fopen("output2.txt", "w+");

	int line_count=0;
	char line[100];
	char text[200];
	INST ins;
	int i=0;

	int len=4218;
	int textlen=0, flag=0;
	int startingAddress=0;
	char temp[100];
	//pcRelative("8a","101b");

	if(f1 == NULL) {
		printf("Unable to open input.txt file\n");
		exit -1;
	}
	if(f2 == NULL) {
		printf("Unable to open intermediate.txt file\n");
		exit -1;
	}

	if(f3 == NULL) {
		printf("Unable to open input.txt file\n");
		exit -1;
	}

	while(fgets(line, sizeof(line), f1))
	{
        int type=3;
        char *operand_address;
		line[strcspn(line, "\n")] = 0;
		line_count++;
		ins = tokenise(line);
		printf("%s\t%s\t%s\t%s\n",ins.address, ins.label, ins.opcode, ins.operand);
//		fprintf(f3, "%s %s %s %s\n",ins.address, ins.label, ins.opcode, ins.operand);

		if(line_count == 1) {
			if(strcmp(ins.opcode, "START") == 0) {
				fprintf(f3, "%s\t%s\t%s\t%s\n",ins.address, ins.label, ins.opcode, ins.operand);
				startingAddress = strtol(ins.operand, NULL, 16);
				fprintf(f2, "H^%6s^%6s^%06x\n",ins.address, ins.operand, len);
				strcpy(text, "T^00");
				strcat(text, ins.operand);
				strcat(text, "^--");
			} else {
				fprintf(f2, "H^      ^000000^%6x\n",len);
				strcpy(text, "T^000000^--");
//				fprintf(f3, "T^000000^--", );
			}
		}

		if(strcmp(ins.opcode, "END") == 0)
		{
			fprintf(f3, "%s\t%s\t%s\t%s\n",ins.address, ins.label, ins.opcode, ins.operand);
			if(textlen != 0)
			{
				char buffer[5];
				sprintf(buffer,"%x",textlen);
				if(strlen(buffer)==1)
				{
					text[10] = buffer[0];
				}
				else
				{
					text[9] = buffer[0];
					text[10] = buffer[1];
				}
				fprintf(f2, "%s\n",text);
			}
			fprintf(f2, "E^%x\n",startingAddress);
			break;
		}

		char* opc = search_opcode(ins.opcode);

		char obj[100];
		if(opc != NULL) {
			textlen += 3;
			strcat(obj, opc);
			//printf("opcode: %s , object code: %s\n", opc, obj);

			  if(strcmp(ins.operand, "") != 0) {
				flag = 0;
				for(i=0; i<strlen(ins.operand); i++)
                {
					if(ins.operand[i] == ',')
					{
						flag = 1;
						break;
					}
					temp[i] = ins.operand[i];
				}
				if(ins.operand[0]=='#')
                {
                    int t=1;
                    type=1;
                    for(t=1;t<strlen(ins.operand);t++)
                    {
                        temp[t-1]=ins.operand[t];
                    }
                    temp[t-1]='\0';

                }
                else if(ins.operand[0]=='@')
                {
                    int t=1;
                    type=2;
                    for(t=1;t<strlen(ins.operand);t++)
                    {
                        temp[t-1]=ins.operand[t];
                    }
                    temp[t-1]='\0';
                }
				temp[i]='\0';
				opc = search_symtab(temp);
				operand_address=search_symtab(temp);
				printf("\n address %s\n",operand_address);
				//strcpy( opc,operand_address);

				printf("symbolname: %s symbol value: %s\n",temp, opc);
				if(opc != NULL) {
                //	char ch = opc[0];
					if(flag == 1)
                    {
						char buffer[5];
						buffer[0] = opc[0];
    					buffer[1] = 0;
						int x = strtol(buffer, NULL, 16);
						x = 10;
						sprintf(buffer,"%x",x);
    					opc[0] = buffer[0];
					}
					else
                    {
                        char buffer[5];
						buffer[0] = opc[0];
    					buffer[1] = 0;
						int x = strtol(buffer, NULL, 16);
						x = 2;
						sprintf(buffer,"%x",x);
    					opc[0] = buffer[0];
                    }

                printf("--%s\t%s--",obj,opc);
                printf("TYPE = %d",type);
                strcpy( obj,pcRelative(obj, type));

                int pc = strtol(ins.address, NULL, 16);
                pc += 3;

                int add = strtol(operand_address, NULL, 16);
                int ans = add - pc;
                printf("\nans = %x", ans);
                char charans[4];
                sprintf(charans,"%03x",ans);
                char temp[5];
                temp[0] = '\0';
                if(strlen(charans) > 3)
                {

                    int j;
                    int i=2;
                    for(j=strlen(charans)-1;i>=0 ; j--, i--){
                        temp[i] = charans[j];
                    }
                }
                printf("\ncharans = %s",charans);
                printf("\ntemp = %s",temp);

                if(strlen(temp) == 0)
                {
                    int i=1;
                    for(i=1; i<4; i++)
                        opc[i] = charans[i-1];
                }
                else
                {
                    int i=1;
                    for(i=1; i<4; i++)
                        opc[i] = temp[i-1];
                }

                strcat(obj, opc);
				} else {
					strcat(obj, "0000");
					printf("Set error flag\n");
				}


			}
			else {
				strcat(obj, "0000");
			}

//			printf("object code: %s\n", obj);
//			obj[0] = '\0';
		}
		else if(strcmp(ins.opcode, "BYTE")==0) {
			if(ins.operand[0] == 'X') {
				printf("operand: %s\n", ins.operand);
				for(i=2 ; i<strlen(ins.operand)-1; i++)
					temp[i-2] = ins.operand[i];

				temp[i-2]='\0';
				strcat(obj, temp);
				textlen += ((strlen(ins.operand)-3)/2);
			} else if(ins.operand[0] == 'C') {
				for(i=2 ; i<strlen(ins.operand)-1; i++) {
					int x = (int)ins.operand[i];
					char buffer[20];
					itoa(x, buffer, 16);
					strcat(obj, buffer);
				}
				textlen += (strlen(ins.operand)-3);
			}
//			printf("object code: %s\n", obj);
//			obj[0] = '\0';
		}
		else if(strcmp(ins.opcode, "WORD") ==0)
		{
			textlen += 3;
			char buffer[10];
			int x = strtol(ins.operand, NULL, 10);
//			int x=30;
			sprintf(buffer,"%6x",x);
			for(i=0; i<strlen(buffer); i++)
			{
				if(buffer[i] == ' ')
					buffer[i] = '0';
			}
			strcat(obj, buffer);
//			printf("object code: %s\n", obj);
//			obj[0] = '\0';
		}
		else
		{
			fprintf(f3, "%s\t%s\t%s\t%s\n",ins.address, ins.label, ins.opcode, ins.operand);
			continue;
		}
		printf("object code: %s\n", obj);
        //		obj[0] = '\0';

		fprintf(f3, "%s\t%s\t%s\t%s\t%s\n",ins.address, ins.label, ins.opcode, ins.operand, obj);

		if(textlen <= 30)
		{
			printf("object code: %s\n", obj);
			printf("%s\n",obj);
			strcat(text, "^");
			strcat(text, obj);
//			fprintf(f3, "%s %s %s %s %s\n",ins.address, ins.label, ins.opcode, ins.operand, obj);
			obj[0] = '\0';
			printf("text record: %s\n", text);
			printf("text length: %d\n", textlen);
		}
		else
		{
			char buffer[5];
			textlen = textlen - strlen(obj)/2;
			printf("text record: %s", text);
//			itoa(textlen, buffer, 16);
			sprintf(buffer,"%x",textlen);
			if(strlen(buffer)==1)
			{
				text[9] = '0';
				text[10] = buffer[0];
			}
			else
			{
				text[9] = buffer[0];
				text[10] = buffer[1];
			}
			fprintf(f2, "%s\n",text);
			text[0] = '\0';
			strcpy(text, "T^00");
			strcat(text, ins.address);
			strcat(text, "^--");
			strcat(text, "^");
			strcat(text, obj);
			textlen = (strlen(obj))/2;
            //fprintf(f3, "%s %s %s %s %s\n",ins.address, ins.label, ins.opcode, ins.operand, obj);
			obj[0] = '\0';

		}


	}


	return 0;
}


