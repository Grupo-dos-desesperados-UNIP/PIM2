#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =========================================================================
 * PROJETO INTEGRADO MULTIDISCIPLINAR II (PIM II) - UNIP
 * TEMA: AI Agent Registry (Catálogo e Gerenciador de Agentes de IA)
 * 
 * VETORES PARALELOS (MÁXIMO 50 REGISTROS):
 * - ids[]: Identificador único (int)
 * - nomes[][81]: Nome do agente (até 80 caracteres + '\0')
 * - instrucoes[][1001]: Script/prompts (até 1000 caracteres + '\0')
 * - notas[]: Avaliação da qualidade do agente (float/int de 0.0 a 5.0)
 * ========================================================================= */

#define MAX_AGENTES 50
#define TAM_NOME 81
#define TAM_INSTRUCOES 1001
#define NOME_ARQUIVO "agentes.txt"

/* --- Vetores Paralelos e Controle de Registros --- */
int ids[MAX_AGENTES];
char nomes[MAX_AGENTES][TAM_NOME];
char instrucoes[MAX_AGENTES][TAM_INSTRUCOES];
float notas[MAX_AGENTES]; /* Vetor obrigatório de notas de 0 a 5 */
int total = 0;

/* =========================================================================
 * UTILITÁRIOS E LEITURA DE DADOS
 * ========================================================================= */

void limpar_buffer(void) {
    /* 
     * [Limpeza de Buffer]:
     * - Consumir caracteres com getchar() em um while até achar '\n' ou EOF.
     * - Evita que 'Enters' pendentes de scanf pulem a leitura de textos seguintes.
     */
     int c;
    /* Consome todos os caracteres restantes na entrada ate o Enter (\n) ou fim de arquivo */
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descarta */
    }
}

void ler_texto(char *destino, int tamanho_max) {
    /* 
     * TODO [Entrada Segura de Texto com Espaços]:
     * - Usar fgets(destino, tamanho_max, stdin) para ler nomes e scripts com espaços.
     * - Remover o '\n' gerado pelo fgets trocando-o por '\0'.
     * - Se o texto exceder o tamanho máximo e não houver '\n', chamar limpar_buffer().
     */
}

int id_ja_existe(int id_verificar) {
    /* 
     * TODO [Validação de ID Único]:
     * - Percorrer o vetor ids de 0 até total - 1.
     * - Se ids[i] == id_verificar, retornar 1 (já existe).
     * - Se o laço encerrar sem encontrar correspondência, retornar 0 (disponível).
     */
    return 0;
}

/* =========================================================================
 * PERSISTÊNCIA EM ARQUIVO (TXT)
 * ========================================================================= */

void carregar_arquivo(void) {
    /* 
     * TODO [Persistência - Leitura Inicial do Arquivo]:
     * 1. Abrir NOME_ARQUIVO em modo leitura ("r") com fopen.
     * 2. Se for NULL, emitir aviso de que o arquivo ainda não existe (início com total = 0).
     * 3. Se abrir com sucesso, executar um laço lendo cada linha com separador (ex: ';' ou '\n').
     *    - Cada registro deve carregar: ID, Nome, Script/Instruções e a NOTA do agente.
     * 4. Preencher na posição [total]:
     *    - ids[total]
     *    - nomes[total]
     *    - instrucoes[total]
     *    - notas[total] (convertido para float)
     * 5. Incrementar total++ a cada agente carregado, respeitando o limite MAX_AGENTES.
     * 6. Fechar o arquivo com fclose.
     */

    FILE *arquivo = fopen(NOME_ARQUIVO, "r");

    if (arquivo == NULL) {
        perror ("Não foi possível abrir o arquivo");
        return;
    } else {
        char linha[TAM_NOME + TAM_INSTRUCOES + 64];
        
        while(total < MAX_AGENTES && fgets(linha, sizeof(linha), arquivo) != NULL) {
            int lidos = sscanf(linha, "%d;%80[^;];%1000[^;];%f",
                    &ids[total],
                    nomes[total],
                    instrucoes[total],
                    &notas[total]);
        
        if (lidos == 4) {
            total++;
        } else {
            fprintf(stderr, "Linha inválida ignorada: %s", linha);
        }
    }
    fclose(arquivo);
    }
}

void salvar_arquivo(void) {
    /* 
     * TODO [Persistência - Gravação em Disco]:
     * 1. Abrir NOME_ARQUIVO em modo escrita ("w") com fopen.
     * 2. Se for NULL, acusar erro de abertura de arquivo.
     * 3. Executar laço for de 0 até total - 1 gravando linha a linha com fprintf:
     *    - Formato: id;nome;instrucoes;nota\n
     *    - Gravar a nota com precisão de uma casa decimal (ex: %.1f).
     * 4. Fechar o arquivo com fclose ao término.
     */
    
    FILE *arquivo = fopen(NOME_ARQUIVO, "w");
    
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }
    for (int i = 0; i < total; i++) {
        fprintf(arquivo,"%d;%s;%s; %.1f\n", 
            ids[i], nomes[i], instrucoes[i], notas[i]);
    }
    fclose(arquivo);
}
    


/* =========================================================================
 * CADASTRO E LISTAGEM
 * ========================================================================= */

void cadastrar_agente(void) {
    /* 
     * TODO [Operação: Cadastro de Agente]:
     * 1. Validar se total < MAX_AGENTES. Se total == 50, alertar limite e voltar.
     * 2. Solicitar o ID:
     *    - Ler com scanf e validar se o ID é positivo.
     *    - Usar id_ja_existe() em um laço while para impedir duplicidade de identificador.
     * 3. Solicitar o Nome do agente (até 80 caracteres) usando ler_texto().
     * 4. Solicitar o Script/Instruções (até 1000 caracteres, entrada única) usando ler_texto().
     * 5. Solicitar a NOTA do agente:
     *    - Exigir nota no intervalo entre 0.0 e 5.0.
     *    - Executar laço do-while que repete a pergunta enquanto o valor for < 0.0 ou > 5.0.
     * 6. Atribuir os dados recebidos aos vetores paralelos na posição [total]:
     *    - ids[total] = id_lido;
     *    - strcpy(nomes[total], nome_lido);
     *    - strcpy(instrucoes[total], instrucoes_lidas);
     *    - notas[total] = nota_lida;
     * 7. Incrementar total++.
     * 8. Chamar salvar_arquivo() para manter o arquivo de texto sincronizado.
     */
}

void listar_agentes(void) {
    /* 
     * TODO [Operação: Listagem Geral]:
     * 1. Se total == 0, informar que o catálogo está vazio e retornar.
     * 2. Imprimir cabeçalho formatado na tela.
     * 3. Percorrer os vetores de 0 até total - 1 exibindo:
     *    - ID do agente;
     *    - Nome do agente;
     *    - Script / Instruções cadastradas;
     *    - NOTA atribuída ao agente (exibida formatada: %.1f de 5.0).
     * 4. Exibir o total geral de agentes cadastrados.
     */
    int i;

    if (total == 0) {
        printf("\nO catalogo esta vazio.\n");
        return;
    }

    printf("\n========== AGENTES CADASTRADOS ==========\n");
    for (i = 0; i < total; i++) {
        printf("\nID: %d\n", ids[i]);
        printf("Nome: %s\n", nomes[i]);
        printf("Script / Instrucoes: %s\n", instrucoes[i]);
        printf("Nota: %.1f de 5.0\n", notas[i]);
    }
    printf("\nTotal de agentes cadastrados: %d\n", total);
}

/* =========================================================================
 * CONSULTAS, SUBMENU DE GERENCIAMENTO, EDIÇÃO E EXCLUSÃO
 * ========================================================================= */

void exibir_ficha_agente(int indice) {
    /* 
     * TODO [Exibição Detalhada do Registro Localizado]:
     * 1. Imprimir bloco com todos os dados do agente localizado no índice:
     *    - ID: ids[indice]
     *    - Nome: nomes[indice]
     *    - Script/Instruções: instrucoes[indice]
     *    - Nota: notas[indice] (ex: 4.5 / 5.0)
     */
    printf("\n========== FICHA DO AGENTE ==========\n");
    printf("ID: %d\n", ids[indice]);
    printf("Nome: %s\n", nomes[indice]);
    printf("Script / Instrucoes: %s\n", instrucoes[indice]);
    printf("Nota: %.1f / 5.0\n", notas[indice]);
}

void editar_agente(int indice) {
    /* 
     * TODO [Operação: Edição de Dados]:
     * 1. Informar os campos permitidos para alteração (Nome, Script e Nota).
     *    (O ID é mantido inalterado para preservar a chave primária).
     * 2. Ler novo Nome e atualizar em nomes[indice].
     * 3. Ler novo Script/Instruções e atualizar em instrucoes[indice].
     * 4. Ler nova NOTA do agente:
     *    - Validar obrigatoriamente se a nova nota permanece entre 0.0 e 5.0.
     *    - Atualizar notas[indice] com o novo valor.
     * 5. Chamar salvar_arquivo() para persistir a modificação.
     * 6. Informar ao usuário que a atualização foi concluída com sucesso.
     */
}

void excluir_agente(int indice) {
    /* 
     * TODO [Operação: Exclusão com Deslocamento de Vetores Paralelos]:
     * 1. Pedir confirmação do usuário (S/N) antes de remover.
     * 2. Se confirmado:
     *    - Executar laço for de j = indice até total - 2.
     *    - Puxar todos os elementos da posição j + 1 para a posição j em TODOS os vetores:
     *        ids[j] = ids[j + 1];
     *        strcpy(nomes[j], nomes[j + 1]);
     *        strcpy(instrucoes[j], instrucoes[j + 1]);
     *        notas[j] = notas[j + 1];  <-- Deslocar a nota para fechar a lacuna
     *    - Decrementar o contador global: total--;
     *    - Chamar salvar_arquivo() para consolidar a remoção no txt.
     *    - Exibir mensagem informando a exclusão.
     * 3. Se cancelado, abortar o processo e retornar.
     */
}

void gerenciar_agente_encontrado(int indice) {
    /* 
     * TODO [Submenu Contextual Pós-Busca - Requisito do Manual]:
     * 1. Chamar exibir_ficha_agente(indice) para mostrar a ficha completa (inclusive a nota).
     * 2. Apresentar submenu com as opções:
     *    [1] Editar dados do agente (incluindo nota)
     *    [2] Excluir este agente
     *    [0] Voltar ao menu principal
     * 3. Ler a opção com validação de entrada.
     * 4. Executar switch-case:
     *    - Caso 1: Chamar editar_agente(indice) e reexibir a ficha atualizada.
     *    - Caso 2: Chamar excluir_agente(indice) e quebrar o submenu (pois o registro foi apagado).
     *    - Caso 0: Retornar ao menu principal.
     */
}

void buscar_por_id(void) {
    /* 
     * TODO [Operação: Busca por ID]:
     * 1. Se total == 0, avisar catálogo vazio e retornar.
     * 2. Solicitar o ID desejado ao usuário.
     * 3. Percorrer o vetor ids de 0 até total - 1.
     * 4. Se ids[i] == id_digitado:
     *    - Acionar gerenciar_agente_encontrado(i).
     *    - Encerrar a busca.
     * 5. Se o laço terminar sem encontrar, informar que o ID não foi cadastrado.
     */
}

void buscar_por_nome(void) {
    /* 
     * TODO [Operação: Busca por Nome]:
     * 1. Se total == 0, avisar catálogo vazio e retornar.
     * 2. Solicitar o nome ou fragmento a ser pesquisado.
     * 3. Percorrer o vetor nomes comparando com strstr() para permitir busca flexível.
     * 4. Se localizar correspondência:
     *    - Acionar gerenciar_agente_encontrado(i).
     * 5. Se nenhuma ocorrência for encontrada, avisar que não há agente com o termo informado.
     */
}

/* =========================================================================
 * FUNÇÃO PRINCIPAL (MAIN)
 * ========================================================================= */

int main(void) {
    int opcao = -1;

    /* 
     * TODO [Inicialização]:
     * Executar carregar_arquivo() para recuperar os registros e notas salvos anteriormente.
     */

    do {
        printf("\n=========================================\n");
        printf("   AI AGENT REGISTRY - CATALOGO DE IA    \n");
        printf("=========================================\n");
        printf("1. Cadastrar Novo Agente\n");
        printf("2. Listar Agentes Cadastrados\n");
        printf("3. Buscar Agente por ID\n");
        printf("4. Buscar Agente por Nome\n");
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
                /* TODO: Chamar cadastrar_agente(); */
                break;
            case 2:
                /* TODO: Chamar listar_agentes(); */
                break;
            case 3:
                /* TODO: Chamar buscar_por_id(); */
                break;
            case 4:
                /* TODO: Chamar buscar_por_nome(); */
                break;
            case 0:
                /* 
                 * TODO [Finalização]:
                 * Confirmar gravação com salvar_arquivo() antes da saída.
                 */
                printf("\nEncerrando o programa e sincronizando dados...\n");
                break;
            default:
                printf("\nOpcao invalida. Digite uma opcao de 0 a 4.\n");
                break;
        }

    } while (opcao != 0);

    return 0;
}
