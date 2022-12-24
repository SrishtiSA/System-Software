#include<stdio.h>
#include<stdlib.h>
#include<string.h>

static int start_f = 0;

// Structure to hold components of instruction
struct instruction
{
    char *label;
    char *opcode;
    char *operand;
};


// Function to display an instruction
void displayInstruction(struct instruction inst)
{
    printf("%s %s %s\n",inst.label,inst.opcode,inst.operand);
}



// Function to tokenize instruction from a line (string) and return structure of instructure
struct instruction tokenizeInstruction(char line[])
{
    int count=0;
    struct instruction inst;
    char *token1=NULL;
    char *token2=NULL;
    char *token3=NULL;

    token1=strtok(line," ");
    token2=strtok(NULL," ");
    token3=strtok(NULL," ");

    if(token2==NULL)
        count=1;

    if(token3==NULL && token2!=NULL)
        count=2;

    if(token3!=NULL)
        count=3;

    if(count==1)
    {
        inst.label="";
        inst.opcode=token1;
        inst.operand="";
    }

    if(count==2)
    {
        inst.label="";
        inst.opcode=token1;
        inst.operand=token2;
    }

    if(count==3)
    {
        inst.label=token1;
        // if(!strcmp(inst.label,"(null)"))
        // {
        //     printf("\nScheme\n");
        // }
        inst.opcode=token2;
        inst.operand=token3;
    }
    return inst;
}

// Returning Address as Hexadecimal integer even though in struct it is a string?
void getAddress(struct instruction inst,int *address)
{
    static int read=0;  // read flag is initially 0, after reading address it is set to 1

    if(!strcmp(inst.opcode,"START")||!strcmp(inst.opcode,"start"))
    {
        if(inst.operand==NULL){
            *address=0;
            read=1;
            //printf("address = %d\n",address);
        }

        if(inst.operand!=NULL)
        {
            //*address=atoi(inst.operand);
            sscanf(inst.operand,"%X",address);
            read=1;
            start_f=1;
            //printf("address = %d\n",address);
        }
    }
    if(read==0)
    {
        *address=0;
        read=1;
    }
}


void addSYMTAB(int *address,FILE *fp3, struct instruction inst)
{
    // Check if label already exists

    rewind(fp3);  // Take file pointer to Start of File

    char temp;  // Temporary character variable
    char line[30];  // Temporary string to hold a line/instruction
    int i=0;

    while(!feof(fp3))
    {
        // Read a character
        fscanf(fp3,"%c",&temp);

        // Put it in a character array
        line[i]=temp;

        // Increment string index
        i++;

        if(temp==' ')
        {
            line[--i]='\0'; // Adding null terminator
            i=0;

            // Check if label names are same !strcmp(*temp_inst.opcode,*inst.label)
            if(!strcmp(inst.label,line))
            {
                printf("\nERROR: Label %s is repeated\n",inst.label);
                exit(5);
            }
        }

        if(temp=='\n')
        {
            i=0;
        }
    }

    // If code gets executed till here then
    // 1. The label we want to insert is a new one and does not exist in the SYMTAB
    // 2. FILE pointer fp3 has reached EOF

    // Adding new label in SYMTAB
    fprintf(fp3,"%s %x\n",inst.label,*address);

    return;
}

// This function is called first
/* Reads first line expecting "Start" opcode. Address is updated in LOCCTR.
Line counter is incremented and Character counter i is used and reset.
*/
int ProcessFirstLine(FILE *fp1,FILE *fp2,FILE *fp3,struct instruction temp_inst,int *address)
{
    char temp;
    char line[30];
    int i=0;
    int linecount=0;

    while(!feof(fp1)){

        // Read a character
        fscanf(fp1,"%c",&temp);

        // Put it in a character array
        line[i]=temp;

        // Increment string index
        i++;

        if(temp=='\n')
        {
            // string has been read till '\n' so string is "example\n"
            // But we need one line in one string variable - "example\0"
            line[--i]='\0'; // Adding null terminator
            i=0;

            temp_inst = tokenizeInstruction(line);

            getAddress(temp_inst,address);

            displayInstruction(temp_inst);
            if(start_f==1){
                // Don't call Update Address
                fprintf(fp2," %s %s %s\n",temp_inst.label,temp_inst.opcode,temp_inst.operand);
            }
            else{
                // Symbol Table
                if(temp_inst.label!="")
                {
                    addSYMTAB(address,fp3,temp_inst);
                }
                fprintf(fp2,"%x %s %s %s\n",*address,temp_inst.label,temp_inst.opcode,temp_inst.operand);
                // Update Address
                //here
            }


            linecount++;
            return linecount;  // Break from the loop after reading first line
        }
    }
    return 1;
}

int ProcessLines(FILE *fp1,FILE *fp2,FILE *fp3,struct instruction temp_inst,int *address)
{
    char temp;  // Temporary character variable
    char line[30];  // Temporary string to hold a line/instruction
    int i=0;
    int linecount = 0;

    while(!feof(fp1))
    {
        // Read a character
        fscanf(fp1,"%c",&temp);

        // Put it in a character array
        line[i]=temp;

        // Increment string index
        i++;

        // If the read character is '\n' then we have completed reading a line
        if(temp=='\n')
        {
            // string has been read till '\n' so string is "example\n"
            // But we need one line in one string variable - "example\0"
            line[--i]='\0'; // Adding null terminator
            i=0;

            temp_inst = tokenizeInstruction(line);

            if(!strcmp(temp_inst.opcode,"End"))
            {
                break;
            }

            displayInstruction(temp_inst);

            // Symbol Table
            if(temp_inst.label!="")
            {
                addSYMTAB(address,fp3,temp_inst);
            }

            fprintf(fp2,"%x %s %s %s\n",*address,temp_inst.label,temp_inst.opcode,temp_inst.operand);
             char code[10];
            char mnemonic[3];
            char length_byte[10];
            int flag=0;
             FILE *fp6;
             fp6=fopen("optab.txt","r");
            while (!feof(fp6)) {
            fscanf(fp6, "%s\t%s\t%s", code, mnemonic,length_byte);
            if (strcmp(temp_inst.opcode, code) == 0) {
                flag=1;
                break;
            }
            }
            fclose(fp6);
            int lb=(int)strtol(length_byte, NULL, 16);
            int flag2=0;

            if(temp_inst.opcode[0]=='+')
            {
                flag2=1;
            }

           if(flag==1)
           {
               *address=*address+lb;
           }
           else
           {
               if(strcmp(temp_inst.opcode,"WORD")==0||strcmp(temp_inst.opcode,"word")==0)
            {
                *address+=3;
            }
            else if(strcmp(temp_inst.opcode,"RESW")==0||strcmp(temp_inst.opcode,"resw")==0)
            {
                char oa[100];
                strcpy(oa,temp_inst.operand);
                int v=oa[0]-'0';
                printf("%d",v);
                *address=*address+3*v;
            }
            else if(strcmp(temp_inst.opcode,"RESB")==0||strcmp(temp_inst.opcode,"resb")==0)
            {
                char oa[100];
                strcpy(oa,temp_inst.operand);
                int v=oa[0]-'0';
                *address=*address+v;
            }
            else if(strcmp(temp_inst.opcode,"BYTE")==0||strcmp(temp_inst.opcode,"byte")==0||strcmp(temp_inst.opcode,"Byte")==0)
            {
                int v=strlen(temp_inst.operand);
                if(temp_inst.operand[0] == 'X'){
                *address=*address+(v-3)/2;
                 }
                 else if(temp_inst.operand[0] == 'C'){
                 *address=*address+(v-3);
            }
            }
            else if(flag2==1)
            {
                *address+=4;
            }
            else
            {
               printf("Opcode not found");
                return 0;
            }
           }

            linecount++;
        }
    }

    // Last Line
    line[--i]='\0';
    temp_inst = tokenizeInstruction(line);

    displayInstruction(temp_inst);

    // Symbol Table
    if(temp_inst.label!="")
    {
        addSYMTAB(address,fp3,temp_inst);
    }

    fprintf(fp2,"%x %s %s %s\n",*address,temp_inst.label,temp_inst.opcode,temp_inst.operand);
    linecount++;

    return linecount;
}


int main()
{
    FILE *fp1;  // Read file pointer
    FILE *fp2;  // Write file pointer
    FILE *fp3;  // SYMTAB file pointer
    FILE *fp4;  // OPTAB file pointer

    fp1=fopen("input.txt","r");
    fp2=fopen("output1.txt","w");
    fp3=fopen("SYMTAB1.txt","w+");
    fp4=fopen("optab.txt","r");


    // File opening Exceptions
    if(fp1==NULL){
        printf("  => Error opening the read file\n\n");
        return 1;
    }
    if(fp2==NULL){
        printf("  => Error opening the write file\n\n");
        return 2;
    }
    if(fp3==NULL){
        printf("  => Error opening the Symbol Table file\n\n");
        return 3;
    }
    if(fp4==NULL){
        printf("  => Error opening the Opcode Table file\n\n");
        return 4;
    }

    printf("  -Files opened- \n");

    printf("Contents of Read file is: \n");
    printf("--------------------------\n\n");

    struct instruction temp_inst;
    int address;
    int linecount;

    //printf("\nLabel Opcode Operand\n\n");

    // Read First Line to check for Start address
    linecount = ProcessFirstLine(fp1,fp2,fp3,temp_inst,&address);

    // Read Remaining Lines
    linecount += ProcessLines(fp1,fp2,fp3,temp_inst,&address);

    printf("\nAddress = Hex: %x, Decimal: %d\n",address,address);

    printf("\n\n--------------------------\n");
    printf("\nNumber of lines is %d\n",linecount);

    fclose(fp1);
    fclose(fp2);
    fclose(fp3);


    printf("\n  -Files closed- \n\n");

    return 0; // Execution Completed Successfully
}
