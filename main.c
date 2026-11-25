#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =========================================================================
 * PROJETO INTEGRADO MULTIDISCIPLINAR II (PIM II) - UNIP
 * CURSO: CST em Análise e Desenvolvimento de Sistemas
 * TEMA: Compass.IA (Catálogo de Agentes de IA)
 * ========================================================================= */

#define MAX_AGENTES 50
#define TAM_NOME 81          /* Até 80 caracteres + '\0' */
#define TAM_INSTRUCOES 1001   /* Até 1.000 caracteres + '\0' */
#define ARQUIVO_AGENTES "agentes.txt"
#define ARQUIVO_AVALIACOES "avaliacoes.txt"

/* --- VETORES PARALELOS: DADOS DO AGENTE (agentes.txt) --- */
int ids[MAX_AGENTES];
char nomes[MAX_AGENTES][TAM_NOME];
char instrucoes[MAX_AGENTES][TAM_INSTRUCOES];

/* --- VETORES PARALELOS: DADOS DA AVALIAÇÃO (avaliacoes.txt) --- */
int ids_avaliacao[MAX_AGENTES];
float notas[MAX_AGENTES];

int total = 0; /* Quantidade de agentes cadastrados no momento */

/* =========================================================================
 * FUNÇÕES BÁSICAS DE LEITURA E LIMPEZA DE TECLADO
 * ========================================================================= */

/* Limpa o Enter (\n) que sobra no teclado após usar o scanf.
 * Retorna '\n' quando a linha acaba, ou EOF se a entrada terminar. */
int limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* Descarta o caractere lido */
    }
    return c;
}

/* Limpa a tela antes de mostrar o menu de novo.
 * Usa somente printf, que o programa já utiliza. */
void limpar_console(void) {
    printf("\033[H\033[2J\033[3J");
}

/* Lê texto com espaços (fgets) e retira a quebra de linha ('\n') do final.
 * Retorna 1 se leu algo, ou 0 se a entrada terminou. */
int ler_texto(char texto[], int tamanho) {
    if (fgets(texto, tamanho, stdin) != NULL) {
        int tam = strlen(texto);
        if (tam > 0 && texto[tam - 1] == '\n') {
            texto[tam - 1] = '\0'; /* Substitui a quebra de linha pelo fim de string */
        } else {
            int c = getchar();
            if (c != '\n' && c != EOF) {
                while (c != '\n' && c != EOF) {
                    c = getchar();
                }
                printf("\n[Aviso] O texto foi cortado no limite de %d caracteres.\n", tamanho - 1);
            }
        }
        return 1;
    }

    texto[0] = '\0';
    return 0;
}

/* Verifica se um ID já foi cadastrado para não permitir duplicados */
int id_ja_existe(int id_procurado) {
    for (int i = 0; i < total; i++) {
        if (ids[i] == id_procurado) {
            return 1; /* Retorna 1 se o ID já existir */
        }
    }
    return 0; /* Retorna 0 se o ID estiver livre */
}

/* =========================================================================
 * PERSISTÊNCIA EM DOIS ARQUIVOS (agentes.txt e avaliacoes.txt)
 * ========================================================================= */

/* Grava os dados separados nos dois arquivos conforme a modelagem (AGENTE e AVALIACAO).
 * Retorna 0 se gravou, ou 1 se houve falha. */
int salvar_arquivo(void) {
    FILE *arq_agentes = fopen(ARQUIVO_AGENTES, "w");
    FILE *arq_avaliacoes = fopen(ARQUIVO_AVALIACOES, "w");

    if (arq_agentes == NULL || arq_avaliacoes == NULL) {
        printf("\n[Erro] Nao foi possivel abrir os arquivos para salvar!\n");
        if (arq_agentes != NULL) fclose(arq_agentes);
        if (arq_avaliacoes != NULL) fclose(arq_avaliacoes);
        return 1;
    }

    for (int i = 0; i < total; i++) {
        /* 1. Grava no arquivo agentes.txt: Id_agente, Nome e Instrucoes */
        /* 2. Grava no arquivo avaliacoes.txt: Id_avaliacao, Nota e Id_agente (FK) */
        if (fprintf(arq_agentes, "%d\n", ids[i]) < 0 ||
            fprintf(arq_agentes, "%s\n", nomes[i]) < 0 ||
            fprintf(arq_agentes, "%s\n", instrucoes[i]) < 0 ||
            fprintf(arq_avaliacoes, "%d\n", ids_avaliacao[i]) < 0 ||
            fprintf(arq_avaliacoes, "%.2f\n", notas[i]) < 0 ||
            fprintf(arq_avaliacoes, "%d\n", ids[i]) < 0) {
            printf("\n[Erro] Falha ao gravar os dados nos arquivos!\n");
            fclose(arq_agentes);
            fclose(arq_avaliacoes);
            return 1;
        }
    }

    if (fclose(arq_agentes) == EOF || fclose(arq_avaliacoes) == EOF) {
        printf("\n[Erro] Falha ao finalizar a gravacao dos arquivos!\n");
        return 1;
    }

    return 0;
}

/* Descarta o resto de uma linha longa demais, usando somente fgets.
 * Retorna 1 se havia texto alem do limite, ou 0 se a linha cabia. */
int descartar_resto_linha(FILE *arquivo, const char *texto, int capacidade) {
    int tam = strlen(texto);
    if (tam == capacidade - 1 && texto[tam - 1] != '\n') {
        char descarte[TAM_INSTRUCOES];
        int cortou = 0;
        int td;

        if (fgets(descarte, sizeof(descarte), arquivo) == NULL) {
            return 0;
        }
        if (!(descarte[0] == '\n' && descarte[1] == '\0')) {
            cortou = 1;
        }
        td = strlen(descarte);
        if (td == 0 || descarte[td - 1] != '\n') {
            while (fgets(descarte, sizeof(descarte), arquivo) != NULL) {
                td = strlen(descarte);
                if (td > 0 && descarte[td - 1] == '\n') {
                    break;
                }
            }
        }
        return cortou;
    }
    return 0;
}

/* Lê os dados dos dois arquivos (agentes.txt e avaliacoes.txt) ao abrir o programa */
void carregar_arquivo(void) {
    FILE *arq_agentes = fopen(ARQUIVO_AGENTES, "r");
    if (arq_agentes == NULL) {
        /* Se o arquivo de agentes ainda não existir, o sistema começa vazio */
        return;
    }

    char linha_temp[TAM_INSTRUCOES];
    int avisos = 0;
    total = 0;

    /* 1. Carrega os dados da entidade AGENTE (agentes.txt) */
    while (total < MAX_AGENTES && fgets(linha_temp, sizeof(linha_temp), arq_agentes) != NULL) {
        if (descartar_resto_linha(arq_agentes, linha_temp, (int)sizeof(linha_temp))) {
            avisos++;
        }
        ids[total] = atoi(linha_temp);

        /* Lê o Nome */
        if (fgets(nomes[total], TAM_NOME, arq_agentes) == NULL) {
            avisos++;
            break;
        }
        if (descartar_resto_linha(arq_agentes, nomes[total], TAM_NOME)) {
            avisos++;
        }
        int tam_n = strlen(nomes[total]);
        if (tam_n > 0 && nomes[total][tam_n - 1] == '\n') {
            nomes[total][tam_n - 1] = '\0';
        }

        /* Lê as Instruções */
        if (fgets(instrucoes[total], TAM_INSTRUCOES, arq_agentes) == NULL) {
            avisos++;
            break;
        }
        if (descartar_resto_linha(arq_agentes, instrucoes[total], TAM_INSTRUCOES)) {
            avisos++;
        }
        int tam_i = strlen(instrucoes[total]);
        if (tam_i > 0 && instrucoes[total][tam_i - 1] == '\n') {
            instrucoes[total][tam_i - 1] = '\0';
        }

        int nome_vazio = 1;
        for (int c = 0; nomes[total][c] != '\0'; c++) {
            if (nomes[total][c] != ' ' && nomes[total][c] != '\t') {
                nome_vazio = 0;
                break;
            }
        }

        /* Ignora registro invalido (ID, duplicado ou nome vazio) sem alterar os validos */
        if (ids[total] <= 0 || id_ja_existe(ids[total]) || nome_vazio) {
            avisos++;
            continue;
        }

        /* Inicializa valores padrão da avaliação caso ainda não tenha avaliacoes.txt */
        ids_avaliacao[total] = total + 1;
        notas[total] = 0.0f;

        total++;
    }

    if (total == MAX_AGENTES && fgets(linha_temp, sizeof(linha_temp), arq_agentes) != NULL) {
        avisos++;
    }
    if (fclose(arq_agentes) == EOF) {
        avisos++;
    }

    /* 2. Carrega os dados da entidade AVALIACAO (avaliacoes.txt) e vincula pelo Id_agente */
    FILE *arq_avaliacoes = fopen(ARQUIVO_AVALIACOES, "r");
    if (arq_avaliacoes == NULL) {
        if (total > 0) {
            printf("\n[Aviso] Arquivo de avaliacoes nao encontrado. Notas iniciadas em 0.0.\n");
        }
    } else {
        while (fgets(linha_temp, sizeof(linha_temp), arq_avaliacoes) != NULL) {
            int id_aval_lido = atoi(linha_temp);

            if (fgets(linha_temp, sizeof(linha_temp), arq_avaliacoes) == NULL) {
                avisos++;
                break;
            }

            int a = 0;
            int b = strlen(linha_temp);
            int p;
            int nota_ok;
            int tem_digito = 0;
            int separador = 0;
            float nota_lida = 0.0f;

            if (b > 0 && linha_temp[b - 1] == '\n') {
                linha_temp[--b] = '\0';
            }
            while (linha_temp[a] == ' ' || linha_temp[a] == '\t') a++;
            while (b > a && (linha_temp[b - 1] == ' ' || linha_temp[b - 1] == '\t')) b--;

            nota_ok = (a < b);
            p = a;
            if (nota_ok && (linha_temp[p] == '+' || linha_temp[p] == '-')) p++;
            if (p == b) nota_ok = 0;
            for (; nota_ok && p < b; p++) {
                if (linha_temp[p] >= '0' && linha_temp[p] <= '9') {
                    tem_digito = 1;
                } else if ((linha_temp[p] == '.' || linha_temp[p] == ',') && separador == 0) {
                    separador = 1;
                    linha_temp[p] = '.';
                } else {
                    nota_ok = 0;
                }
            }
            if (tem_digito == 0) nota_ok = 0;
            if (nota_ok) {
                char guarda = linha_temp[b];
                linha_temp[b] = '\0';
                nota_lida = atof(linha_temp + a);
                linha_temp[b] = guarda;
            }

            if (fgets(linha_temp, sizeof(linha_temp), arq_avaliacoes) == NULL) {
                avisos++;
                break;
            }
            int id_agente_fk = atoi(linha_temp);

            if (!nota_ok || id_aval_lido <= 0 || nota_lida < 0.0f || nota_lida > 5.0f) {
                avisos++;
                continue;
            }

            /* Procura o agente correspondente (chave estrangeira Id_agente) */
            int achou = 0;
            for (int i = 0; i < total; i++) {
                if (ids[i] == id_agente_fk) {
                    ids_avaliacao[i] = id_aval_lido;
                    notas[i] = nota_lida;
                    achou = 1;
                    break;
                }
            }
            if (!achou) {
                avisos++;
            }
        }
        if (fclose(arq_avaliacoes) == EOF) {
            avisos++;
        }
    }

    if (avisos > 0) {
        printf("\n[Aviso] Alguns dados dos arquivos foram ignorados por estarem invalidos ou incompletos.\n");
    }
}

/* =========================================================================
 * CADASTRO E LISTAGEM GERAL
 * ========================================================================= */

void cadastrar_agente(void) {
    int id_digitado;
    float nota_digitada;

    if (total >= MAX_AGENTES) {
        printf("\n[Aviso] O catalogo esta cheio! Limite de %d agentes atingido.\n", MAX_AGENTES);
        return;
    }

    printf("\n--- CADASTRAR NOVO AGENTE ---\n");

    /* 1. Leitura e validação do ID */
    do {
        char linha[64];
        int ini = 0;
        int fim;
        int k;
        int valido;

        printf("Digite o ID (numero positivo): ");
        if (!ler_texto(linha, 64)) {
            printf("\n[Erro] Entrada encerrada. Cadastro cancelado.\n");
            return;
        }

        while (linha[ini] == ' ' || linha[ini] == '\t') ini++;
        fim = strlen(linha);
        while (fim > ini && (linha[fim - 1] == ' ' || linha[fim - 1] == '\t')) fim--;

        valido = (ini < fim);
        k = ini;
        if (valido && (linha[k] == '+' || linha[k] == '-')) k++;
        if (k == fim) valido = 0;
        for (; valido && k < fim; k++) {
            if (linha[k] < '0' || linha[k] > '9') valido = 0;
        }

        int estouro = 0;
        if (valido) {
            int p = ini;
            char limite[] = "2147483647";
            if (linha[ini] == '-') {
                limite[9] = '8';
            }
            if (linha[p] == '+' || linha[p] == '-') p++;
            while (p < fim - 1 && linha[p] == '0') p++;
            if (fim - p > 10) {
                estouro = 1;
            } else if (fim - p == 10) {
                for (int t = 0; t < 10; t++) {
                    if (linha[p + t] > limite[t]) {
                        estouro = 1;
                        break;
                    }
                    if (linha[p + t] < limite[t]) break;
                }
            }
        }

        if (!valido) {
            id_digitado = -1;
            printf("[Erro] Digite apenas numeros inteiros.\n");
            continue;
        }
        if (estouro) {
            id_digitado = -1;
            printf("[Erro] O ID informado e grande demais.\n");
            continue;
        }

        linha[fim] = '\0';
        id_digitado = atoi(linha + ini);

        if (id_digitado <= 0) {
            printf("[Erro] O ID deve ser maior que zero.\n");
        } else if (id_ja_existe(id_digitado)) {
            printf("[Erro] O ID %d ja existe! Escolha outro.\n", id_digitado);
        }
    } while (id_digitado <= 0 || id_ja_existe(id_digitado));

    /* 2. Leitura do Nome */
    int nome_vazio = 1;
    do {
        printf("Nome do Agente (max 80 caracteres): ");
        nome_vazio = 1;

        if (!ler_texto(nomes[total], TAM_NOME)) {
            printf("\n[Erro] Entrada encerrada. Cadastro cancelado.\n");
            return;
        }
        for (int c = 0; nomes[total][c] != '\0'; c++) {
            if (nomes[total][c] != ' ' && nomes[total][c] != '\t') {
                nome_vazio = 0;
                break;
            }
        }
        if (nome_vazio) {
            printf("[Erro] O nome nao pode ficar vazio.\n");
        }
    } while (nome_vazio);

    /* 3. Leitura das Instruções */
    int instrucao_vazia = 1;
    do {
        printf("Script / Instrucoes (max 1000 caracteres):\n");
        instrucao_vazia = 1;

        if (!ler_texto(instrucoes[total], TAM_INSTRUCOES)) {
            printf("\n[Erro] Entrada encerrada. Cadastro cancelado.\n");
            return;
        }
        for (int c = 0; instrucoes[total][c] != '\0'; c++) {
            if (instrucoes[total][c] != ' ' && instrucoes[total][c] != '\t') {
                instrucao_vazia = 0;
                break;
            }
        }
        if (instrucao_vazia) {
            printf("[Erro] As instrucoes nao podem ficar vazias.\n");
        }
    } while (instrucao_vazia);

    /* 4. Leitura e validação da Nota (0.0 a 5.0) */
    do {
        char linha[64];
        int ini = 0;
        int fim;
        int k;
        int valido;
        int digitos = 0;
        int ponto = 0;

        printf("Nota do Agente (de 0.0 a 5.0): ");
        if (!ler_texto(linha, 64)) {
            printf("\n[Erro] Entrada encerrada. Cadastro cancelado.\n");
            return;
        }

        while (linha[ini] == ' ' || linha[ini] == '\t') ini++;
        fim = strlen(linha);
        while (fim > ini && (linha[fim - 1] == ' ' || linha[fim - 1] == '\t')) fim--;

        valido = (ini < fim);
        k = ini;
        if (valido && (linha[k] == '+' || linha[k] == '-')) k++;
        if (k == fim) valido = 0;
        for (; valido && k < fim; k++) {
            if (linha[k] >= '0' && linha[k] <= '9') {
                digitos = 1;
            } else if ((linha[k] == '.' || linha[k] == ',') && ponto == 0) {
                ponto = 1;
                linha[k] = '.';
            } else {
                valido = 0;
            }
        }
        if (digitos == 0) valido = 0;

        if (!valido) {
            nota_digitada = -1.0f;
            printf("[Erro] Digite um numero valido para a nota.\n");
            continue;
        }

        linha[fim] = '\0';
        nota_digitada = atof(linha + ini);

        if (nota_digitada < 0.0f || nota_digitada > 5.0f) {
            printf("[Erro] A nota deve estar entre 0.0 e 5.0!\n");
        }
    } while (nota_digitada < 0.0f || nota_digitada > 5.0f);

    /* Guarda as informações na posição atual do vetor */
    ids[total] = id_digitado;
    /* Gera o Id_avaliacao sequencial automatico (1, 2, 3...) como no IDENTITY(1,1) do SQL */
    if (total == 0) {
        ids_avaliacao[total] = 1;
    } else {
        int maior = ids_avaliacao[0];
        for (int k = 1; k < total; k++) {
            if (ids_avaliacao[k] > maior) {
                maior = ids_avaliacao[k];
            }
        }
        ids_avaliacao[total] = maior + 1;
    }
    notas[total] = nota_digitada;
    total++;

    /* Salva imediatamente nos dois arquivos (agentes.txt e avaliacoes.txt) */
    if (salvar_arquivo() == 0) {
        printf("\n[Sucesso] Agente cadastrado e gravado com sucesso!\n");
    }
}

void listar_agentes(void) {
    if (total == 0) {
        printf("\nNenhum agente cadastrado no catalogo.\n");
        return;
    }

    printf("\n============================================================\n");
    printf("                  CATALOGO DE AGENTES DE IA                 \n");
    printf("============================================================\n");
    for (int i = 0; i < total; i++) {
        printf("ID: %d | Nome: %s | Nota: %.2f/5.0\n", ids[i], nomes[i], notas[i]);
        printf("Instrucoes: %s\n", instrucoes[i]);
        printf("------------------------------------------------------------\n");
    }
    printf("Total de registros ativos: %d de %d\n", total, MAX_AGENTES);
}

/* =========================================================================
 * DIFERENCIAL DE NEGÓCIO: FILTRO POR NOTA MÍNIMA
 * ========================================================================= */

void filtrar_por_nota_minima(void) {
    float corte;
    int encontrados = 0;

    if (total == 0) {
        printf("\nO catalogo esta vazio.\n");
        return;
    }

    printf("\n--- FILTRO POR NOTA MINIMA (DIFERENCIAL) ---\n");
    printf("Digite a nota minima desejada (0.0 a 5.0): ");
    {
        char linha[64];
        int ini = 0;
        int fim;
        int k;
        int valido;
        int digitos = 0;
        int ponto = 0;

        if (!ler_texto(linha, 64)) {
            printf("\n[Erro] Entrada encerrada.\n");
            return;
        }

        while (linha[ini] == ' ' || linha[ini] == '\t') ini++;
        fim = strlen(linha);
        while (fim > ini && (linha[fim - 1] == ' ' || linha[fim - 1] == '\t')) fim--;

        valido = (ini < fim);
        k = ini;
        if (valido && (linha[k] == '+' || linha[k] == '-')) k++;
        if (k == fim) valido = 0;
        for (; valido && k < fim; k++) {
            if (linha[k] >= '0' && linha[k] <= '9') {
                digitos = 1;
            } else if ((linha[k] == '.' || linha[k] == ',') && ponto == 0) {
                ponto = 1;
                linha[k] = '.';
            } else {
                valido = 0;
            }
        }
        if (digitos == 0) valido = 0;

        if (!valido) {
            printf("[Erro] Valor invalido. Digite um numero de 0.0 a 5.0.\n");
            return;
        }

        linha[fim] = '\0';
        corte = atof(linha + ini);
    }

    if (corte < 0.0f || corte > 5.0f) {
        printf("[Erro] A nota deve estar entre 0.0 e 5.0!\n");
        return;
    }

    printf("\nAgentes com nota igual ou maior que %.2f:\n", corte);
    for (int i = 0; i < total; i++) {
        if (notas[i] >= corte) {
            printf("- ID: %d | Nome: %s | Nota: %.2f\n", ids[i], nomes[i], notas[i]);
            printf("  Script: %s\n", instrucoes[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum agente localizado com essa nota de corte.\n");
    }
}

/* =========================================================================
 * SUBMENU, EDIÇÃO E EXCLUSÃO (CONFORME EXIGÊNCIA DA BUSCA)
 * ========================================================================= */

/* Exibe a ficha completa do agente */
void exibir_ficha_agente(int pos) {
    printf("\n------------------------------------------------------------\n");
    printf("FICHA DO AGENTE ENCONTRADO\n");
    printf("ID:          %d\n", ids[pos]);
    printf("Nome:        %s\n", nomes[pos]);
    printf("Nota:        %.2f de 5.0\n", notas[pos]);
    printf("Instrucoes:  %s\n", instrucoes[pos]);
    printf("------------------------------------------------------------\n");
}

/* Permite editar Nome, Instruções e Nota */
void editar_agente(int pos) {
    float nova_nota;
    char novo_nome[TAM_NOME];
    char novas_instrucoes[TAM_INSTRUCOES];

    printf("\n--- EDITANDO O AGENTE (ID: %d) ---\n", ids[pos]);

    int nome_vazio = 1;
    do {
        printf("Digite o novo Nome: ");
        nome_vazio = 1;
        if (!ler_texto(novo_nome, TAM_NOME)) {
            printf("\n[Erro] Entrada encerrada. Edicao cancelada.\n");
            return;
        }
        for (int c = 0; novo_nome[c] != '\0'; c++) {
            if (novo_nome[c] != ' ' && novo_nome[c] != '\t') {
                nome_vazio = 0;
                break;
            }
        }
        if (nome_vazio) {
            printf("[Erro] O nome nao pode ficar vazio.\n");
        }
    } while (nome_vazio);

    int instrucao_vazia = 1;
    do {
        printf("Digite as novas Instrucoes:\n");
        instrucao_vazia = 1;
        if (!ler_texto(novas_instrucoes, TAM_INSTRUCOES)) {
            printf("\n[Erro] Entrada encerrada. Edicao cancelada.\n");
            return;
        }
        for (int c = 0; novas_instrucoes[c] != '\0'; c++) {
            if (novas_instrucoes[c] != ' ' && novas_instrucoes[c] != '\t') {
                instrucao_vazia = 0;
                break;
            }
        }
        if (instrucao_vazia) {
            printf("[Erro] As instrucoes nao podem ficar vazias.\n");
        }
    } while (instrucao_vazia);

    do {
        char linha[64];
        int ini = 0;
        int fim;
        int k;
        int valido;
        int digitos = 0;
        int ponto = 0;

        printf("Digite a nova Nota (0.0 a 5.0): ");
        if (!ler_texto(linha, 64)) {
            printf("\n[Erro] Entrada encerrada. Edicao cancelada.\n");
            return;
        }

        while (linha[ini] == ' ' || linha[ini] == '\t') ini++;
        fim = strlen(linha);
        while (fim > ini && (linha[fim - 1] == ' ' || linha[fim - 1] == '\t')) fim--;

        valido = (ini < fim);
        k = ini;
        if (valido && (linha[k] == '+' || linha[k] == '-')) k++;
        if (k == fim) valido = 0;
        for (; valido && k < fim; k++) {
            if (linha[k] >= '0' && linha[k] <= '9') {
                digitos = 1;
            } else if ((linha[k] == '.' || linha[k] == ',') && ponto == 0) {
                ponto = 1;
                linha[k] = '.';
            } else {
                valido = 0;
            }
        }
        if (digitos == 0) valido = 0;

        if (!valido) {
            nova_nota = -1.0f;
            printf("[Erro] Digite uma nota valida.\n");
            continue;
        }

        linha[fim] = '\0';
        nova_nota = atof(linha + ini);

        if (nova_nota < 0.0f || nova_nota > 5.0f) {
            printf("[Erro] A nota deve estar entre 0.0 e 5.0!\n");
        }
    } while (nova_nota < 0.0f || nova_nota > 5.0f);

    strcpy(nomes[pos], novo_nome);
    strcpy(instrucoes[pos], novas_instrucoes);
    notas[pos] = nova_nota;

    if (salvar_arquivo() == 0) {
        printf("\n[Sucesso] Registro atualizado e salvo!\n");
    }
}

/* Exclui o agente deslocando os próximos registros para a esquerda (Shift) */
int excluir_agente(int pos) {
    char confirmacao;

    printf("\nTem certeza que deseja excluir o agente '%s' (ID: %d)? (S/N): ", nomes[pos], ids[pos]);
    {
        char resposta[16];
        int i = 0;

        if (!ler_texto(resposta, 16)) {
            printf("\n[Erro] Confirmacao invalida. Operacao cancelada.\n");
            return 0;
        }
        int fim_resp = strlen(resposta);
        while (resposta[i] == ' ' || resposta[i] == '\t') i++;
        while (fim_resp > i && (resposta[fim_resp - 1] == ' ' || resposta[fim_resp - 1] == '\t')) {
            fim_resp--;
        }
        if (fim_resp - i != 1) {
            printf("\n[Erro] Confirmacao invalida. Operacao cancelada.\n");
            return 0;
        }
        confirmacao = resposta[i];
        if (confirmacao != 'S' && confirmacao != 's' && confirmacao != 'N' && confirmacao != 'n') {
            printf("\n[Erro] Confirmacao invalida. Operacao cancelada.\n");
            return 0;
        }
    }

    if (confirmacao == 'S' || confirmacao == 's') {
        /* Puxa todos os dados das posições da frente para trás */
        for (int j = pos; j < total - 1; j++) {
            ids[j] = ids[j + 1];
            strcpy(nomes[j], nomes[j + 1]);
            strcpy(instrucoes[j], instrucoes[j + 1]);
            ids_avaliacao[j] = ids_avaliacao[j + 1];
            notas[j] = notas[j + 1];
        }
        total--; /* Diminui a contagem total de agentes */

        limpar_console();
        if (salvar_arquivo() == 0) {
            printf("\n[Sucesso] Agente excluido com sucesso!\n");
        }
        return 1; /* Excluiu com sucesso */
    }

    printf("\nOperacao de exclusao cancelada.\n");
    return 0;
}

/* Submenu que se abre obrigatoriamente após localizar um agente por busca */
void submenu_agente(int pos) {
    int sub_opcao;

    do {
        exibir_ficha_agente(pos);
        printf("\nO que deseja fazer?\n");
        printf("1. Editar dados deste agente\n");
        printf("2. Excluir este agente\n");
        printf("0. Voltar ao menu principal\n");
        printf("Escolha: ");
        {
            char linha[64];
            int ini = 0;
            int fim;
            int k;
            int valido;

            if (!ler_texto(linha, 64)) {
                printf("\n[Erro] Entrada encerrada.\n");
                return;
            }

            while (linha[ini] == ' ' || linha[ini] == '\t') ini++;
            fim = strlen(linha);
            while (fim > ini && (linha[fim - 1] == ' ' || linha[fim - 1] == '\t')) fim--;

            valido = (ini < fim);
            k = ini;
            if (valido && (linha[k] == '+' || linha[k] == '-')) k++;
            if (k == fim) valido = 0;
            for (; valido && k < fim; k++) {
                if (linha[k] < '0' || linha[k] > '9') valido = 0;
            }

            int estouro = 0;
            if (valido) {
                int p = ini;
                char limite[] = "2147483647";
                if (linha[ini] == '-') limite[9] = '8';
                if (linha[p] == '+' || linha[p] == '-') p++;
                while (p < fim - 1 && linha[p] == '0') p++;
                if (fim - p > 10) {
                    estouro = 1;
                } else if (fim - p == 10) {
                    for (int t = 0; t < 10; t++) {
                        if (linha[p + t] > limite[t]) {
                            estouro = 1;
                            break;
                        }
                        if (linha[p + t] < limite[t]) break;
                    }
                }
            }

            if (!valido || estouro) {
                sub_opcao = -1;
                printf("\n[Erro] Digite apenas numeros inteiros.\n");
                continue;
            }

            linha[fim] = '\0';
            sub_opcao = atoi(linha + ini);
        }

        switch (sub_opcao) {
            case 1:
                editar_agente(pos);
                break;
            case 2:
                if (excluir_agente(pos)) {
                    return; /* Se excluiu, fecha o submenu imediatamente */
                }
                break;
            case 0:
                printf("\nVoltando ao menu principal...\n");
                break;
            default:
                printf("\nOpcao invalida. Digite 1, 2 ou 0.\n");
                break;
        }
    } while (sub_opcao != 0);
}

/* =========================================================================
 * OPERAÇÕES DE BUSCA
 * ========================================================================= */

void buscar_por_id(void) {
    int id_busca;

    if (total == 0) {
        printf("\nO catalogo esta vazio.\n");
        return;
    }

    printf("\nDigite o ID que deseja pesquisar: ");
    {
        char linha[64];
        int ini = 0;
        int fim;
        int k;
        int valido;

        if (!ler_texto(linha, 64)) {
            printf("\n[Erro] Entrada encerrada.\n");
            return;
        }

        while (linha[ini] == ' ' || linha[ini] == '\t') ini++;
        fim = strlen(linha);
        while (fim > ini && (linha[fim - 1] == ' ' || linha[fim - 1] == '\t')) fim--;

        valido = (ini < fim);
        k = ini;
        if (valido && (linha[k] == '+' || linha[k] == '-')) k++;
        if (k == fim) valido = 0;
        for (; valido && k < fim; k++) {
            if (linha[k] < '0' || linha[k] > '9') valido = 0;
        }

        int estouro = 0;
        if (valido) {
            int p = ini;
            char limite[] = "2147483647";
            if (linha[ini] == '-') limite[9] = '8';
            if (linha[p] == '+' || linha[p] == '-') p++;
            while (p < fim - 1 && linha[p] == '0') p++;
            if (fim - p > 10) {
                estouro = 1;
            } else if (fim - p == 10) {
                for (int t = 0; t < 10; t++) {
                    if (linha[p + t] > limite[t]) {
                        estouro = 1;
                        break;
                    }
                    if (linha[p + t] < limite[t]) break;
                }
            }
        }

        if (!valido) {
            printf("[Erro] ID invalido. Digite apenas numeros inteiros.\n");
            return;
        }
        if (estouro) {
            printf("[Erro] O ID informado e grande demais.\n");
            return;
        }

        linha[fim] = '\0';
        id_busca = atoi(linha + ini);
    }

    if (id_busca <= 0) {
        printf("[Erro] O ID deve ser maior que zero.\n");
        return;
    }

    /* Procura o ID no vetor */
    for (int i = 0; i < total; i++) {
        if (ids[i] == id_busca) {
            submenu_agente(i); /* Encontrou: abre o submenu */
            return;
        }
    }

    printf("\n[Aviso] Nenhum agente encontrado com o ID %d.\n", id_busca);
}

void buscar_por_nome(void) {
    char termo[TAM_NOME];

    if (total == 0) {
        printf("\nO catalogo esta vazio.\n");
        return;
    }

    printf("\nDigite o nome (ou parte do nome) do agente: ");
    if (!ler_texto(termo, TAM_NOME)) {
        printf("\n[Erro] Entrada encerrada.\n");
        return;
    }
    int termo_vazio = 1;
    for (int c = 0; termo[c] != '\0'; c++) {
        if (termo[c] != ' ' && termo[c] != '\t') {
            termo_vazio = 0;
            break;
        }
    }
    if (termo_vazio) {
        printf("[Erro] Informe um nome para a busca.\n");
        return;
    }

    /* Procura o texto em todos os nomes, sem diferenciar maiusculas */
    int posicoes[MAX_AGENTES];
    int qtd = 0;
    int tam_termo = strlen(termo);
    for (int i = 0; i < total; i++) {
        int tam_nome = strlen(nomes[i]);
        int achou = 0;
        if (tam_termo <= tam_nome) {
            for (int ini = 0; ini + tam_termo <= tam_nome; ini++) {
                int igual = 1;
                for (int c = 0; c < tam_termo; c++) {
                    char ca = nomes[i][ini + c];
                    char cb = termo[c];
                    if (ca >= 'A' && ca <= 'Z') ca = (char)(ca - 'A' + 'a');
                    if (cb >= 'A' && cb <= 'Z') cb = (char)(cb - 'A' + 'a');
                    if (ca != cb) {
                        igual = 0;
                        break;
                    }
                }
                if (igual) {
                    achou = 1;
                    break;
                }
            }
        }
        if (achou) {
            posicoes[qtd] = i;
            qtd++;
        }
    }

    if (qtd == 0) {
        printf("\n[Aviso] Nenhum agente encontrado contendo '%s'.\n", termo);
        return;
    }
    if (qtd == 1) {
        submenu_agente(posicoes[0]);
        return;
    }

    printf("\nForam encontrados %d agentes:\n", qtd);
    for (int i = 0; i < qtd; i++) {
        int p = posicoes[i];
        printf("- ID: %d | Nome: %s | Nota: %.2f\n", ids[p], nomes[p], notas[p]);
    }

    printf("Digite o ID do agente desejado: ");
    {
        char linha[64];
        int ini = 0;
        int fim;
        int k;
        int valido;
        int id_escolhido;

        if (!ler_texto(linha, 64)) {
            printf("\n[Erro] Entrada encerrada.\n");
            return;
        }

        while (linha[ini] == ' ' || linha[ini] == '\t') ini++;
        fim = strlen(linha);
        while (fim > ini && (linha[fim - 1] == ' ' || linha[fim - 1] == '\t')) fim--;

        valido = (ini < fim);
        k = ini;
        if (valido && (linha[k] == '+' || linha[k] == '-')) k++;
        if (k == fim) valido = 0;
        for (; valido && k < fim; k++) {
            if (linha[k] < '0' || linha[k] > '9') valido = 0;
        }

        int estouro = 0;
        if (valido) {
            int p = ini;
            char limite[] = "2147483647";
            if (linha[ini] == '-') limite[9] = '8';
            if (linha[p] == '+' || linha[p] == '-') p++;
            while (p < fim - 1 && linha[p] == '0') p++;
            if (fim - p > 10) {
                estouro = 1;
            } else if (fim - p == 10) {
                for (int t = 0; t < 10; t++) {
                    if (linha[p + t] > limite[t]) {
                        estouro = 1;
                        break;
                    }
                    if (linha[p + t] < limite[t]) break;
                }
            }
        }

        if (!valido) {
            printf("[Erro] ID invalido. Digite apenas numeros inteiros.\n");
            return;
        }
        if (estouro) {
            printf("[Erro] O ID informado e grande demais.\n");
            return;
        }

        linha[fim] = '\0';
        id_escolhido = atoi(linha + ini);
        if (id_escolhido <= 0) {
            printf("[Erro] O ID deve ser maior que zero.\n");
            return;
        }

        for (int i = 0; i < qtd; i++) {
            if (ids[posicoes[i]] == id_escolhido) {
                submenu_agente(posicoes[i]);
                return;
            }
        }
    }

    printf("\n[Aviso] Nenhum agente encontrado contendo '%s'.\n", termo);
}

/* =========================================================================
 * FUNÇÃO PRINCIPAL (MENU PRINCIPAL)
 * ========================================================================= */

int main(void) {
    int opcao = -1;
    int voltou_ao_menu = 0;
    int tem_opcao_adiantada = 0;
    int opcao_adiantada = -1;
    int aviso_menu = 0;

    /* Carrega os agentes do arquivo antes de mostrar o menu */
    carregar_arquivo();

    do {
        /* Ao voltar de uma opcao, limpa a tela antes de mostrar o menu */
        if (voltou_ao_menu) {
            limpar_console();
        }

        if (tem_opcao_adiantada) {
            opcao = opcao_adiantada;
            tem_opcao_adiantada = 0;
        } else {
            if (aviso_menu == 1) {
                printf("\n[Erro] Digite apenas numeros inteiros de 0 a 5.\n");
            } else if (aviso_menu == 2) {
                printf("\n[Erro] Opcao invalida! Digite um numero de 0 a 5.\n");
            }
            aviso_menu = 0;

            printf("\n=========================================\n");
            printf("       Compass.IA - CATALOGO DE IA       \n");
            printf("=========================================\n");
            printf("1. Cadastrar Novo Agente\n");
            printf("2. Listar Todos os Agentes\n");
            printf("3. Buscar Agente por ID\n");
            printf("4. Buscar Agente por Nome\n");
            printf("5. Filtrar por Nota Minima (Diferencial)\n");
            printf("0. Encerrar Programa\n");
            printf("-----------------------------------------\n");
            printf("Escolha uma opcao: ");
            {
                char linha[64];
                int ini = 0;
                int fim;
                int k;
                int valido;
                int estouro = 0;

                if (!ler_texto(linha, 64)) {
                    opcao = 0;
                    if (salvar_arquivo() == 0) {
                        printf("\nDados salvos! Encerrando o sistema...\n");
                    } else {
                        printf("Encerrando o sistema...\n");
                    }
                    break;
                }

                while (linha[ini] == ' ' || linha[ini] == '\t') ini++;
                fim = strlen(linha);
                while (fim > ini && (linha[fim - 1] == ' ' || linha[fim - 1] == '\t')) fim--;

                valido = (ini < fim);
                k = ini;
                if (valido && (linha[k] == '+' || linha[k] == '-')) k++;
                if (k == fim) valido = 0;
                for (; valido && k < fim; k++) {
                    if (linha[k] < '0' || linha[k] > '9') valido = 0;
                }

                if (valido) {
                    int p = ini;
                    char limite[] = "2147483647";
                    if (linha[ini] == '-') limite[9] = '8';
                    if (linha[p] == '+' || linha[p] == '-') p++;
                    while (p < fim - 1 && linha[p] == '0') p++;
                    if (fim - p > 10) {
                        estouro = 1;
                    } else if (fim - p == 10) {
                        for (int t = 0; t < 10; t++) {
                            if (linha[p + t] > limite[t]) {
                                estouro = 1;
                                break;
                            }
                            if (linha[p + t] < limite[t]) break;
                        }
                    }
                }

                if (!valido) {
                    opcao = -1;
                    printf("\n[Erro] Digite apenas numeros inteiros de 0 a 5.\n");
                } else if (estouro) {
                    opcao = -1;
                    printf("\n[Erro] Opcao invalida! Digite um numero de 0 a 5.\n");
                } else {
                    linha[fim] = '\0';
                    opcao = atoi(linha + ini);
                }
            }
        }

        if (opcao != -1) {
            switch (opcao) {
                case 1:
                    cadastrar_agente();
                    break;
                case 2:
                    listar_agentes();
                    break;
                case 3:
                    buscar_por_id();
                    break;
                case 4:
                    buscar_por_nome();
                    break;
                case 5:
                    filtrar_por_nota_minima();
                    break;
                case 0:
                    if (salvar_arquivo() == 0) {
                        printf("\nDados salvos! Encerrando o sistema...\n");
                    } else {
                        printf("Encerrando o sistema...\n");
                    }
                    break;
                default:
                    printf("\nOpcao invalida! Digite um numero de 0 a 5.\n");
                    break;
            }
        }

        if (opcao != 0) {
            char linha[64];
            int ini = 0;
            int fim;
            int k;
            int valido;
            int estouro = 0;

            printf("\nPressione Enter para voltar ao menu...");
            if (!ler_texto(linha, 64)) {
                opcao = 0;
                if (salvar_arquivo() == 0) {
                    printf("\nDados salvos! Encerrando o sistema...\n");
                } else {
                    printf("Encerrando o sistema...\n");
                }
            } else {
                while (linha[ini] == ' ' || linha[ini] == '\t') ini++;
                fim = strlen(linha);
                while (fim > ini && (linha[fim - 1] == ' ' || linha[fim - 1] == '\t')) fim--;

                valido = (ini < fim);
                k = ini;
                if (valido && (linha[k] == '+' || linha[k] == '-')) k++;
                if (k == fim) valido = 0;
                for (; valido && k < fim; k++) {
                    if (linha[k] < '0' || linha[k] > '9') valido = 0;
                }

                if (valido) {
                    int p = ini;
                    char limite[] = "2147483647";
                    if (linha[ini] == '-') limite[9] = '8';
                    if (linha[p] == '+' || linha[p] == '-') p++;
                    while (p < fim - 1 && linha[p] == '0') p++;
                    if (fim - p > 10) {
                        estouro = 1;
                    } else if (fim - p == 10) {
                        for (int t = 0; t < 10; t++) {
                            if (linha[p + t] > limite[t]) {
                                estouro = 1;
                                break;
                            }
                            if (linha[p + t] < limite[t]) break;
                        }
                    }
                }

                voltou_ao_menu = 1;
                if (!valido && ini != fim) {
                    aviso_menu = 1;
                } else if (estouro) {
                    aviso_menu = 2;
                } else if (valido) {
                    linha[fim] = '\0';
                    opcao_adiantada = atoi(linha + ini);
                    if (opcao_adiantada >= 0 && opcao_adiantada <= 5) {
                        tem_opcao_adiantada = 1;
                    } else {
                        aviso_menu = 2;
                    }
                }
            }
        }
    } while (opcao != 0);

    return 0;
}
