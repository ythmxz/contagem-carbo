#define FATOR_SENSIBILIDADE 45.0
#define GLICEMIA_ALVO 130.0

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void confirmar(void);
void salvarHistorico(int refeicao, int glicemia, int carboidratos, int insulina);
void menu(void);
void contarCarboidratos(void);
void visualizarHistorico(void);
int calcularInsulina(float proporcao, float glicemiaAtual, float carboidratos);
char *prompt(int code);

int main(void) {
    menu();
    return 0;
}

void confirmar() {
#ifdef _WIN32
#include <conio.h>
    getch();
#else
    system("read a");
#endif
}

FILE *historico = NULL;
char nomeArquivo[] = "historico_insulina.txt";

void salvarHistorico(int refeicao, int glicemia, int carboidratos, int insulina) {
    historico = fopen(nomeArquivo, "a");

    if (historico == NULL) {
        historico = fopen(nomeArquivo, "w");

        if (historico == NULL) {
            exit(1);
        }
    }

    char *nomeRefeicao;

    switch (refeicao) {
    case 1:
        nomeRefeicao = "CAFÉ";
        break;
    case 2:
        nomeRefeicao = "ALMOÇO";
        break;
    case 3:
        nomeRefeicao = "JANTA";
        break;
    default:
        nomeRefeicao = "N/A";
        break;
    }

    time_t atual;
    struct tm *horario;
    char hora[20], data[20];

    time(&atual);
    horario = localtime(&atual);
    strftime(data, sizeof(data), "%d/%m/%Y", horario);
    strftime(hora, sizeof(hora), "%H:%M", horario);

    char buffer[256];
    if (carboidratos == -1) {
        sprintf(buffer, "[%s - %s] - R: %s | G: %d mg/L | C: - | I: %d UI\n", data, hora, nomeRefeicao, glicemia, insulina);

    }

    else {
        sprintf(
            buffer, "[%s - %s] - R: %s | G: %d mg/L | C: %d g | I: %d UI\n", data, hora, nomeRefeicao, glicemia, carboidratos, insulina);
    }

    fprintf(historico, "%s", buffer);
    fclose(historico);
}

void menu() {
    int opcao = 0;

    while (1) {
        system("clear");
        printf("CONTAGEM DE CARBOIDRATOS\n");
        printf("\n1 - Calcular Insulina\n");
        printf("2 - Visualizar Histórico\n");
        printf("\n0 - Sair\n");
        printf("\n> ");

        scanf("%d", &opcao);

        switch (opcao) {
        case 0:
            exit(0);

        case 1:
            contarCarboidratos();
            break;

        case 2:
            visualizarHistorico();
            break;

        default:
            printf("\n%s", prompt(1));
            confirmar();
            break;
        }
    }
}

void contarCarboidratos() {
    int refeicao = 0;
    int proporcao = 0;

    while (proporcao == 0) {
        system("clear");
        printf("Escolha uma refeição:\n");
        printf("\n1 - Café da Manhã\n");
        printf("2 - Almoço\n");
        printf("3 - Janta\n");
        printf("\n0 - Voltar\n");
        printf("\n> ");

        scanf("%d", &refeicao);

        switch (refeicao) {
        case 0:
            return;

        case 1:
            proporcao = 15;
            break;

        case 2:
            proporcao = 15;
            break;

        case 3:
            proporcao = 30;
            break;

        default:
            printf("\n%s", prompt(1));
            confirmar();
            break;
        }
    }

    int glicemiaAtual = 0;

    do {
        system("clear");
        printf("Glicemia (mg/L): ");
        scanf("%d", &glicemiaAtual);

        if (glicemiaAtual < 0) {
            printf("\n\n%s", prompt(1));
            confirmar();
        }
    } while (glicemiaAtual < 0);

    int carboidratos = 0;

    do {
        system("clear");
        printf("Carboidratos (g): ");
        scanf("%d", &carboidratos);

        if (carboidratos < -1) {
            printf("\n%s", prompt(1));
            confirmar();
        }
    } while (carboidratos < -1);

    int insulina = calcularInsulina(proporcao, glicemiaAtual, carboidratos);

    salvarHistorico(refeicao, glicemiaAtual, carboidratos, insulina);

    system("clear");
    printf("RESUMO\n");
    printf("\nGlicemia: %d mg/L\n", glicemiaAtual);
    printf("Carboidratos: %d g\n", carboidratos);
    printf("\nInsulina: %d UI\n", insulina);

    printf("\n%s", prompt(0));
    confirmar();
}

void visualizarHistorico() {
    historico = fopen(nomeArquivo, "r");

    if (historico == NULL) {
        system("clear");
        printf("%s", prompt(2));
        confirmar();
        return;
    }

    fseek(historico, 0, SEEK_END);
    long tamanho = ftell(historico);
    rewind(historico);

    if (tamanho == 0) {
        fclose(historico);
        system("clear");
        printf("%s", prompt(2));
        confirmar();
        return;
    }

    system("clear");
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), historico) != NULL) {
        printf("%s", buffer);
    }

    fclose(historico);
    printf("\n%s", prompt(0));
    confirmar();
}

int calcularInsulina(float proporcao, float glicemiaAtual, float carboidratos) {
    float resultado = ((carboidratos / proporcao) + ((glicemiaAtual - GLICEMIA_ALVO) / FATOR_SENSIBILIDADE));

    if (resultado < 0) {
        return 0;
    }

    resultado = ((resultado - (int)resultado) > 0.5) ? (int)resultado + 1.0 : (int)resultado;
    return (int)resultado;
}

char *prompt(int code) {
    switch (code) {
    case 0:
        return "Pressione qualquer tecla...\n";

    case 1:
        return "Valor Inválido!\n";

    case 2:
        return "Histórico vazio!\n";
    }
}
