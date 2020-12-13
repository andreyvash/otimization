#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "lp_lib.h"


typedef struct Maquina {
    int custo;
    int horas;
    int nTarefas;
    int *tarefas;

} Maquina;

void imprimeMaquinas(int nMaquinas, Maquina *maquinas)
{
    for(int i = 0; i < nMaquinas; i++)
    {
        printf("Custo e hora maquina%d:\n", i+1);
        printf("%d ", maquinas[i].custo);
        printf("%d\n", maquinas[i].horas);

    }

    for(int i = 0; i < nMaquinas; i++)
    {
        printf("Tarefas maquina%d:\n", i+1);
        printf("%d\n", maquinas[i].nTarefas);
        for(int j = 0; j < maquinas[i].nTarefas; j++)
        {
            printf("%d\n", maquinas[i].tarefas[j]);
        }
    }

}

void gerarFuncaoObjetivo(FILE *fp, int numMaq, int numTarefas, Maquina *maquinas){

    fprintf(fp, "min: ");

    for(int i = 0; i < numMaq; i ++)
    {
        for(int j = 0; j < numTarefas; j ++)
        {
            fprintf(fp, "+%dm%dt%d ", maquinas[i].custo, i+1,j+1);
        }
    }

    fprintf(fp, ";\n");
}

void gerarRestricoes(FILE *fp, int numMaquinas, int numTarefas, Maquina *maquinas, int *tarefas)
{

    fprintf(fp, "\n");
    for(int i = 0; i < numMaquinas; i ++)
    {
        for(int j = 0; j < numTarefas; j ++)
        {
            fprintf(fp, "m%dt%d >= 0;\n",i+1,j+1);
        }
    }

    for(int i = 0; i < numMaquinas; i ++)
    {
        for(int j = 0; j < numTarefas; j ++)
        {
            fprintf(fp, "m%dt%d <= %d;\n", i+1,j+1, maquinas[i].horas);
        }
    }

    fprintf(fp, "\n");

    for(int i = 0; i < numMaquinas; i ++)
    {
        for(int j = 0; j < numTarefas; j ++)
        {
            fprintf(fp, "+m%dt%d ", i+1,j+1);
        }
        fprintf(fp, "<= %d", maquinas[i].horas);
        fprintf(fp, ";\n");
    }

    for(int i = 0; i < numMaquinas; i ++)
    {
        for(int j = 0; j < numTarefas; j ++)
        {
            fprintf(fp, "+m%dt%d ", i+1,j+1);
        }
        fprintf(fp, "<= %d", maquinas[i].horas);
        fprintf(fp, ";\n");
    }
    int temTarefa = 0;
    for(int j = 0; j < numTarefas; j ++)
    {
        for(int i = 0; i < numMaquinas; i ++)
        {
            for(int k = 0; k < maquinas[i].nTarefas; k++)
            {
                if(j+1 == maquinas[i].tarefas[k])
                {
                    temTarefa = 1;
                }
            }

            if(temTarefa == 1)
            {
                fprintf(fp, "+m%dt%d ", i+1,j+1);
            }
            else
            {
               fprintf(fp, "+0m%dt%d ", i+1,j+1);
            }
            temTarefa = 0;         
        }

        fprintf(fp, "= %d", tarefas[j]);
        fprintf(fp, ";\n");
    }

}


int main (int argc, char **argv)
{  
    lprec *lp; 
    int nTarefas;
    int nMaquinas; 
    int *horasTarefas;
    Maquina *maquinas;
    
    /* le a entradada*/
    scanf("%d", &nTarefas);
    scanf("%d", &nMaquinas);

    horasTarefas = malloc( nTarefas * sizeof(int));

    for(int i = 0; i < nTarefas; i++)
    {
        scanf("%d", &horasTarefas[i]);
    }

    maquinas = malloc( nMaquinas * sizeof(Maquina));

    for(int i = 0; i < nMaquinas; i++)
    {
        scanf("%d", &maquinas[i].custo);
        scanf("%d", &maquinas[i].horas);
    }
    
    for(int i = 0; i < nMaquinas; i++)
    {
        scanf("%d", &maquinas[i].nTarefas);
        maquinas[i].tarefas = malloc (maquinas[i].nTarefas * sizeof(int));
        for(int j = 0; j < maquinas[i].nTarefas; j++)
        {
            scanf("%d", &maquinas[i].tarefas[j]);
        }
    }

    #ifdef DEBUG
        printf(" nTarefas %d  nMaquinas %d\n", nTarefas, nMaquinas);

        for(int i = 0; i < nTarefas; i++)
        {
            printf("horasTarefas %d\n", horasTarefas[i]);
        }

        imprimeMaquinas(nMaquinas, maquinas);
    #endif

    /* endregion*/

    /* cria um arquivo para ser lido pelo lpsolve*/

    FILE *fp;
    fp = fopen("lpteste.lp", "w+");
    gerarFuncaoObjetivo(fp, nMaquinas, nTarefas, maquinas);
    gerarRestricoes(fp, nMaquinas, nTarefas, maquinas, horasTarefas);

    fclose(fp);

    /* endregion*/

    
    /* Create a new LP model */
    lp = read_LP("lpteste.lp", IMPORTANT, "test model");
    if(lp == NULL) {
        fprintf(stderr, "Unable to create new LP model\n");
        return(1);
    }

    solve(lp);

    double *vars = malloc(nMaquinas*nTarefas*sizeof(double));
    get_variables(lp, vars);
    double custo = 0.0;
    //saida do programa

    for(int i = 0; i < nMaquinas; i++)
    {
        for(int j = 0; j < nTarefas; j++)
        {
            custo += vars[(i*nTarefas) + j] * maquinas[i].custo;
            printf("%g ", vars[(i*nTarefas) + j]);
        }
        printf("\n");
    }

    printf("%g\n", custo);

    delete_lp(lp);
    return(0);
}
