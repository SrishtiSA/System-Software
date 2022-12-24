 #include <stdio.h>
#include <Stdlib.h>
#include <string.h>

int locctr = 0;
int startingAddress = 0;
typedef struct Instruction
{
    char label[20];
    char opcode[20];
    char operand[50];
} INST;

typedef struct sym
{
    char name[30];
    int val;
} sym;

sym symtbl[20];
int symb = 0;
int fin = 0, res[10];
char op[10];

typedef struct literal
{
    char name[30], val[30];
    int length, add;
} lit;

lit lite[20];
int litb = 0;

typedef struct blocks
{
    char *blck;
    int loc, len;
} block;

block blo[10];
int blclines = 1;

INST tokenise(char line[])
{
    int count = 0;
    INST inst;
    char *token1 = "";
    char *token2 = NULL;
    char *token3 = "";

    token1 = strtok(line, " ");
    token2 = strtok(NULL, " ");
    token3 = strtok(NULL, " ");

    if (token2 == NULL)
        count = 1;

    if (token3 == NULL && token2 != NULL)
        count = 2;

    if (token3 != NULL)
        count = 3;

    if (count == 1)
    {
        strcpy(inst.label, "");
        strcpy(inst.opcode, token1);
        strcpy(inst.operand, "");
    }

    if (count == 2)
    {
        strcpy(inst.label, "");
        strcpy(inst.opcode, token1);
        strcpy(inst.operand, token2);
    }

    if (count == 3)
    {
        strcpy(inst.label, token1);
        strcpy(inst.opcode, token2);
        strcpy(inst.operand, token3);
    }
    return inst;
}

int search_opcode(char *a)
{
    INST ins;
    char line[100];
    int address = 0, flag = 0;
    char temp[20];
    FILE *ptr = fopen("optab.txt", "r");
    if (a[0] == '+')
    {
        int i = 1;
        for (i = 1; i < strlen(a); i++)
            temp[i - 1] = a[i];
        temp[i - 1] = '\0';
    }
    else
    {
        strcpy(temp, a);
    }
    while (fgets(line, sizeof(line), ptr))
    {
        line[strcspn(line, "\n")] = 0;
        ins = tokenise(line);
        if (strcmp(ins.opcode, temp) == 0)
        {
            fclose(ptr);
            return 1;
        }
    }
    fclose(ptr);
    return 0;
}

int search_register(char *a)
{
    FILE *ptr = fopen("registers.txt", "r");
    char line[100], temp[20];
    int i = 0;
    INST ins;
    for (i = 0; i < strlen(a); i++)
    {
        if (a[i] == ',')
            break;
        temp[i] = a[i];
    }
    temp[i] = '\0';

    while (fgets(line, sizeof(line), ptr))
    {
        line[strcspn(line, "\n")] = 0;
        ins = tokenise(line);
        if (strcmp(ins.opcode, temp) == 0)
        {
            fclose(ptr);
            return 1;
        }
    }
    fclose(ptr);
    return 0;
}

int searchsym(char line[])
{
    FILE *ptr = fopen("symtbl.txt", "r");
    char *ins;
    char line1[20];
    while (fgets(line1, sizeof(line1), ptr))
    {
        line1[strcspn(line1, "\n")] = 0;
        ins = strtok(line1, " ");
        if (strncmp(ins, line, 4) == 0)
        {
            fclose(ptr);
            ins = strtok(NULL, " ");
            return strtol(ins, NULL, 16);
            ;
        }
    }
    fclose(ptr);
    return 0;
}

int main()
{

    char line[100], temp[10][100];
    int label_count = 0, res = 0, equf, orgf;
    int i = 0, h = 0, u = 0;

    FILE *f1 = fopen("input.txt", "r");
    FILE *f2 = fopen("intermediate.txt", "w");
    FILE *f3 = fopen("block.txt", "w");

    if (f1 == NULL)
    {
        printf("Unable to open input.txt file\n");
        exit - 1;
    }
    if (f2 == NULL)
    {
        printf("Unable to open intermediate.txt file\n");
        exit - 1;
    }

    INST ins;
    int line_count = 0, dif = 0, ltf = 0, lttf = 0, blckf = 0;
    while (fgets(line, sizeof(line), f1))
    {
        line[strcspn(line, "\n")] = 0;
        line_count++;
        res = 0;
        ltf = 0;
        orgf = 0;
        equf = 0;
        blckf = 0;
        ins = tokenise(line);
        if (strcmp(ins.opcode, "END") == 0)
        {
            fprintf(f2, "%x %s %s %s\n", locctr, ins.label, ins.opcode, ins.operand);
            break;
        }

        if (strcmp(ins.opcode, "START") == 0)
        {
            locctr = strtol(ins.operand, NULL, 16);
            startingAddress = strtol(ins.operand, NULL, 16);
            blo[0].loc = startingAddress;
            blo[0].blck = "default";
            // printf("locctr : %X, starting address: %x\n", locctr, startingAddress);
            fprintf(f2, "%s %s %s\n", ins.label, ins.opcode, ins.operand);
            continue;
        }

        if (strcmp(ins.opcode, "BASE") == 0)
        {
            fprintf(f2, "     %s %s %s\n", ins.label, ins.opcode, ins.operand);
            continue;
        }

        if (strcmp(ins.opcode, "ORG") == 0)
        {
            orgf = 1;
            if (strtol(ins.operand, NULL, 16) < locctr)
            {
                printf("Warning LOCCTR could be repeated \n");
            }
            dif = dif + (locctr - startingAddress);
            locctr = strtol(ins.operand, NULL, 16);
            startingAddress = strtol(ins.operand, NULL, 16);
            fprintf(f2, "      %s %s\n", ins.opcode, ins.operand);
        }

        if (strcmp(ins.opcode, "EQU") == 0)
        {
            equf = 1;
            fprintf(f2, "      %s %s %s\n", ins.label, ins.opcode, ins.operand);
        }

        // if (strcmp(ins.opcode, "USE") == 0)
        // {
        //     blckf = 1;
        //     fprintf(f2, "      %s %s\n", ins.opcode, ins.operand);
        //     if (strcmp(ins.operand, "") != 0)
        //     {
        //         blckf = 0;
        //         printf("here\n");
        //         for (int j = 0; j < blclines; j++)
        //         {
        //             if (strcmp(blo[j].blck, ins.operand) == 0)
        //             {
        //                 blckf = 1;
        //                 blo[j].len += locctr - blo[j].loc;
        //                 locctr = blo[j].len;
        //                 break;
        //             }
        //         }
        //         if (blckf == 0)
        //         {
        //             strcpy(blo[blclines].blck, ins.operand);
        //             locctr = 0;
        //             blo[blclines].len = blo[blclines].loc = 0;
        //             blclines++;
        //         }
        //         printf("SYMBOL\n");
        //     }
        //     else
        //     {
        //         blo[0].len += locctr-blo[0].loc;
        //         locctr = blo[0].len;
        //         printf("%s", line);
        //         printf("NOT SYMBOL\n");
        //     }
        // }

        if (ins.operand[0] == '=')
        {
            lttf = 0;
            int j = 3;
            int m = 0;
            while (j < strlen(ins.operand) - 1)
            {
                lite[litb].val[m] = ins.operand[j];
                j++;
                m++;
            }
            strcpy(lite[litb].name, ins.operand);
            lite[litb].add = locctr;
            if (ins.operand[1] == 'X')
            {
                lite[litb].length = 1;
            }
            else if (ins.operand[1] == 'C')
            {

                lite[litb].length = (strlen(ins.operand) - 3);
            }
            litb++;
        }

        if (strcmp(ins.opcode, "LTORG") == 0)
        {
            lttf = 1;
            fprintf(f2, "     LTORG\n");
            ltf = 1;
            FILE *ptr = fopen("lit.txt", "a+");
            for (int i = 0; i < litb; i++)
            {
                lite[i].add = locctr;
                fprintf(f2, "%x * %s\n", locctr, lite[i].name);
                locctr = locctr + lite[i].length;
            }
            for (int i = 0; i < litb; i++)
            {
                fprintf(ptr, "%s %s %d %x \n", lite[i].name, lite[i].val, lite[i].length, lite[i].add);
            }
            fclose(ptr);
            litb = 0;
        }

        else
        {
            if (orgf == 0 && equf == 0 && ltf == 0 && blckf == 0)
            {
                fprintf(f2, "%x %s %s %s\n", locctr, ins.label, ins.opcode, ins.operand);
            }
        }

        if (strcmp(ins.label, "") != 0)
        {
            INST inst1;
            char line1[100];
            int address = 0, flag = 0;
            FILE *ptr = fopen("symtbl.txt", "a+");
            while (fgets(line1, sizeof(line1), ptr))
            {
                line1[strcspn(line1, "\n")] = 0;
                inst1 = tokenise(line1);
                if (strcmp(inst1.opcode, ins.label) == 0)
                {
                    printf("Error Duplicate symbol %s in line : %d\n", ins.label, line_count);
                    flag = 1;
                    fclose(ptr);
                    goto error;
                    break;
                }
            }
            if (flag == 0)
            {
                if (equf == 1)
                {
                    if (strcmp(ins.operand, "*") == 0)
                    {
                        fprintf(ptr, "%s %x\n", ins.label, locctr-3);
                    }
                    else
                    {
                        char o = '+';
                        h = 0, u = 0;
                        int p = 0;
                        while (p <= strlen(ins.operand))
                        {
                            if (p == strlen(ins.operand) || ins.operand[p] == '+' || ins.operand[p] == '-' || ins.operand[p] == '/' || ins.operand[p] == '*')
                            {
                                res = searchsym(temp[h]);
                                if (res == 0)
                                {
                                    printf("ERROR: No Symbol %s found in EQU \n", temp[h]);
                                }
                                if (o == '+')
                                {
                                    u = u + res;
                                }
                                if (o == '-')
                                {
                                    u = u - res;
                                }
                                if (o == '/')
                                {
                                    u = u / res;
                                }
                                if (o == '*')
                                {
                                    printf("ERROR: '*' found in equation \n");
                                }
                                o = ins.operand[p];
                                h++;
                                i = -1;
                            }
                            else
                            {
                                temp[h][i] = ins.operand[p];
                            }
                            i++;
                            p++;
                        }
                        fprintf(ptr, "%s %x\n", ins.label, u);
                    }
                }
                else
                {
                    fprintf(ptr, "%s %x\n", ins.label, locctr);
                }
                fclose(ptr);
            }
        }

        if (orgf == 0 && equf == 0 && ltf == 0 && blckf == 0)
        {
            if (search_opcode(ins.opcode) == 1)
            {
                if (ins.opcode[0] == '+')
                    locctr += 4;
                else if (ins.opcode[strlen(ins.opcode) - 1] == 'R')
                    locctr += 2;
                else
                    locctr += 3;
            }
            else if (strcmp(ins.opcode, "WORD") == 0)
            {
                locctr += 3;
            }
            else if (strcmp(ins.opcode, "RESW") == 0)
            {
                int hex = strtol(ins.operand, NULL, 10);
                locctr += (3 * hex);
            }
            else if (strcmp(ins.opcode, "RESB") == 0)
            {
                locctr = locctr + strtol(ins.operand, NULL, 10);
            }
            else if (strcmp(ins.opcode, "BYTE") == 0)
            {
                if (ins.operand[0] == 'X')
                {
                    locctr = locctr + ((strlen(ins.operand) - 3) / 2);
                }
                else if (ins.operand[0] == 'C')
                {
                    locctr = locctr + (strlen(ins.operand) - 3);
                }
            }
            else
            {
                printf("Invalid opcode %s in line %d\n", ins.opcode, line_count);
            }
        error:
            printf("");
        }
    }

    if (lttf == 0)
    {
        fprintf(f2, "     LTORG\n");
        FILE *ptr = fopen("lit.txt", "a+");
        for (int i = 0; i < litb; i++)
        {
            lite[i].add = locctr;
            fprintf(f2, "%x * %s\n", locctr, lite[i].name);
            locctr = locctr + lite[i].length;
        }
        for (int i = 0; i < litb; i++)
        {
            fprintf(ptr, "%s %s %d %x \n", lite[i].name, lite[i].val, lite[i].length, lite[i].add);
        }
        fclose(ptr);
        litb = 0;
    }

    fprintf(f3, "%s 0 %x %x", blo[0].blck, blo[0].loc, blo[0].len);
    for (int i = 1; i < blclines; i++)
    {
        fprintf(f3, "%s %d %x %x", blo[i].blck, i, blo[i - 1].loc + blo[i - 1].len, blo[i].len);
    }

    fclose(f1);
    fclose(f2);

    printf("Complete pass 1\n");
    return 0;
}
