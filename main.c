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

/* Limpa o Enter (\n) que sobra no teclado após usar o scanf */
void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* Descarta o caractere lido */
    }
}

/* Lê texto com espaços (fgets) e retira a quebra de linha ('\n') do final */
void ler_texto(char texto[], int tamanho) {
    if (fgets(texto, tamanho, stdin) != NULL) {
        int tam = strlen(texto);
        if (tam > 0 && texto[tam - 1] == '\n') {
            texto[tam - 1] = '\0'; /* Substitui a quebra de linha pelo fim de string */
        } else {
            limpar_buffer();
        }
    }
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

/* Grava os dados separados nos dois arquivos conforme a modelagem (AGENTE e AVALIACAO) */
void salvar_arquivo(void) {
    FILE *arq_agentes = fopen(ARQUIVO_AGENTES, "w");
    FILE *arq_avaliacoes = fopen(ARQUIVO_AVALIACOES, "w");

    if (arq_agentes == NULL || arq_avaliacoes == NULL) {
        printf("\n[Erro] Nao foi possivel abrir os arquivos para salvar!\n");
        if (arq_agentes != NULL) fclose(arq_agentes);
        if (arq_avaliacoes != NULL) fclose(arq_avaliacoes);
        return;
    }

    for (int i = 0; i < total; i++) {
        /* 1. Grava no arquivo agentes.txt: Id_agente, Nome e Instrucoes */
        fprintf(arq_agentes, "%d\n", ids[i]);
        fprintf(arq_agentes, "%s\n", nomes[i]);
        fprintf(arq_agentes, "%s\n", instrucoes[i]);

        /* 2. Grava no arquivo avaliacoes.txt: Id_avaliacao, Nota e Id_agente (FK) */
        fprintf(arq_avaliacoes, "%d\n", ids_avaliacao[i]);
        fprintf(arq_avaliacoes, "%.1f\n", notas[i]);
        fprintf(arq_avaliacoes, "%d\n", ids[i]);
    }

    fclose(arq_agentes);
    fclose(arq_avaliacoes);
}

/* Lê os dados dos dois arquivos (agentes.txt e avaliacoes.txt) ao abrir o programa */
void carregar_arquivo(void) {
    FILE *arq_agentes = fopen(ARQUIVO_AGENTES, "r");
    if (arq_agentes == NULL) {
        /* Se o arquivo de agentes ainda não existir, o sistema começa vazio */
        return;
    }

    char linha_temp[TAM_INSTRUCOES];
    total = 0;

    /* 1. Carrega os dados da entidade AGENTE (agentes.txt) */
    while (total < MAX_AGENTES && fgets(linha_temp, sizeof(linha_temp), arq_agentes) != NULL) {
        ids[total] = atoi(linha_temp);

        /* Lê o Nome */
        if (fgets(nomes[total], TAM_NOME, arq_agentes) == NULL) break;
        int tam_n = strlen(nomes[total]);
        if (tam_n > 0 && nomes[total][tam_n - 1] == '\n') {
            nomes[total][tam_n - 1] = '\0';
        }

        /* Lê as Instruções */
        if (fgets(instrucoes[total], TAM_INSTRUCOES, arq_agentes) == NULL) break;
        int tam_i = strlen(instrucoes[total]);
        if (tam_i > 0 && instrucoes[total][tam_i - 1] == '\n') {
            instrucoes[total][tam_i - 1] = '\0';
        }

        /* Inicializa valores padrão da avaliação caso ainda não tenha avaliacoes.txt */
        ids_avaliacao[total] = total + 1;
        notas[total] = 0.0f;

        total++;
    }
    fclose(arq_agentes);

    /* 2. Carrega os dados da entidade AVALIACAO (avaliacoes.txt) e vincula pelo Id_agente */
    FILE *arq_avaliacoes = fopen(ARQUIVO_AVALIACOES, "r");
    if (arq_avaliacoes != NULL) {
        while (fgets(linha_temp, sizeof(linha_temp), arq_avaliacoes) != NULL) {
            int id_aval_lido = atoi(linha_temp);

            if (fgets(linha_temp, sizeof(linha_temp), arq_avaliacoes) == NULL) break;
            float nota_lida = atof(linha_temp);

            if (fgets(linha_temp, sizeof(linha_temp), arq_avaliacoes) == NULL) break;
            int id_agente_fk = atoi(linha_temp);

            /* Procura o agente correspondente (chave estrangeira Id_agente) */
            for (int i = 0; i < total; i++) {
                if (ids[i] == id_agente_fk) {
                    ids_avaliacao[i] = id_aval_lido;
                    notas[i] = nota_lida;
                    break;
                }
            }
        }
        fclose(arq_avaliacoes);
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
        printf("Digite o ID (numero positivo): ");
        if (scanf("%d", &id_digitado) != 1) {
            limpar_buffer();
            id_digitado = -1;
            printf("[Erro] Digite apenas numeros inteiros.\n");
            continue;
        }
        limpar_buffer();

        if (id_digitado <= 0) {
            printf("[Erro] O ID deve ser maior que zero.\n");
        } else if (id_ja_existe(id_digitado)) {
            printf("[Erro] O ID %d ja existe! Escolha outro.\n", id_digitado);
        }
    } while (id_digitado <= 0 || id_ja_existe(id_digitado));

    /* 2. Leitura do Nome */
    printf("Nome do Agente (max 80 caracteres): ");
    ler_texto(nomes[total], TAM_NOME);

    /* 3. Leitura das Instruções */
    printf("Script / Instrucoes (max 1000 caracteres):\n");
    ler_texto(instrucoes[total], TAM_INSTRUCOES);

    /* 4. Leitura e validação da Nota (0.0 a 5.0) */
    do {
        printf("Nota do Agente (de 0.0 a 5.0): ");
        if (scanf("%f", &nota_digitada) != 1) {
            limpar_buffer();
            nota_digitada = -1.0f;
            printf("[Erro] Digite um numero valido para a nota.\n");
            continue;
        }
        limpar_buffer();

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
        ids_avaliacao[total] = ids_avaliacao[total - 1] + 1;
    }
    notas[total] = nota_digitada;
    total++;

    /* Salva imediatamente nos dois arquivos (agentes.txt e avaliacoes.txt) */
    salvar_arquivo();
    printf("\n[Sucesso] Agente cadastrado e gravado com sucesso!\n");
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
        printf("ID: %d | Nome: %s | Nota: %.1f/5.0\n", ids[i], nomes[i], notas[i]);
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
    if (scanf("%f", &corte) != 1) {
        limpar_buffer();
        printf("[Erro] Valor invalido.\n");
        return;
    }
    limpar_buffer();

    if (corte < 0.0f || corte > 5.0f) {
        printf("[Erro] A nota deve estar entre 0.0 e 5.0!\n");
        return;
    }

    printf("\nAgentes com nota igual ou maior que %.1f:\n", corte);
    for (int i = 0; i < total; i++) {
        if (notas[i] >= corte) {
            printf("- ID: %d | Nome: %s | Nota: %.1f\n", ids[i], nomes[i], notas[i]);
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
    printf("Nota:        %.1f de 5.0\n", notas[pos]);
    printf("Instrucoes:  %s\n", instrucoes[pos]);
    printf("------------------------------------------------------------\n");
}

/* Permite editar Nome, Instruções e Nota */
void editar_agente(int pos) {
    float nova_nota;

    printf("\n--- EDITANDO O AGENTE (ID: %d) ---\n", ids[pos]);

    printf("Digite o novo Nome: ");
    ler_texto(nomes[pos], TAM_NOME);

    printf("Digite as novas Instrucoes:\n");
    ler_texto(instrucoes[pos], TAM_INSTRUCOES);

    do {
        printf("Digite a nova Nota (0.0 a 5.0): ");
        if (scanf("%f", &nova_nota) != 1) {
            limpar_buffer();
            nova_nota = -1.0f;
            printf("[Erro] Digite uma nota valida.\n");
            continue;
        }
        limpar_buffer();

        if (nova_nota < 0.0f || nova_nota > 5.0f) {
            printf("[Erro] A nota deve estar entre 0.0 e 5.0!\n");
        }
    } while (nova_nota < 0.0f || nova_nota > 5.0f);

    notas[pos] = nova_nota;

    salvar_arquivo();
    printf("\n[Sucesso] Registro atualizado e salvo!\n");
}

/* Exclui o agente deslocando os próximos registros para a esquerda (Shift) */
int excluir_agente(int pos) {
    char confirmacao;

    printf("\nTem certeza que deseja excluir o agente '%s' (ID: %d)? (S/N): ", nomes[pos], ids[pos]);
    scanf(" %c", &confirmacao);
    limpar_buffer();

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

        salvar_arquivo();
        printf("\n[Sucesso] Agente excluido com sucesso!\n");
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

        if (scanf("%d", &sub_opcao) != 1) {
            limpar_buffer();
            sub_opcao = -1;
            continue;
        }
        limpar_buffer();

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
    if (scanf("%d", &id_busca) != 1) {
        limpar_buffer();
        printf("[Erro] ID invalido.\n");
        return;
    }
    limpar_buffer();

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
    ler_texto(termo, TAM_NOME);

    /* Procura se o texto digitado existe em algum nome cadastrado */
    for (int i = 0; i < total; i++) {
        if (strstr(nomes[i], termo) != NULL) {
            submenu_agente(i); /* Encontrou: abre o submenu */
            return;
        }
    }

    printf("\n[Aviso] Nenhum agente encontrado contendo '%s'.\n", termo);
}

/* =========================================================================
 * FUNÇÃO PRINCIPAL (MENU PRINCIPAL)
 * ========================================================================= */

int main(void) {
    int opcao = -1;

    /* Carrega os agentes do arquivo antes de mostrar o menu */
    carregar_arquivo();

    do {
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

        if (scanf("%d", &opcao) != 1) {
            limpar_buffer();
            opcao = -1;
            continue;
        }
        limpar_buffer();

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
                salvar_arquivo();
                printf("\nDados salvos! Encerrando o sistema...\n");
                break;
            default:
                printf("\nOpcao invalida! Digite um numero de 0 a 5.\n");
                break;
        }
    } while (opcao != 0);

    return 0;
}
