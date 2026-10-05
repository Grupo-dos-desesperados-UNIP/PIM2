#include <stdio.h>
#include <string.h>

/*
 * Catalogo e gerenciador de configuracoes de agentes de IA.
 * Programa de um unico arquivo, para iniciante.
 *
 * O mesmo indice i e o mesmo agente em todos os vetores.
 * Compilar com: gcc main.c -o sistema
 *
 * TODO (Preparacao) [RESPONSAVEL: Dev 1 - Lider/Engenheiro]:
 * Revisar a arquitetura dos vetores paralelos, validar a compilacao
 * e realizar o Code Review da integracao entre todos os membros.
 * Usar for, while, if/else, switch, printf, scanf, fgets, fopen, fprintf e fclose.
 */

#define MAX 50
#define ARQUIVO "agentes.txt"

int ids[MAX];
char nomes[MAX][50];
char modelos[MAX][30];
char prompts[MAX][200];
float temperaturas[MAX];
int tokens[MAX];
int total = 0;

void limpar_buffer(void) {
    /*
     * TODO (Teclado) [RESPONSAVEL: Dev 2 - Cadastro]:
     * Descartar o Enter que fica sobrando depois de um scanf.
     * Ler caractere por caractere com getchar ate encontrar '\n' ou EOF.
     */
}

void carregar_arquivo(void) {
    /*
     * TODO (Abrir o catalogo ao iniciar) [RESPONSAVEL: Dev 5 - Arquivos]:
     * 1. Abrir ARQUIVO com fopen no modo "r".
     * 2. Se o arquivo nao existir, avisar na tela e manter total = 0.
     * 3. Se existir, ler linha por linha.
     *    Cada linha tem: id;nome;modelo;prompt;temperatura;tokens
     * 4. Guardar cada campo no indice total:
     *    ids, nomes, modelos, prompts, temperaturas e tokens.
     * 5. Somar 1 em total a cada agente lido, sem passar de MAX.
     * 6. Fechar o arquivo com fclose.
     */
}

void salvar_arquivo(void) {
    /*
     * TODO (Gravar o catalogo) [RESPONSAVEL: Dev 5 - Arquivos]:
     * 1. Abrir ARQUIVO com fopen no modo "w".
     * 2. Com um for de 0 ate total - 1, gravar uma linha por agente:
     *    id;nome;modelo;prompt;temperatura;tokens e uma quebra de linha.
     * 3. Usar fprintf.
     * 4. Fechar o arquivo com fclose.
     */
}

void cadastrar(void) {
    /*
     * TODO (Cadastrar um agente) [RESPONSAVEL: Dev 2 - Cadastro]:
     * 1. Se total ja for igual a MAX, avisar que o catalogo esta cheio e voltar.
     * 2. O id do novo agente e total + 1. Guardar em ids[total].
     * 3. Pedir nome, modelo e prompt.
     *    A leitura precisa aceitar espacos e tirar o '\n' do final.
     *    Chamar limpar_buffer quando o scanf anterior deixar Enter sobrando.
     * 4. Pedir a temperatura. Com while, repetir enquanto o valor
     *    estiver fora do intervalo de 0.0 a 2.0.
     * 5. Pedir os tokens. Com while, repetir enquanto o valor nao for maior que 0.
     * 6. Fazer total++.
     * 7. Chamar salvar_arquivo().
     */
}

void listar(void) {
    /*
     * TODO (Mostrar os agentes) [RESPONSAVEL: Dev 3 - Consulta]:
     * 1. Se total for 0, avisar que a lista esta vazia.
     * 2. Senao, usar um for de 0 ate total - 1.
     * 3. Em cada volta, mostrar de forma organizada:
     *    id, nome, modelo, prompt, temperatura e tokens.
     */
}

void editar(void) {
    /*
     * TODO (Atualizar agente) [RESPONSAVEL: Dev 4 - Exclusao/Edicao]:
     * 1. Pedir o ID do agente a ser modificado.
     * 2. Localizar o indice correspondente no vetor ids[].
     * 3. Permitir atualizar novo modelo, novo prompt, nova temperatura e novos tokens.
     * 4. Chamar salvar_arquivo().
     */
}

void excluir(void) {
    /*
     * TODO (Remover um agente) [RESPONSAVEL: Dev 4 - Exclusao]:
     * 1. Pedir o ID.
     * 2. Com um for, procurar em qual indice esse ID esta. Guardar em pos.
     * 3. Se nao encontrar, avisar e voltar.
     * 4. Se encontrar, deslocar todo mundo uma posicao para a esquerda,
     *    do indice pos ate total - 2, para fechar o buraco.
     *    Em cada posicao j, copiar os dados de j + 1:
     *    ids, nomes (strcpy), modelos (strcpy), prompts (strcpy),
     *    temperaturas e tokens.
     * 5. Fazer total--.
     * 6. Chamar salvar_arquivo().
     * 7. Avisar que o agente foi removido.
     */
}

int main(void) {
    int opcao = -1;

    /* TODO (Inicio) [RESPONSAVEL: Dev 1 - Lider/Engenheiro]: chamar carregar_arquivo() antes do menu. */

    do {
        printf("\nCatalogo de Agentes de IA\n");
        printf("1. Cadastrar Agente\n");
        printf("2. Listar Agentes\n");
        printf("3. Excluir Agente\n");
        printf("4. Salvar Alteracoes\n");
        printf("0. Sair\n");
        printf("Opcao: ");

        if (scanf("%d", &opcao) != 1) {
            limpar_buffer();
            opcao = -1;
            continue;
        }
        limpar_buffer();

        switch (opcao) {
        case 1:
            /* TODO (Menu) [RESPONSAVEL: Dev 1 - Lider/Engenheiro]: chamar cadastrar(). */
            break;
        case 2:
            /* TODO (Menu) [RESPONSAVEL: Dev 1 - Lider/Engenheiro]: chamar listar(). */
            break;
        case 3:
            /* TODO (Menu) [RESPONSAVEL: Dev 1 - Lider/Engenheiro]: chamar excluir(). */
            break;
        case 4:
            /* TODO (Menu) [RESPONSAVEL: Dev 1 - Lider/Engenheiro]: chamar salvar_arquivo(). */
            break;
        case 0:
            /* TODO (Menu) [RESPONSAVEL: Dev 1 - Lider/Engenheiro]: chamar salvar_arquivo() e deixar o laco terminar. */
            break;
        default:
            printf("Opcao invalida.\n");
            break;
        }
    } while (opcao != 0);

    return 0;
}