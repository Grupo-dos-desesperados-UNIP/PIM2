#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =========================================================================
 * PROJETO INTEGRADO MULTIDISCIPLINAR II (PIM II) - UNIP
 * CURSO: CST em Análise e Desenvolvimento de Sistemas
 * TEMA: AI Agent Registry (Catálogo e Gerenciador de Agentes de IA)
 * 
 * ESPECIFICAÇÃO DE DADOS (VETORES PARALELOS):
 * - ids[]: Identificador único (int)
 * - nomes[][81]: Nome do agente (até 80 caracteres úteis + '\0')
 * - instrucoes[][1001]: Script/instruções em entrada única (até 1.000 caracteres úteis + '\0')
 * - notas[]: Avaliação da qualidade do agente (float, validada de 0.0 a 5.0)
 * Capacidade máxima: 50 registros simultâneos.
 * Diferencial de negócio: Filtros dinâmicos por nota mínima e nota máxima.
 * ========================================================================= */

#define MAX_AGENTES 50
#define TAM_NOME 81
#define TAM_INSTRUCOES 1001
#define NOME_ARQUIVO "agentes.txt"

/* --- Vetores Paralelos e Controle Global --- */
int ids[MAX_AGENTES];
char nomes[MAX_AGENTES][TAM_NOME];
char instrucoes[MAX_AGENTES][TAM_INSTRUCOES];
float notas[MAX_AGENTES];
int total = 0;

/* =========================================================================
 * UTILITÁRIOS E TRATAMENTO DE ENTRADA (I/O)
 * ========================================================================= */

/* Descarta caracteres remanescentes no buffer de entrada (stdin) */
void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* Apenas consome o fluxo pendente */
    }
}

/* Leitura segura com fgets, removendo a quebra de linha (\n) e limpando estouro */
void ler_texto(char *destino, int tamanho_max) {
    if (fgets(destino, tamanho_max, stdin) != NULL) {
        size_t len = strlen(destino);
        if (len > 0 && destino[len - 1] == '\n') {
            destino[len - 1] = '\0';
        } else {
            /* Se o texto digitado excedeu o tamanho máximo, limpa o restante do buffer */
            limpar_buffer();
        }
    }
}

/* Sanitiza o texto para o arquivo: substitui ';' por ',' para nao corromper delimitadores */
void sanitizar_delimitador(char *texto) {
    for (int i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == ';') {
            texto[i] = ',';
        }
    }
}

/* Busca linear para validação de unicidade de ID */
int id_ja_existe(int id_verificar) {
    for (int i = 0; i < total; i++) {
        if (ids[i] == id_verificar) {
            return 1; /* Já cadastrado */
        }
    }
    return 0; /* Disponível */
}

/* =========================================================================
 * PERSISTÊNCIA EM ARQUIVO (TXT)
 * ========================================================================= */

/* Grava todos os vetores paralelos em disco formatados por ';' */
void salvar_arquivo(void) {
    FILE *arquivo = fopen(NOME_ARQUIVO, "w");
    if (arquivo == NULL) {
        printf("\n[Erro] Nao foi possivel abrir o arquivo para gravacao.\n");
        return;
    }

    for (int i = 0; i < total; i++) {
        fprintf(arquivo, "%d;%s;%s;%.1f\n", ids[i], nomes[i], instrucoes[i], notas[i]);
    }

    fclose(arquivo);
}

/* Carrega os registros do arquivo texto na memória RAM de forma segura */
void carregar_arquivo(void) {
    FILE *arquivo = fopen(NOME_ARQUIVO, "r");
    if (arquivo == NULL) {
        /* Primeira execução ou arquivo inexistente: inicia catálogo com total = 0 */
        return;
    }

    char linha[TAM_NOME + TAM_INSTRUCOES + 128];
    total = 0;

    while (total < MAX_AGENTES && fgets(linha, sizeof(linha), arquivo) != NULL) {
        size_t len = strlen(linha);
        if (len > 0 && linha[len - 1] == '\n') {
            linha[len - 1] = '\0';
        }

        /* Divisão segura por ponteiros (delimitador ';') */
        char *p_id = strtok(linha, ";");
        char *p_nome = strtok(NULL, ";");
        char *p_instrucoes = strtok(NULL, ";");
        char *p_nota = strtok(NULL, ";");

        if (p_id && p_nome && p_instrucoes && p_nota) {
            ids[total] = atoi(p_id);
            strncpy(nomes[total], p_nome, TAM_NOME - 1);
            nomes[total][TAM_NOME - 1] = '\0';

            strncpy(instrucoes[total], p_instrucoes, TAM_INSTRUCOES - 1);
            instrucoes[total][TAM_INSTRUCOES - 1] = '\0';

            notas[total] = atof(p_nota);
            total++;
        }
    }

    fclose(arquivo);
}

/* =========================================================================
 * OPERAÇÕES: CADASTRO E LISTAGEM
 * ========================================================================= */

void cadastrar_agente(void) {
    int id_lido;
    char nome_lido[TAM_NOME];
    char instrucoes_lidas[TAM_INSTRUCOES];
    float nota_lida;

    if (total >= MAX_AGENTES) {
        printf("\n[Aviso] Capacidade maxima de %d agentes atingida!\n", MAX_AGENTES);
        return;
    }

    printf("\n=========================================\n");
    printf("        CADASTRAR NOVO AGENTE DE IA      \n");
    printf("=========================================\n");

    /* 1. ID único */
    do {
        printf("Digite o ID do Agente (numero positivo): ");
        if (scanf("%d", &id_lido) != 1) {
            limpar_buffer();
            id_lido = -1;
            printf("[Erro] Entrada invalida. Digite apenas numeros inteiros.\n");
            continue;
        }
        limpar_buffer();

        if (id_lido <= 0) {
            printf("[Erro] O ID deve ser estritamente positivo.\n");
        } else if (id_ja_existe(id_lido)) {
            printf("[Erro] O ID %d ja se encontra cadastrado. Escolha outro.\n", id_lido);
        }
    } while (id_lido <= 0 || id_ja_existe(id_lido));

    /* 2. Nome do Agente (até 80 caracteres) */
    printf("Digite o nome do Agente (max %d caracteres): ", TAM_NOME - 1);
    ler_texto(nome_lido, TAM_NOME);
    sanitizar_delimitador(nome_lido);

    /* 3. Script/Instruções (até 1.000 caracteres em entrada única) */
    printf("Digite o Script / Instrucoes do Agente (max %d caracteres):\n", TAM_INSTRUCOES - 1);
    ler_texto(instrucoes_lidas, TAM_INSTRUCOES);
    sanitizar_delimitador(instrucoes_lidas);

    /* 4. Nota (de 0.0 a 5.0) */
    do {
        printf("Digite a nota de avaliacao (0.0 a 5.0): ");
        if (scanf("%f", &nota_lida) != 1) {
            limpar_buffer();
            nota_lida = -1.0f;
            printf("[Erro] Digite um numero valido para a nota (ex.: 4.5).\n");
            continue;
        }
        limpar_buffer();

        if (nota_lida < 0.0f || nota_lida > 5.0f) {
            printf("[Erro] Nota fora da faixa permitida (0.0 a 5.0).\n");
        }
    } while (nota_lida < 0.0f || nota_lida > 5.0f);

    /* Gravação nos vetores paralelos */
    ids[total] = id_lido;
    strcpy(nomes[total], nome_lido);
    strcpy(instrucoes[total], instrucoes_lidas);
    notas[total] = nota_lida;
    total++;

    salvar_arquivo();
    printf("\n[Sucesso] Agente '%s' registrado e salvo em arquivo com sucesso!\n", nome_lido);
}

void listar_agentes(void) {
    if (total == 0) {
        printf("\n[Aviso] O catalogo esta vazio. Nenhum agente cadastrado.\n");
        return;
    }

    printf("\n=================================================================================\n");
    printf("                         CATALOGO GERAL DE AGENTES DE IA                         \n");
    printf("=================================================================================\n");
    for (int i = 0; i < total; i++) {
        printf("ID: %d | Nome: %s | Nota: %.1f/5.0\n", ids[i], nomes[i], notas[i]);
        printf("Instrucoes: %s\n", instrucoes[i]);
        printf("---------------------------------------------------------------------------------\n");
    }
    printf("Total de registros ativos: %d de %d\n", total, MAX_AGENTES);
}

/* =========================================================================
 * RECURSO DIFERENCIAL (EXIGÊNCIA DE NEGÓCIO DO MANUAL)
 * ========================================================================= */

void filtrar_por_nota_minima(void) {
    float corte;
    int encontrados = 0;

    if (total == 0) {
        printf("\n[Aviso] O catalogo esta vazio.\n");
        return;
    }

    printf("\n--- FILTRAR AGENTES POR NOTA MINIMA (DIFERENCIAL) ---\n");
    printf("Digite a nota de corte desejada (0.0 a 5.0): ");
    if (scanf("%f", &corte) != 1) {
        limpar_buffer();
        printf("[Erro] Entrada numerica invalida.\n");
        return;
    }
    limpar_buffer();

    printf("\n=================================================================================\n");
    printf("               AGENTES COM AVALIACAO IGUAL OU SUPERIOR A %.1f                    \n", corte);
    printf("=================================================================================\n");

    for (int i = 0; i < total; i++) {
        if (notas[i] >= corte) {
            printf("ID: %-4d | Nome: %-25s | Nota: %.1f\n", ids[i], nomes[i], notas[i]);
            printf("Script: %s\n", instrucoes[i]);
            printf("---------------------------------------------------------------------------------\n");
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum agente localizado com nota maior ou igual a %.1f.\n", corte);
    } else {
        printf("Total de agentes encontrados no filtro: %d\n", encontrados);
    }
}

void filtrar_por_nota_maxima(void) {
    float teto;
    int encontrados = 0;

    if (total == 0) {
        printf("\n[Aviso] O catalogo esta vazio.\n");
        return;
    }

    printf("\n--- FILTRAR AGENTES POR NOTA MAXIMA (TETO) ---\n");
    printf("Digite a nota maxima desejada (0.0 a 5.0): ");
    if (scanf("%f", &teto) != 1) {
        limpar_buffer();
        printf("[Erro] Entrada numerica invalida.\n");
        return;
    }
    limpar_buffer();

    printf("\n=================================================================================\n");
    printf("               AGENTES COM AVALIACAO IGUAL OU INFERIOR A %.1f                    \n", teto);
    printf("=================================================================================\n");

    for (int i = 0; i < total; i++) {
        if (notas[i] <= teto) {
            printf("ID: %-4d | Nome: %-25s | Nota: %.1f\n", ids[i], nomes[i], notas[i]);
            printf("Script: %s\n", instrucoes[i]);
            printf("---------------------------------------------------------------------------------\n");
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum agente localizado com nota menor ou igual a %.1f.\n", teto);
    } else {
        printf("Total de agentes encontrados no filtro: %d\n", encontrados);
    }
}

/* =========================================================================
 * DETALHES, SUBMENU, EDIÇÃO E EXCLUSÃO (SHIFT)
 * ========================================================================= */

void exibir_ficha_agente(int indice) {
    if (indice < 0 || indice >= total) {
        return;
    }

    printf("\n=================================================================================\n");
    printf("                              FICHA DO AGENTE DE IA                              \n");
    printf("=================================================================================\n");
    printf("ID:                  %d\n", ids[indice]);
    printf("Nome:                %s\n", nomes[indice]);
    printf("Nota:                %.1f de 5.0\n", notas[indice]);
    printf("---------------------------------------------------------------------------------\n");
    printf("Script / Instrucoes:\n%s\n", instrucoes[indice]);
    printf("=================================================================================\n");
}

void editar_agente(int indice) {
    float nova_nota;

    printf("\n--- EDITANDO REGISTRO (ID: %d) ---\n", ids[indice]);

    printf("Digite o novo Nome: ");
    ler_texto(nomes[indice], TAM_NOME);
    sanitizar_delimitador(nomes[indice]);

    printf("Digite o novo Script / Instrucoes:\n");
    ler_texto(instrucoes[indice], TAM_INSTRUCOES);
    sanitizar_delimitador(instrucoes[indice]);

    do {
        printf("Digite a nova Nota (0.0 a 5.0): ");
        if (scanf("%f", &nova_nota) != 1) {
            limpar_buffer();
            nova_nota = -1.0f;
            printf("[Erro] Valor invalido.\n");
            continue;
        }
        limpar_buffer();

        if (nova_nota < 0.0f || nova_nota > 5.0f) {
            printf("[Erro] Nota fora da faixa permitida (0.0 a 5.0).\n");
        }
    } while (nova_nota < 0.0f || nova_nota > 5.0f);

    notas[indice] = nova_nota;

    salvar_arquivo();
    printf("\n[Sucesso] Registro atualizado e persistido com sucesso!\n");
}

void excluir_agente(int indice) {
    char confirmacao;

    printf("\nConfirma a exclusao definitiva do agente '%s' (ID: %d)? (S/N): ", nomes[indice], ids[indice]);
    if (scanf(" %c", &confirmacao) != 1) {
        limpar_buffer();
        return;
    }
    limpar_buffer();

    if (confirmacao == 'S' || confirmacao == 's') {
        /* Desloca todos os registros posteriores uma posição à esquerda (shift) */
        for (int j = indice; j < total - 1; j++) {
            ids[j] = ids[j + 1];
            strcpy(nomes[j], nomes[j + 1]);
            strcpy(instrucoes[j], instrucoes[j + 1]);
            notas[j] = notas[j + 1];
        }
        total--;

        salvar_arquivo();
        printf("\n[Sucesso] Agente removido e arquivo sincronizado!\n");
    } else {
        printf("\n[Cancelado] Operacao de exclusao cancelada pelo usuario.\n");
    }
}

void gerenciar_agente_encontrado(int indice) {
    int opcao;

    exibir_ficha_agente(indice);

    do {
        printf("\n--- GERENCIAR AGENTE LOCALIZADO ---\n");
        printf("1. Editar dados do agente\n");
        printf("2. Excluir este agente\n");
        printf("0. Voltar ao menu principal\n");
        printf("Escolha uma acao: ");

        if (scanf("%d", &opcao) != 1) {
            limpar_buffer();
            opcao = -1;
            continue;
        }
        limpar_buffer();

        switch (opcao) {
            case 1:
                editar_agente(indice);
                exibir_ficha_agente(indice);
                break;
            case 2:
                excluir_agente(indice);
                return; /* Retorna pois o registro foi deletado */
            case 0:
                return;
            default:
                printf("\nOpcao invalida. Digite 1, 2 ou 0.\n");
                break;
        }
    } while (opcao != 0);
}

/* =========================================================================
 * BUSCAS
 * ========================================================================= */

void buscar_por_id(void) {
    int id_procurado;

    if (total == 0) {
        printf("\n[Aviso] O catalogo esta vazio.\n");
        return;
    }

    printf("\n========== BUSCA POR ID ==========\n");
    printf("Digite o ID que deseja pesquisar: ");
    if (scanf("%d", &id_procurado) != 1) {
        limpar_buffer();
        printf("\n[Erro] Entrada invalida. Digite um numero inteiro.\n");
        return;
    }
    limpar_buffer();

    for (int i = 0; i < total; i++) {
        if (ids[i] == id_procurado) {
            gerenciar_agente_encontrado(i);
            return;
        }
    }

    printf("\n[Aviso] Nenhum agente encontrado com o ID %d.\n", id_procurado);
}

void buscar_por_nome(void) {
    char termo[TAM_NOME];
    int encontrados = 0;

    if (total == 0) {
        printf("\n[Aviso] O catalogo esta vazio.\n");
        return;
    }

    printf("\n========== BUSCA POR NOME ==========\n");
    printf("Digite o nome ou trecho do nome: ");
    ler_texto(termo, TAM_NOME);

    for (int i = 0; i < total; i++) {
        if (strstr(nomes[i], termo) != NULL) {
            gerenciar_agente_encontrado(i);
            encontrados++;
            return; /* Localizou a primeira correspondência e abriu o gerenciador */
        }
    }

    if (!encontrados) {
        printf("\n[Aviso] Nenhum agente localizado com o termo '%s'.\n", termo);
    }
}

/* =========================================================================
 * FUNÇÃO PRINCIPAL (MAIN)
 * ========================================================================= */

int main(void) {
    int opcao = -1;

    /* Recupera os dados persistidos em disco antes de abrir a interface */
    carregar_arquivo();

    do {
        printf("\n=========================================\n");
        printf("   AI AGENT REGISTRY - CATALOGO DE IA    \n");
        printf("=========================================\n");
        printf("1. Cadastrar Novo Agente\n");
        printf("2. Listar Todos os Agentes\n");
        printf("3. Buscar Agente por ID\n");
        printf("4. Buscar Agente por Nome\n");
        printf("5. Filtrar por Nota Minima\n");
        printf("6. Filtrar por Nota Maxima\n");
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
            case 6:
                filtrar_por_nota_maxima();
                break;
            case 0:
                salvar_arquivo();
                printf("\nEncerrando o sistema. Dados sincronizados com sucesso!\n");
                break;
            default:
                printf("\nOpcao invalida. Digite um numero de 0 a 6.\n");
                break;
        }
    } while (opcao != 0);

    return 0;
}
